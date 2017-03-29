#include "syscall.h"

int
main()
{	
	Exec("../test/size1Tes");
	Join(Exec("../test/size1Tes"));

	Exit(0);

}
