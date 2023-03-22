/**
 * This file contains my solution to the 'SW Engineering' assignment.
 * Author: Jakob Götberg
 * Date: 22 Mars 2023
 */
#include "ProximityDecoder.h"

bool unformatted_output = false;
std::string unformatted_flag = "-uf";
const uint8_t APPROX_PACKAGE_SIZE = 37;
int socket_fd;
coordinate_t *coordinate_array;
printer_struct *ps;

static void clean_up(int signal)
{
    close(socket_fd);
    free(coordinate_array);
    free(ps);
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) 
{
    if (argc > 1 && (argv[1] == unformatted_flag))
        unformatted_output = true;

    uint32_t count = 0;
    char buffer[BUFF_SIZE];
    uint32_t read_bytes = 0;
    uint32_t packages = 0;
    struct sockaddr_in server;

    coordinate_array = (coordinate_t *) calloc(sizeof(coordinate_t), ARRAY_SIZE);
    ps = (printer_struct *) calloc(sizeof(printer_struct), 1);
    ps->count = &count;
    ps->coordinate_array = coordinate_array;

    std::chrono::high_resolution_clock::time_point timer;
    std::chrono::duration<double> diff;

    pthread_t printer;
    pthread_attr_t printer_attr;
    pthread_attr_init(&printer_attr);
    pthread_attr_setdetachstate(&printer_attr, PTHREAD_CREATE_DETACHED);

    std::signal(SIGINT, clean_up);

    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
        err_exit("Socket initiation failed");
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT); 
    server.sin_addr.s_addr = INADDR_ANY;
    if (connect(socket_fd, (struct sockaddr*)&server, sizeof(server)) == -1) 
        err_exit("Connecting to server failed");

    timer = std::chrono::high_resolution_clock::now();
    while (true) 
    {
        std::chrono::high_resolution_clock::time_point time_stamp 
            = std::chrono::high_resolution_clock::now(); 
        diff = std::chrono::duration_cast<std::chrono::milliseconds>(time_stamp - timer);
        if (diff.count() >= OUTPUT_DELAY_MS)
        {
            pthread_create(&printer, &printer_attr, printer_task, (void *) ps);
            timer = std::chrono::high_resolution_clock::now();
        }
        
        read_bytes = recv(socket_fd, buffer, sizeof(buffer), 0);
        if (read_bytes == -1) err_exit("Communication with server failed");

        packages = read_bytes / APPROX_PACKAGE_SIZE;
        std::istringstream parse_buffer(buffer);
        if (parse_data(parse_buffer, packages, &coordinate_array[count], &count) == -1) 
            err_exit("Parser failed, input data was corrupted");
    }
}


