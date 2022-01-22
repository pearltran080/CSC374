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

const char*	pointNames[]		= {"love","15","30","40","game",
                                           "game-over","game-over"};

int		currentNumPlayers	= 0;

int		shouldPlay		= 1;

pid_t		playerArray[MAX_NUM_PLAYERS];

int		points[MAX_NUM_PLAYERS];

void		sigUsr1Handler	(int		sigNum,
				 siginfo_t*	infoPtr,
				 void*		dataPtr
				)
{
  //  I.D.  YOUR CODE to increment the corresponding points[] elements
  if (playerArray[1] == infoPtr->si_pid)
    points[1] = points[1] + 1;
  if (playerArray[0] == infoPtr->si_pid)
    points[0] = points[0] + 1;

  if ((points[0] == NUM_POINTS_TO_WIN) || (points[1] == NUM_POINTS_TO_WIN)){
    shouldPlay = 0;
  }

  printf("Referee: \"%s %s (server %d, receiver %d)\"\n",
	 pointNames[points[1]],pointNames[points[0]],
	 points[1],points[0]
	);
}


int		main		()
{
  struct sigaction	act;
  int			i;

  //  I.C.1.  YOUR CODE to install sigUsr1Handler() as the SIGUSR1 handler
  memset(&act,'\0',sizeof(act));
  act.sa_sigaction = sigUsr1Handler;
  act.sa_flags = SA_SIGINFO;
  sigaction(SIGUSR1,&act,NULL);


  //  I.C.2.  YOUR CODE to start both child processes
  
  pid_t first = fork();
  if (first == 0) {
    execl(PLAYER_PROG, PLAYER_PROG, NULL);
    fprintf(stderr, "Cannot run %s\n", PLAYER_PROG);
    exit(EXIT_FAILURE);
   
  } else if (first > 0) {
    playerArray[0] = first;
    
    pid_t second = fork();
    if (second == 0 ) {
      char text[TEXT_LEN];
      snprintf(text,TEXT_LEN,"%d", playerArray[0]);
      execl(PLAYER_PROG, PLAYER_PROG, text, NULL);
      fprintf(stderr, "Cannot run %s\n", PLAYER_PROG);
      exit(EXIT_FAILURE);
    } else if (second > 0) {
      playerArray[1] = second;
    } else {
      exit(EXIT_FAILURE);
    }
  } else {
    exit(EXIT_FAILURE);
  }

  //  I.C.3.  Wait for the game to be over
  while  (shouldPlay)
    sleep(1);

  //  I.C.4.  YOUR CODE to tell children to stop:
  for (i = 0; i < MAX_NUM_PLAYERS; i++) {
    kill(playerArray[i], SIGTERM);
    wait(NULL);
  }
  
  //  III.  Finished:
  return(EXIT_SUCCESS);
}
