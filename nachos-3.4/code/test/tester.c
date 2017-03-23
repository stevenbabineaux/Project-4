#include "syscall.h"

int
main()
{

	Write("Hey This Works duude\n", 0, 1);
	Exec("../test/sizeTest1");
	Exit(0);

}
