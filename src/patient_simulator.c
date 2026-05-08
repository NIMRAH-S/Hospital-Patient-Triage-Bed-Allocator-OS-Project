#include "common.h"

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        printf("Usage: patient_simulator <id> <priority> <bedid> <type>\n");
        return 1;
    }

    int patient_id = atoi(argv[1]);
    int priority = atoi(argv[2]);
    (void)priority;
    int bed_id = atoi(argv[3]);
    char bed_type[32];

    strcpy(bed_type, argv[4]);

    printf("Patient %d admitted to %s Bed %d\n",
           patient_id,
           bed_type,
           bed_id);

    srand(time(NULL) ^ getpid());

    int sleep_time = 0;

    if (strcmp(bed_type, "ICU") == 0)
        sleep_time = rand() % 11 + 5;
    else if (strcmp(bed_type, "ISOLATION") == 0)
        sleep_time = rand() % 8 + 3;
    else
        sleep_time = rand() % 7 + 2;

    printf("Patient %d treatment started for %d seconds\n",
           patient_id,
           sleep_time);

    sleep(sleep_time);

    int fd = open(FIFO_PATH, O_WRONLY);

    if (fd >= 0)
    {
        write(fd, &patient_id, sizeof(int));
        close(fd);
    }

    printf("Patient %d discharged from Bed %d\n",
           patient_id,
           bed_id);

    return 0;
}
