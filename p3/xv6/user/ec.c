/* heap must not grow into the page below stack */
#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{

  char* s = "this is a string constant";
  *s = 'x';
  printf(1, "TEST FAILED\n");
  exit();
}