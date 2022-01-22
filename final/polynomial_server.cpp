/*-------------------------------------------------------------------------*
 *---									---*
 *---		polynomial_server.cpp					---*
 *---									---*
 *---	    This file defines a C program that waits for a client to	---*
 *---	connect, gets a list of integers from the client and stores	---*
 *---	them in a stack, launches a polynomil-computing program, writes	---*
 *---	number to the pipe for the program, gets the computed		---*
 *---	polynomical values back, and sends them back to the client.	---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

/*---		Header file inclusion					---*/

#include	"PolynomialServerHeaders.h"
#include	<pthread.h>
#include	<signal.h>

const int	LO_LEGAL_PORT	= 1025;
const int	HI_LEGAL_PORT	= 65535;
const int	ERROR_FD	= -1;

extern void	computePolynomials
				(FloatStack*	outputStackPtr,
				 FloatStack*	inputStackPtr
				);

//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//	to the OS telling this server when a client process has connect()-ed
//	to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int		getServerFileDescriptor
				(int		port,
				 const char*	progName
				)
{
  //  I.  Application validity check:
  if  (progName == NULL)
  {
    fprintf(stderr,"BUG: NULL ptr to getServerFileDescriptor().\n");
    exit(EXIT_FAILURE);
  }

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET, // AF_INET domain
			        SOCK_STREAM, // Reliable TCP
			        0);

  if  (socketDescriptor < 0)
  {
    perror(progName);
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
    perror(progName);
    return(ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor,5);

  //  III.  Finished:
  return(socketDescriptor);
}


//  PURPOSE:  To ask the user which port to attempt to monopolize, and to return
//	entered port number.
int		getPort		()
{
  //  I.  Application validity check:

  //  II.  Get port number
  int	port;

  do
  {
    char	buffer[BUFFER_LEN];

    printf("Please enter port number to monopolize [%d-%d]: ",
	   LO_LEGAL_PORT,HI_LEGAL_PORT
	  );
    fgets(buffer,BUFFER_LEN,stdin);
    port = strtol(buffer,NULL,10);
  }
  while  ( (port < LO_LEGAL_PORT)  ||  (port > HI_LEGAL_PORT) );

  //  III.  Finished:
  return(port);
}


//  PURPOSE:  To do the work of handling the client.  Communication with the
//	client take place using file-descriptor obtained from '(int*)vPtr'.
//	Returns 'NULL'.
void*		handleClient	(void*	vPtr
				)
{
  int*	argArray	= (int*)vPtr;		// <-- CHANGE THAT NULL
  int	fd		= argArray[0];	// <-- CHANGE THAT 0
  int	threadNum	= argArray[1];		// <-- CHANGE THAT 0

  //  YOUR CODE HERE
	free(argArray);

  printf("Client thread %d:\n",threadNum);

  int	     	integer;
  float		x;
  FloatStack	inputStack;
  FloatStack	outputStack;

  do
  {
    //  YOUR CODE HERE
		read(fd, &integer, sizeof(integer));
		integer = ntohl(integer);
		x = *(float*)&integer;
		inputStack.push(x);
		printf("Received %g\n",x);
  }
  while  (x != 0);

  computePolynomials(&outputStack,&inputStack);

	float popped;
	int toSend;

  while  ( !outputStack.isEmpty() )
  {
    //  YOUR CODE HERE
		popped = outputStack.pop();
		toSend = htonl(*(int*)&popped);
		write(fd, &toSend, sizeof(toSend));
  }

  //  YOUR CODE HERE
	close(fd);

  //  III.  Finished:
  return(NULL);
}


//  PURPOSE:  To serve the clients using file-descriptor 'listenFd' to tell
//	when a client has connected.  Each client is handled by its own child
//	process.  Both the parent and the child processes close unnecesary
//	file-descriptorors.  No return value.
void	       	doServer  	(int  	 	listenFd
				)
{
  //  I.  Application validity check:
  if  (listenFd < 0)
  {
    fprintf(stderr,"Illegal file-descriptor to doServer()\n");
    exit(EXIT_FAILURE);
  }

  //  II.  Do the work of the server:
  int			i;
  pthread_t		thread;
  pthread_attr_t	attr;

  //  YOUR CODE HERE
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  for  (i = 0;  ;  i++)
  {
    // Accept connection to client
    int*	argArray	= (int*)calloc(2, sizeof(int));		// <-- CHANGE THAT NULL

    //  YOUR CODE HERE
		argArray[0] = accept(listenFd, NULL, NULL);
		argArray[1] = i;

		pthread_create(&thread, &attr, handleClient, (void*)argArray);
  }

  //  YOUR CODE HERE
	pthread_attr_destroy(&attr);

  //  III.  Finished:
}


//  PURPOSE:  To oversee the main work of the server.  Ignores 'argc' but
//	uses 'argv[0]' as the name of the program.  Returns 'EXIT_SUCCESS' to
//	OS on success or 'EXIT_FAILURE' otherwise.
int		main		(int		argc,
				 char*		argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Do server:
  struct sigaction	act;
  int			port		= getPort();
  int			socketFd	= getServerFileDescriptor(port,argv[0]);

  memset(&act,'\0',sizeof(act));
  act.sa_handler	= SIG_IGN;
  sigaction(SIGPIPE,&act,NULL);

  doServer(socketFd);
  close(socketFd);

  //  III.  Finished:
  return(EXIT_SUCCESS);
}
