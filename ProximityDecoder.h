#include <cstdlib>
#include <iostream>
#include <regex>
#include <string>
#include <chrono>
#include <sstream>
#include <cstring>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>
#include <csignal>

#define PORT                   5463
#define BUFF_SIZE              1024
#define ARRAY_SIZE             256
#define OUTPUT_DELAY_MS        1.5
#define TARGET_COORDINATE      150

const uint32_t COLOR_RED =     0x1b5b316d;
const uint32_t COLOR_YELLOW =  0x1b5b336d;
const uint32_t COLOR_GREEN =   0x1b5b326d;
const uint32_t PREABMLE =      0xFEFF;
extern bool unformatted_output;

typedef struct coordinate_t
{
    uint64_t id;
    int32_t x;
    int32_t y;
    uint32_t type;
    uint32_t color;
}coordinate_t;

typedef struct printer_struct
{
    uint32_t *count;
    coordinate_t *coordinate_array;
}printer_struct;

int parse_data(std::istringstream& , int, coordinate_t *, uint32_t *);
void err_exit(const char *);
inline void calc_color(coordinate_t *);
void print_data(uint32_t *, coordinate_t *);
void * printer_task(void *args);
