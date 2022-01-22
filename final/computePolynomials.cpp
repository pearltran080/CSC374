/*-------------------------------------------------------------------------*
 *---									---*
 *---		computePolynomial.cpp					---*
 *---									---*
 *---	    This file defines a function that creates a child process,	---*
 *---	sends the contents of '*inputStackPtr' to the child process  	---*
 *---	via a pipe, reads the results that the child gives back using	---*
 *---	another pipe, and put those results into '*outputStackPtr'.	---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1.0		2021 May 27		Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	"PolynomialServerHeaders.h"
#include	<wait.h>

void		computePolynomials
				(FloatStack*	outputStackPtr,
				 FloatStack*	inputStackPtr
				)
{
  int		toChild[2];
  int		fromChild[2];
  pid_t		polynomialPid;
  char		buffer[BUFFER_LEN];
  float		fOfX;
  float		x;

  //  YOUR CODE HERE
	if ((pipe(toChild) < 0) || (pipe(fromChild) < 0)) {
		fprintf(stderr, "Can't make pipes\n");
		exit (EXIT_FAILURE);
	}

	polynomialPid = fork();

  if  (polynomialPid == 0)
  {
    //  YOUR CODE HERE
		close(toChild[1]);
		close(fromChild[0]);

		dup2(toChild[0], STDIN_FILENO);
		dup2(fromChild[1], STDOUT_FILENO);

		execl(PROGRAM_NAME, PROGRAM_NAME, NULL);
		exit(EXIT_FAILURE);
  }

  //  YOUR CODE HERE
	close(toChild[0]);
	close(fromChild[1]);

  while  ( !inputStackPtr->isEmpty() )
  {
    //  YOUR CODE HERE
		snprintf(buffer, BUFFER_LEN, "%g\n", inputStackPtr->pop());
		write(toChild[1], &buffer, BUFFER_LEN);
  }

  //  YOUR CODE HERE
	write(toChild[1], END_OF_INPUT_MARKER, sizeof(END_OF_INPUT_MARKER));

  FILE*	inputPtr	= fdopen(fromChild[0],"r");

  while  ( fgets(buffer,BUFFER_LEN,inputPtr) != NULL )
  {
    //  YOUR CODE HERE
		sscanf(buffer, "%g %g", x, fOfX);
		outputStackPtr->push(fOfX);
		outputStackPtr->push(x);
  }
  //  YOUR CODE HERE
	fclose(inputPtr);
	close(toChild[1]);
	close(fromChild[0]);
	wait(NULL);
}
