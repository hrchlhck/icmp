#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

args_t* create_args(const char* src_addr, const char* dst_addr) {
    args_t* ret = (args_t*) malloc(sizeof(args_t));

    strcpy(ret->src_addr, src_addr);
    strcpy(ret->dst_addr, dst_addr);

    return ret;
}

unsigned int check_args(int argc, char const *argv[]) {
    switch(argc) {
        case 1:
            printf("Missing SRC_ADDR and DST_ADDR\n");
            return 1;
        
        case 2:
            printf("Missing DST_ADDR\n");
            return 2;
        
        case 3:
            return 0;
    }
    return -1;
}