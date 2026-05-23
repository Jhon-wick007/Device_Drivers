#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main()
{
    int input_fd, driver_fd, output_fd;
    char buff[4096];

    input_fd = open("input.txt",O_RDONLY);
    if(input_fd<0)
    {
        printf("Could not open input file\n");
        return -1;
    }

    driver_fd = open("/dev/read_write_node",O_RDWR);
    if(driver_fd<0)
    {
        printf("could not open driver file\n");
        return -1;
    }

    output_fd = open("output.txt",O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if(output_fd<0)
    {
        printf("Could not open output file\n");
        return -1;
    }

    ssize_t bytes;

    while((bytes = read(input_fd, buff,4096))>0)
    {
        printf("Writing in driver buffer\n");
        write(driver_fd, buff,bytes);
    }

    close(input_fd);

    lseek(driver_fd,0,SEEK_SET);

    while((bytes = read(driver_fd, buff,4096))>0)
    {
        printf("Writing in output file\n");
        write(output_fd, buff, bytes);
    }

    close(driver_fd);
    close(output_fd);

    return 0;
}
