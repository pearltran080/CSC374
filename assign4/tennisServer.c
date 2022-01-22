/*-------------------------------------------------------------------------*
 *---									---*
 *---		tennisServer.c						---*
 *---									---*
 *---	    This file defines a C program that, when told to by a	---*
 *---	a client over a socket, starts a virtual game of tennis.	---*
 *---	It then reports the score back to the client.			---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

//	Compile with:
//	$ gcc tennisServer.c -o tennisServer -lpthread

//---		Header file inclusion					---//

#include	"clientServer.h"
#include	<pthread.h>	// For pthread_create()


//---		Definition of constants:				---//

#define		STD_BYE_MSG		"Good bye!"

const int	STD_BYE_MSG_LEN		= sizeof(STD_BYE_MSG) - 1;

const int	ERROR_FD		= -1;


//---		Definition of global vars:				---//

//  PURPOSE:  To be non-zero for as long as this program should run, or '0'
//	otherwise.


//---		Definition of functions:				---//

//  PURPOSE:  To:
//	(1) Put -1 in network endian in both serverScore and receiverScore
//	(2) Make a pipe
//	(3) fork() a child process.  This child process will:
//	  (3a) close() the input end of the pipe
//	  (3b) redirect the output end of the pipe to STDOUT_FILENO
//	  (3c) Run refereee
//	  (3d) If running the referee fails then
// 		   printf("-1 -1\n");
//	       and exit() with EXIT_FAILURE
//	(4) The parent process should
//	  (4a) close() the output end of the pipe
//	  (4b) get a buffer of text from the input end of the pipe
//	  (4c) parse integers serverScore and receiverScore from the text buffer
//	  (4d) convert both integers to network endian
//	  (4e) send the serverScore and receiverScore back to the client
//	  (4f) wait() for the child process to end
//	If any step fails then send serverScore and receiverScore back to the
//	client with their -1 in network endian values.
void		playTennis	(int		toClientFd
				)
{

  int	serverScore	= htonl(-1);	// <-- CHANGE THAT 0!
  int	receiverScore	= htonl(-1);	// <-- CHANGE THAT 0!
  pid_t	refereePid;
  int	fromChildPipe[2];

  int	status;
  char	buffer[BUFFER_LEN];

	pipe(fromChildPipe);
	refereePid = fork();

	if(refereePid == 0) {

		close(fromChildPipe[0]);
		dup2(fromChildPipe[1], STDOUT_FILENO);

		execl("referee", "referee", NULL);
		printf("-1 -1\n");
		exit(EXIT_FAILURE);

	} else if (refereePid > 0) {

		close(fromChildPipe[1]);
		read(fromChildPipe[0], buffer, BUFFER_LEN);
		sscanf(buffer, "%d %d", &serverScore, &receiverScore);

		status = htonl(serverScore);
		write(toClientFd,&status,sizeof(status));
		status = htonl(receiverScore);
		write(toClientFd,&status,sizeof(status));

		wait(NULL);

	} else {

		write(toClientFd, &serverScore, sizeof(serverScore));
		write(toClientFd, &receiverScore, sizeof(receiverScore));
		exit(EXIT_FAILURE);

	}

}


//  PURPOSE:  To cast 'vPtr' to the pointer type coming from 'doServer()'
//	that points to two integers.  Then, to use those two integers,
//	one as a file descriptor, the other as a thread number, to fulfill
//	requests coming from the client over the file descriptor socket.
//	Returns 'NULL'.
void*		handleClient	(void*		vPtr
				)
{
  //  D.1.
  int*	 intArray		= (int*)vPtr;		// <-- CHANGE THAT NULL!
  int 	 fd					= intArray[0];		// <-- CHANGE THAT 0!
  int	   threadNum	= intArray[1];		// <-- CHANGE THAT 0!

  //  YOUR CODE TO release the memory
	free(intArray);

  //  D.2.
  char	buffer[BUFFER_LEN];

  printf("Thread %d starting.\n",threadNum);
	read(fd, buffer, BUFFER_LEN);
  //  D.3.

	if  (strcmp(buffer,PLAY_CMD) == 0)
    playTennis(fd);
  else
  {
    int	serverScore	= -1;	// <-- CHANGE THAT 0!
    int	receiverScore	= -1;	// <-- CHANGE THAT 0!

		 int toSend = htonl(serverScore);
		 write(fd,&toSend,sizeof(toSend));
		 toSend = htonl(receiverScore);
		 write(fd,&toSend,sizeof(toSend));
  }

  //  D.4.
  printf("Thread %d quitting.\n",threadNum);
	close(fd);
  return(NULL);
}


//  PURPOSE:  To run the server by 'accept()'-ing client requests from
//	'listenFd' and doing them.
void		doServer	(int		listenFd
				)
{
  pthread_t		threadId;
  pthread_attr_t	threadAttr;
  int			threadCount	= 0;

  //  YOUR CODE HERE
	pthread_attr_init(&threadAttr);
	pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED);

  while  (1)
  {
    int* clientFdPtr 	= (int*)calloc(2, sizeof(int));;		// <-- CHANGE THAT NULL
    //  YOUR CODE HERE

		clientFdPtr[0] = accept(listenFd,NULL,NULL);
		clientFdPtr[1] = threadCount++;

		pthread_create(&threadId, &threadAttr, handleClient, (void*)clientFdPtr);
  }

  //  YOUR CODE HERE
	pthread_attr_destroy(&threadAttr);

}


//  PURPOSE:  To decide a port number, either from the command line arguments
//	'argc' and 'argv[]', or by asking the user.  Returns port number.
int		getPortNum	(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Get listening socket:
  int	portNum;

  if  (argc >= 2)
    portNum	= strtol(argv[1],NULL,0);
  else
  {
    char	buffer[BUFFER_LEN];

    printf("Port number to monopolize? ");
    fgets(buffer,BUFFER_LEN,stdin);
    portNum	= strtol(buffer,NULL,0);
  }

  //  III.  Finished:
  return(portNum);
}


//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//	to the OS telling this server when a client process has connect()-ed
//	to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int		getServerFileDescriptor
				(int		port
				)
{
  //  I.  Application validity check:

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET, // AF_INET domain
			        SOCK_STREAM, // Reliable TCP
			        0);

  if  (socketDescriptor < 0)
  {
    perror("socket()");
    return(ERROR_FD);
  }

  //  II.B.  Attempt to bind 'socketDescriptor' to 'port':
  //  II.B.1.  We'll fill in this datastruct
  struct sockaddr_in socketInfo;

  //  II.B.2.  Fill socketInfo with 0's
  memset(&socketInfo,'\0',sizeof(socketInfo));

  //  II.B.3.  Use TCP/IP:
  socketInfo.sin_family = AF_INET;

  //  II.B.4.  Tell port in network endian with htons()
  socketInfo.sin_port = htons(port);

  //  II.B.5.  Allow machine to connect to this service
  socketInfo.sin_addr.s_addr = INADDR_ANY;

  //  II.B.6.  Try to bind socket with port and other specifications
  int status = bind(socketDescriptor, // from socket()
		    (struct sockaddr*)&socketInfo,
		    sizeof(socketInfo)
		   );

  if  (status < 0)
  {
    perror("bind()");
    return(ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor,5);

  //  III.  Finished:
  return(socketDescriptor);
}


int		main		(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Do server:
  int 	      port	= getPortNum(argc,argv);
  int	      listenFd	= getServerFileDescriptor(port);
  int	      status	= EXIT_FAILURE;

  if  (listenFd >= 0)
  {
    doServer(listenFd);
    close(listenFd);
    status	= EXIT_SUCCESS;
  }

  //  III.  Finished:
  return(status);
}
