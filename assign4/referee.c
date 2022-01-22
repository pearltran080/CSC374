#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>
#include	<wait.h>
#include	<unistd.h>
#include	<signal.h>
#include	<sys/types.h>

const int	MAX_NUM_PLAYERS		= 2;

const int	TEXT_LEN		= 256;

const int	NUM_POINTS_TO_WIN	= 4;

#define		PLAYER_PROG		"tennisPlayer"

const char*	pointNames[]		= {"love","15","30","40","game","game-over","game-over"};

int		shouldPlay		= 1;

pid_t		playerArray[MAX_NUM_PLAYERS]
					= {-1,-1};

int		points[MAX_NUM_PLAYERS]	= {0,0};

void		sigUsr1Handler	(int		sigNum,
				 siginfo_t*	infoPtr,
				 void*		dataPtr
				)
{
  if  (infoPtr->si_pid == playerArray[0])
  {
    points[0]++;

    if  (points[0] == NUM_POINTS_TO_WIN)
      shouldPlay	= 0;
  }
  else
  {
    points[1]++;

    if  (points[1] == NUM_POINTS_TO_WIN)
      shouldPlay	= 0;
  }

  fprintf(stderr, "Referee: \"%s %s (server %d, receiver %d)\"\n",
	 pointNames[points[1]],pointNames[points[0]],
	 points[1],points[0]
	);
}


int		main		()
{
  struct sigaction	act;
  int			i;

  memset(&act,'\0',sizeof(act));
  act.sa_sigaction	= sigUsr1Handler;
  act.sa_flags		= SA_SIGINFO;
  sigaction(SIGUSR1,&act,NULL);

  for  (i = 0;  i < MAX_NUM_PLAYERS;  i++)
  {
    playerArray[i]	= fork();

    if  (playerArray[i] == 0)
    {

      if  (i == 0)
	execl(PLAYER_PROG,PLAYER_PROG,NULL);
      else
      {
        char	text[TEXT_LEN];

        snprintf(text,TEXT_LEN,"%d",playerArray[0]);
	execl(PLAYER_PROG,PLAYER_PROG,text,NULL);
      }

      fprintf(stderr,"Cannot run %s\n",PLAYER_PROG);
      exit(EXIT_FAILURE);
    }
  }

  while  (shouldPlay)
    sleep(1);

	printf("%d %d\n",points[1],points[0]);

  for  (i = 0;  i < MAX_NUM_PLAYERS;  i++)
  {
    kill(playerArray[i],SIGTERM);
    wait(NULL);
  }

  return(EXIT_SUCCESS);
}
