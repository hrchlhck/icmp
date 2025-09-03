#ifndef UTILS_H

#define UTILS_H
#define IP_STR_SIZE 15

typedef struct args_t {
    char src_addr[IP_STR_SIZE];
    char dst_addr[IP_STR_SIZE];
} args_t;

args_t* create_args(const char* src_addr, const char* dst_addr);
unsigned int check_args(int argc, char const *argv[]);

#endif // UTILS_H