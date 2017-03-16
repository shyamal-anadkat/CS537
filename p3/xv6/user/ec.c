
/* syscall argument checks (null page) */
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int
main(int argc, char *argv[])
{
	char* s = "this is a stricccng constant";
	*s = 'x';

  printf(1, "TEST PASSED\n");
  exit();
}