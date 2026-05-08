#include "common.h"

void simulate_fcfs();
void simulate_priority();

SharedWard *ward;
int shm_id;

pthread_mutex_t bed_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bed_freed = PTHREAD_COND_INITIALIZER;

sem_t icu_sem;
sem_t isolation_sem;
sem_t queue_full;
sem_t queue_empty;

PatientRecord queue[MAX_PATIENTS];
int front = 0;
int rear = 0;

int patient_counter = 1;

void enqueue(PatientRecord p)
{
    sem_wait(&queue_empty);

    queue[rear] = p;
    rear = (rear + 1) % MAX_PATIENTS;

    sem_post(&queue_full);
}

PatientRecord dequeue()
{
    sem_wait(&queue_full);

    PatientRecord p = queue[front];
    front = (front + 1) % MAX_PATIENTS;

    sem_post(&queue_empty);

    return p;
}

void initialize_beds()
{
    int index = 0;

    for (int i = 0; i < ICU_BEDS; i++)
    {
        ward->beds[index].partition_id = index;
        ward->beds[index].size = 3;
        ward->beds[index].is_free = 1;
        strcpy(ward->beds[index].bed_type, "ICU");
        index++;
    }

    for (int i = 0; i < ISOLATION_BEDS; i++)
    {
        ward->beds[index].partition_id = index;
        ward->beds[index].size = 2;
        ward->beds[index].is_free = 1;
        strcpy(ward->beds[index].bed_type, "ISOLATION");
        index++;
    }

    for (int i = 0; i < GENERAL_BEDS; i++)
    {
        ward->beds[index].partition_id = index;
        ward->beds[index].size = 1;
        ward->beds[index].is_free = 1;
        strcpy(ward->beds[index].bed_type, "GENERAL");
        index++;
    }
}

int best_fit_allocate(int care_units, char *type)
{
    int best = -1;
    int min_diff = 9999;

    for (int i = 0; i < TOTAL_BEDS; i++)
    {
        if (ward->beds[i].is_free &&
            strcmp(ward->beds[i].bed_type, type) == 0)
        {
            int diff = ward->beds[i].size - care_units;

            if (diff >= 0 && diff < min_diff)
            {
                min_diff = diff;
                best = i;
            }
        }
    }

    return best;
}

void free_bed(int patient_id)
{
    pthread_mutex_lock(&bed_mutex);

    for (int i = 0; i < TOTAL_BEDS; i++)
    {
        if (ward->beds[i].patient_id == patient_id)
        {
            ward->beds[i].is_free = 1;
            ward->beds[i].patient_id = -1;

            printf("Bed %d freed for Patient %d\n",
                   i,
                   patient_id);

            if (strcmp(ward->beds[i].bed_type, "ICU") == 0)
                sem_post(&icu_sem);

            if (strcmp(ward->beds[i].bed_type, "ISOLATION") == 0)
                sem_post(&isolation_sem);
        }
    }

    pthread_cond_broadcast(&bed_freed);
    pthread_mutex_unlock(&bed_mutex);
}

void *receptionist_thread(void *arg)
{
    mkfifo(TRIAGE_FIFO, 0666);

    int fd = open(TRIAGE_FIFO, O_RDONLY);

    while (1)
    {
        PatientRecord p;

        int n = read(fd, &p, sizeof(PatientRecord));

        if (n > 0)
        {
            printf("Receptionist received Patient %d\n",
                   p.patient_id);

            enqueue(p);
        }
    }

    return NULL;
}

void *scheduler_thread(void *arg)
{
    while (1)
    {
        PatientRecord p = dequeue();

        pthread_mutex_lock(&bed_mutex);

        int bed = -1;
        char type[32];

        if (p.priority <= 2)
        {
            sem_wait(&icu_sem);
            strcpy(type, "ICU");
            bed = best_fit_allocate(3, type);
        }
        else if (p.priority == 3)
        {
            sem_wait(&isolation_sem);
            strcpy(type, "ISOLATION");
            bed = best_fit_allocate(2, type);
        }
        else
        {
            strcpy(type, "GENERAL");
            bed = best_fit_allocate(1, type);
        }

        while (bed == -1)
        {
            printf("No bed available. Waiting...\n");
            pthread_cond_wait(&bed_freed, &bed_mutex);
            bed = best_fit_allocate(p.care_units, type);
        }

        ward->beds[bed].is_free = 0;
        ward->beds[bed].patient_id = p.patient_id;

        printf("Patient %d allocated Bed %d (%s)\n",
               p.patient_id,
               bed,
               type);

        pthread_mutex_unlock(&bed_mutex);

        pid_t pid = fork();

        if (pid == 0)
        {
            char id[10], pr[10], bd[10];

            sprintf(id, "%d", p.patient_id);
            sprintf(pr, "%d", p.priority);
            sprintf(bd, "%d", bed);

            char *args[] = {
                "./patient_simulator",
                id,
                pr,
                bd,
                type,
                NULL};

            execv("./patient_simulator", args);
            exit(0);
        }
    }

    return NULL;
}

void *nurse_thread(void *arg)
{
    mkfifo(FIFO_PATH, 0666);

    int fd = open(FIFO_PATH, O_RDONLY);

    while (1)
    {
        int patient_id;

        int n = read(fd, &patient_id, sizeof(int));

        if (n > 0)
            free_bed(patient_id);
    }

    return NULL;
}

void sigchld_handler(int sig)
{
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
}

void print_fragmentation()
{
    int free_units = 0;
    int largest = 0;

    for (int i = 0; i < TOTAL_BEDS; i++)
    {
        if (ward->beds[i].is_free)
        {
            free_units += ward->beds[i].size;

            if (ward->beds[i].size > largest)
                largest = ward->beds[i].size;
        }
    }

    double fragmentation = 0;

    if (free_units > 0)
    {
        fragmentation =
            (1.0 - ((double)largest / free_units)) * 100;
    }

    FILE *fp = fopen("logs/memory_log.txt", "a");

    fprintf(fp,
            "Free Units: %d Largest Block: %d Fragmentation: %.2f%%\n",
            free_units,
            largest,
            fragmentation);

    fclose(fp);
}

int main()
{
    signal(SIGCHLD, sigchld_handler);

    shm_id = shmget(SHM_KEY,
                    sizeof(SharedWard),
                    IPC_CREAT | 0666);

    ward = (SharedWard *)shmat(shm_id, NULL, 0);

    initialize_beds();

    sem_init(&icu_sem, 0, ICU_BEDS);
    sem_init(&isolation_sem, 0, ISOLATION_BEDS);

    sem_init(&queue_full, 0, 0);
    sem_init(&queue_empty, 0, MAX_PATIENTS);

    pthread_t receptionist;
    pthread_t scheduler;
    pthread_t nurse1;
    pthread_t nurse2;
    pthread_t nurse3;

    pthread_create(&receptionist,
                   NULL,
                   receptionist_thread,
                   NULL);

    pthread_create(&scheduler,
                   NULL,
                   scheduler_thread,
                   NULL);

    pthread_create(&nurse1,
                   NULL,
                   nurse_thread,
                   NULL);

    pthread_create(&nurse2,
                   NULL,
                   nurse_thread,
                   NULL);

    pthread_create(&nurse3,
                   NULL,
                   nurse_thread,
                   NULL);

    printf("Hospital Admissions Manager Running...\n");
    simulate_fcfs();
    simulate_priority();

    while (1)
    {
        print_fragmentation();
        sleep(10);
    }

    shmdt(ward);
    shmctl(shm_id, IPC_RMID, NULL);

    return 0;
}




// #include "common.h"

// // Prototypes for functions in scheduler.c
// void simulate_fcfs();
// void simulate_priority();

// SharedWard *ward;
// int shm_id;

// pthread_mutex_t bed_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_cond_t bed_freed = PTHREAD_COND_INITIALIZER;

// sem_t icu_sem, isolation_sem, queue_full, queue_empty;
// PatientRecord queue[MAX_PATIENTS];
// int front = 0, rear = 0;

// void sigchld_handler(int sig) {
//     while (waitpid(-1, NULL, WNOHANG) > 0);
// }

// // Best-Fit Memory Allocation Logic
// int best_fit_allocate(int units, char* type) {
//     int best_idx = -1;
//     int min_waste = 999;

//     pthread_mutex_lock(&bed_mutex);
//     for (int i = 0; i < TOTAL_BEDS; i++) {
//         if (ward->beds[i].is_free && strcmp(ward->beds[i].bed_type, type) == 0) {
//             int waste = ward->beds[i].size - units;
//             if (waste >= 0 && waste < min_waste) {
//                 min_waste = waste;
//                 best_idx = i;
//             }
//         }
//     }
    
//     if (best_idx != -1) {
//         ward->beds[best_idx].is_free = 0;
//     }
//     pthread_mutex_unlock(&bed_mutex);
//     return best_idx;
// }

// void* receptionist_thread(void* arg) {
//     mkfifo(TRIAGE_FIFO, 0666);
//     int fd = open(TRIAGE_FIFO, O_RDONLY);
//     PatientRecord p;
    
//     while (read(fd, &p, sizeof(PatientRecord)) > 0) {
//         printf("[Reception] Received Patient: %s (Priority: %d)\n", p.name, p.priority);
//         sem_wait(&queue_empty);
//         queue[rear] = p;
//         rear = (rear + 1) % MAX_PATIENTS;
//         sem_post(&queue_full);
//     }
//     close(fd);
//     return NULL;
// }

// void* scheduler_thread(void* arg) {
//     while (1) {
//         sem_wait(&queue_full);
//         PatientRecord p = queue[front];
//         front = (front + 1) % MAX_PATIENTS;
//         sem_post(&queue_empty);

//         char* type;
//         if (p.priority == 1) type = "ICU";
//         else if (p.priority == 2) type = "ISOLATION";
//         else type = "GENERAL";
        
//         int bed_idx = -1;
//         while ((bed_idx = best_fit_allocate(p.care_units, type)) == -1) {
//             printf("[Scheduler] No %s bed for %s. Waiting...\n", type, p.name);
//             sleep(2);
//         }

//         pid_t pid = fork();
//         if (pid == 0) {
//             char pid_str[10], prio_str[10], bed_str[10];
//             sprintf(pid_str, "%d", p.patient_id);
//             sprintf(prio_str, "%d", p.priority);
//             sprintf(bed_str, "%d", bed_idx);
//             execl("./patient_simulator", "patient_simulator", pid_str, prio_str, bed_str, type, (char *)NULL);
//             exit(1); 
//         }
//     }
//     return NULL;
// }

// int main() {
//     signal(SIGCHLD, sigchld_handler);
    
//     shm_id = shmget(SHM_KEY, sizeof(SharedWard), IPC_CREAT | 0666);
//     ward = (SharedWard *)shmat(shm_id, NULL, 0);
    
//     // Initialize beds - FIXED TYPO HERE
//     for(int i = 0; i < TOTAL_BEDS; i++) {
//         ward->beds[i].partition_id = i;
//         ward->beds[i].is_free = 1;
//         ward->beds[i].size = (i % 2 == 0) ? 5 : 3; // Variable sizes for Best-Fit
        
//         if (i < ICU_BEDS) strcpy(ward->beds[i].bed_type, "ICU");
//         else if (i < (ICU_BEDS + ISOLATION_BEDS)) strcpy(ward->beds[i].bed_type, "ISOLATION");
//         else strcpy(ward->beds[i].bed_type, "GENERAL");
//     }

//     sem_init(&queue_full, 0, 0);
//     sem_init(&queue_empty, 0, MAX_PATIENTS);

//     pthread_t rec, sched;
//     pthread_create(&rec, NULL, receptionist_thread, NULL);
//     pthread_create(&sched, NULL, scheduler_thread, NULL);

//     printf("Hospital Admissions Manager Running...\n");
    
//     pthread_join(rec, NULL); // Keep main alive
//     return 0;
// }