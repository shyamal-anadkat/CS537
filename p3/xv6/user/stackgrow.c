#include "types.h"
#include "stat.h"
#include "user.h"

#define PAGE (2000)
#define MAX_PROC_MEM (640 * 1024)

int stdout = 1;
void recursive_stack_grow(int num);

int
main(int argc, char *argv[])
{
	//int num = 0;
	printf(stdout, "Starting overcommit-stack.c TEST:\n");
	// Grow stack past the middle -> expect error //
	
	int large_array[PAGE];
	uint i;
    for(i = 0; i < (PAGE); i++) {
		large_array[i] = i;
    }
    large_array[0] = large_array[1];

	//recursive_stack_grow(num);

	printf(stdout, "overcommit-stack.c test DONE\n");
	exit();
}

void
recursive_stack_grow(int num)
{
	uint i;
	int large_array[PAGE];
	for(i = 0; i < (PAGE); i++)
		large_array[i] = i;
	uint a = large_array[0];
	a = a + 1;
	// stop infinite loop //
	if(num > 2000)
		return;
	recursive_stack_grow(num++);
}