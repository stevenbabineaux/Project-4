#include "syscall.h"

int
main()
{	
	Exec("../test/size1Tes");
	Write("Hakuna Matata\n", 1 , 1);
	
	Yield();

	Exit(0);

}
