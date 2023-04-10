#include <math.h>
#include <string.h>
#include "image.h"
#include "test.h"
#include "args.h"

int main(int argc, char **argv)
{
    if(argc < 3){
        printf("usage: %s test <hw0 | hw1...>\n", argv[0]);  
    } else if (0 == strcmp(argv[1], "test")){
        if (0 == strcmp(argv[2], "hw1")) test_hw1();
        if (0 == strcmp(argv[2], "hw2")) test_hw2();
    }
    return 0;
}
