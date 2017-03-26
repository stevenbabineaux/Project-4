#include "syscall.h"

int
main()
{	
	Exec("../test/size1Tes");
	Write("Hakuna Matata\n", 1 , 1);
	
	Yield();
	
	/*
	Exec("../test/sizeTest1");

	Yield();
	
	Exec("../test/sizeTest1");

	Yield();

	Yield();
	Exec("../test/sizeTest1"); 

	Yield();
	Exec("../test/sizeTest1");

	Yield();
	Exec("../test/sizeTest1");

	Yield();
	Exec("../test/sizeTest1");

	Yield();
	Exec("../test/sizeTest1");
	
	*/
	Exit(0);

}
