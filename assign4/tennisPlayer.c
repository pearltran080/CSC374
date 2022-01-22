#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<wait.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/types.h>

const char*	playerNameCPtr	= "";

pid_t		opponentPid	= -1;

int		isServer	= 0;

int		shouldPlay	= 1;

int		isMyTurn	= 0;

void		swing		()
{
  sleep(1);
  isMyTurn		= 0;

  if  ((rand() % 4) == 0)
  {
    fprintf(stderr, "%8s:\t\"Drat!\"\n",playerNameCPtr);
    kill(opponentPid,SIGUSR2);
    isMyTurn	= isServer;
  }
  else
  {
    fprintf(stderr, "%8s:\t\"(fwack)\"\n",playerNameCPtr);
    kill(opponentPid,SIGUSR1);
  }

}


void		sigTermHandler	(int		sigNum
				)
{
  shouldPlay	= 0;
}


void		sigUsr1Handler	(int		sigNum,
				 siginfo_t*	infoPtr,
				 void*		dataPtr
				)
{
  isMyTurn	= 1;
  opponentPid	= infoPtr->si_pid;
}


void		sigUsr2Handler	(int		sigNum,
				 siginfo_t*	infoPtr,
				 void*		dataPtr
				)
{
  isMyTurn	= isServer;
//opponentPid	= infoPtr->si_pid;
  kill(getppid(),SIGUSR1);
}


int		main		(int		argc,
				 char*		argv[]
				)
{
  srand(getpid());

  if  (argc > 1)
  {
    opponentPid		= strtol(argv[1],NULL,0);
    playerNameCPtr	= "server";
    isServer		= 1;
    isMyTurn		= 1;
  }
  else
    playerNameCPtr	= "receiver";

  struct sigaction	act;

  memset(&act,'\0',sizeof(act));
  act.sa_handler	= sigTermHandler;
  sigaction(SIGTERM,&act,NULL);

  memset(&act,'\0',sizeof(act));
  act.sa_sigaction	= sigUsr1Handler;
  act.sa_flags		= SA_SIGINFO;
  sigaction(SIGUSR1,&act,NULL);
  act.sa_sigaction	= sigUsr2Handler;
  sigaction(SIGUSR2,&act,NULL);


  while  (shouldPlay)
  {
    while  (!isMyTurn)
      sleep(1);

    swing();
  }

  return(EXIT_SUCCESS);
}
