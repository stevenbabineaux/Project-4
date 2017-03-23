/* sizeTest.c
 *	Simple program to test whether running a user program works.
 *	
 *	Just do a "syscall" that shuts down the OS.
 *
 * 	NOTE: for some reason, user programs with global data structures 
 *	sometimes haven't worked in the Nachos environment.  So be careful
 *	out there!  One option is to allocate data structures as 
 * 	automatics within a procedure, but if you do this, you have to
 *	be careful to allocate a big enough stack to hold the automatics!
 */

#include "syscall.h"

int i;
int a;
int arr[1500];

int
main()
{

	Exec("../test/sizeTest1");

	/*
	for(i = 0; i < 1500; i++){
		//a *= a;
		arr[i] = a;
	}*/
	
	
}
