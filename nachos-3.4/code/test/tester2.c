#include "syscall.h"

int
main()
{	
	Exec("../test/tester");
	Write("Hakuna Matata\n", 1 , 1);
	Exit(0);

}
