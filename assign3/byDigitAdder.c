/*-------------------------------------------------------------------------*
 *---									---*
 *---		byDigitAdder.c						---*
 *---									---*
 *---	    This file defines a C program that represents positive	---*
 *---	decimal integers as linked lists of digits.  It asks the user	---*
 *---	for 2 integers, computes the sum, and outputs it.		---*
 *---									---*
 *---	----	----	----	----	----	----	----	----	---*
 *---									---*
 *---	Version 1a					Joseph Phillips	---*
 *---									---*
 *-------------------------------------------------------------------------*/

#include	<stdlib.h>
#include	<stdio.h>
#include	<string.h>

#define		NUM_TEXT_LEN	256

//  PURPOSE:  To represent one digit of a decimal number.
struct			DigitNode
{
  int			digit_;		// I suggest you range this in [0..9]
  struct DigitNode*	nextPtr_;	// I suggest you make this point to
  	 				//  the next most significant digit.
};


//  PURPOSE:  To obtain the text of a decimal integer into array 'numberCPtr'
//	of length 'numberTextLen'.  'descriptionCPtr' is printed because it
//	tells the user the integer that is expected.   Ending '\n' from
//	'fgets()' is replaced with '\0'.  No return value.
void		obtainPostiveInt(char*		numberCPtr,
				 int		numberTextLen,
				 const char*	descriptionCPtr
				)
{
   // YOUR CODE HERE
  while (1) {
    int check = 1;
    printf(descriptionCPtr);
    fgets(numberCPtr, numberTextLen, stdin);

    int i = 0;

    for (i; (numberCPtr[i] != '\n' && numberCPtr[i] != '\0'); i++) {
      if (isdigit(numberCPtr[i]) == 0) {
        check = 0;
      }
    }
    if (check == 1) break;
  }
}


//  PURPOSE:  To build and return a linked list IN LITTLE ENDIAN ORDER
//	of the decimal number whose text is pointed to by 'numberCPtr'.
//	If 'numberCPtr' points to the string "123" then the linked list
//	returned is 'digit_=3' -> 'digit_=2' -> 'digit_=1' -> NULL.
struct	DigitNode*
		numberList	(const char*	numberCPtr
				)
{
  //  YOUR CODE HERE
  struct DigitNode* new = NULL;
  struct DigitNode* head = NULL;


  int i = 0;
  for (; (numberCPtr[i] != '\n' && numberCPtr[i] != '\0'); i++) {
    char c = numberCPtr[i];
    int num = c - '0';

    new = (struct DigitNode *)malloc(sizeof(struct DigitNode));
    new->digit_ = num;
    new->nextPtr_ = head;
    head = new;
  }
  return head;
}


//  PURPOSE:  To build and return a linked list IN LITTLE ENDIAN ORDER
//	of the decimal number that results from adding the decimal numbers
//	whose digits are pointed to by 'list0' and 'list1'.
struct	DigitNode*
		add		(const struct DigitNode*	list0,
				 const struct DigitNode*	list1
				)
{
  //  YOUR CODE HERE
  struct DigitNode* result = NULL;
  struct DigitNode* point = NULL;
  struct DigitNode* new = NULL;

  long carry = 0;

  while (list0 != NULL || list1 != NULL) {
    long add = carry;
    if (list0 != NULL) {
      add = add + list0->digit_;
      list0 = list0->nextPtr_;
    }
    if (list1 != NULL) {
      add = add + list1->digit_;
      list1 = list1->nextPtr_;
    }

    new = (struct DigitNode*)malloc(sizeof(struct DigitNode));
    if (add < 10) {
      new->digit_ = add;
      carry = 0;
    } else {
      new->digit_ = (add - 10);
      carry = 1;
    }

    if (point == NULL) {
      point = new;
      result = new;
    } else {
      point->nextPtr_ = new;
      point = point->nextPtr_;
    }
  }

  if (carry > 0) {
    new = (struct DigitNode*)malloc(sizeof(struct DigitNode));
    new->digit_ = carry;
    point->nextPtr_ =  new;
  }

  return result;
}


//  PURPOSE:  To print the decimal number whose digits are pointed to by 'list'.
//	Note that the digits are IN LITTLE ENDIAN ORDER.  No return value.
void		printList	(const struct DigitNode*	list
				)
{

  //  YOUR CODE HERE
  if (list != NULL) {
    printList(list->nextPtr_);
    printf("%d", list->digit_);
  }

}


//  PURPOSE:  To print the nodes of 'list'.  No return value.
void		freeList	(struct DigitNode*		list
				)
{
  //  YOUR CODE HERE
  if (list != NULL) {
    freeList(list->nextPtr_);
    free(list);
  }

}


//  PURPOSE:  To coordinate the running of the program.  Ignores command line
//	arguments.  Returns 'EXIT_SUCCESS' to OS.
int		main		()
{
  char			numberText0[NUM_TEXT_LEN];
  char			numberText1[NUM_TEXT_LEN];
  struct DigitNode*	operand0List	= NULL;
  struct DigitNode*	operand1List	= NULL;
  struct DigitNode*	sumList		= NULL;

  obtainPostiveInt(numberText0,NUM_TEXT_LEN,"Enter the first positive integer: ");
  obtainPostiveInt(numberText1,NUM_TEXT_LEN,"Enter the second positive integer: ");

  operand0List	= numberList(numberText0);
  operand1List	= numberList(numberText1);
  sumList	= add(operand0List,operand1List);

  printList(operand0List);
  printf(" + ");
  printList(operand1List);
  printf(" = ");
  printList(sumList);
  printf("\n");

  freeList(sumList);
  freeList(operand1List);
  freeList(operand0List);

  return(EXIT_SUCCESS);
}
