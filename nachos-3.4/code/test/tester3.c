#include "syscall.h"

int
main()
{	
	
	Exec("../test/size1Tes");
	Write("Hakuna Matata\n", 1 , 1);
	Yield();
	Exec("../test/tester2");

	Exit(0);

}
