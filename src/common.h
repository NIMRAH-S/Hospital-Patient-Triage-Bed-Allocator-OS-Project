#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <semaphore.h>

#define SHM_KEY 0xBEDF00D
#define MAX_PATIENTS 100
#define TOTAL_BEDS 20
#define FIFO_PATH "/tmp/discharge_fifo"
#define TRIAGE_FIFO "/tmp/triage_fifo"

#define ICU_BEDS 4
#define ISOLATION_BEDS 4
#define GENERAL_BEDS 12

#define PAGE_SIZE 2

typedef struct {
    int patient_id;
    char name[64];
    int age;
    int severity;
    int priority;
    int care_units;
    time_t arrival_time;
} PatientRecord;

typedef struct {
    int partition_id;
    int start_unit;
    int size;
    int is_free;
    int patient_id;
    char bed_type[16];
} BedPartition;

typedef struct {
    BedPartition beds[TOTAL_BEDS];
} SharedWard;

#endif
