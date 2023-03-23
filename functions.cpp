/**
 * This file contains functions used for the solution.
 * Author: Jakob Götberg
 * Date: 22 Mars 2023
 */
#include "ProximityDecoder.h"

const std::regex pattern("^ID=(\\d+);X=(\\d+);Y=(\\d+);TYPE=(\\d)");
static pthread_mutex_t coordinate_array_lock = PTHREAD_MUTEX_INITIALIZER;

/**
 * Prints out error message to 'std::clog' and exits the program.
 */
void err_exit(const char *msg)
{
    std::clog << msg << std::endl;
    exit(EXIT_FAILURE);
}

const int OUTPUT_DELAY_MS = 1500;
void * printer_task(void *args)
{
    printer_struct *ps = (printer_struct*) args;
    std::chrono::milliseconds output_delay(OUTPUT_DELAY_MS);
    std::chrono::high_resolution_clock::time_point time_before;
    std::chrono::high_resolution_clock::time_point time_after;
    std::this_thread::sleep_for(output_delay);
    while (true)
    {
        time_before = std::chrono::high_resolution_clock::now();
        print_data(ps->count, ps->coordinate_array);
        time_after  = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(output_delay - (time_after - time_before));
    }
    return NULL;
}

/**
 * Calculates the color, given the objects members.
 */
inline void calc_color(coordinate_t *obj)
{
    uint64_t X = pow((TARGET_COORDINATE - obj->x), 2);
    uint64_t Y = pow((TARGET_COORDINATE - obj->y), 2);
    uint32_t distance = sqrt(X + Y);
    switch (obj->type) 
    {
        case 3: 
            obj->color = (distance < 100) ? COLOR_RED : COLOR_YELLOW;
            break;
        case 2:
            obj->color = (distance < 50) ? COLOR_YELLOW : COLOR_GREEN;
            break;
        case 1:
            if      (distance < 50) obj->color = COLOR_RED;
            else if (distance < 75) obj->color = COLOR_YELLOW;
            else                    obj->color = COLOR_GREEN;
            break;
    }
}

/**
 * Parses out the interesting data, converts it to integers and
 * stores it in a data structure.
 */
int parse_data(std::istringstream& buffer, int packages, coordinate_t *obj, uint32_t *count)
{
    std::string msg;
    std::smatch match;
    pthread_mutex_lock(&coordinate_array_lock);
    for (int i = 0; i < packages; i++)
    {
        std::getline(buffer, msg);
        if (std::regex_search(msg, match, pattern))
        {
            obj->id = std::stol(match[1]);
            obj->x = std::stoi(match[2]);
            obj->y = std::stoi(match[3]);
            obj->type = std::stoi(match[4]);
            calc_color(obj);
            obj++;
            *count += 1;
        }
        else
        {
            pthread_mutex_unlock(&coordinate_array_lock);
            return -1;
        }
    }
    pthread_mutex_unlock(&coordinate_array_lock);
    return 0;
}

/**
 * Prints out the collected data to STDOUT.
 * If the '-uf' argument is given when running the program,
 * the data will be displayed in raw binary. (The byte-order will
 * depend on your architecture).
 */
void print_data(uint32_t *count, coordinate_t *obj)
{
    pthread_mutex_lock(&coordinate_array_lock);
    if (unformatted_output)
    {
        uint32_t buff[] = {PREABMLE, *count};
        std::cout.write((char *) buff, 8);
        std::cout.write((char *) obj, (*count) * sizeof(coordinate_t));
    }
    else
    {
        printf("%08x%08x", PREABMLE, *count);
        for (int i = 0; i < *count; i++)
        {
            printf("%016llx%08x%08x%08x%x", obj->id, obj->x, obj->y, obj->type, obj->color);
            obj++;
        }
    }
    *count = 0;
    pthread_mutex_unlock(&coordinate_array_lock);
    fflush(stdout);
}
