#include "common.h"

void simulate_fcfs()
{
    FILE *fp = fopen("logs/schedule_log.txt", "a");

    fprintf(fp, "\n===== FCFS Scheduling =====\n");

    int bt[5] = {5, 3, 8, 6, 2};

    int wt[5];
    int tat[5];

    wt[0] = 0;

    for (int i = 1; i < 5; i++)
        wt[i] = wt[i - 1] + bt[i - 1];

    for (int i = 0; i < 5; i++)
        tat[i] = wt[i] + bt[i];

    float avg_wt = 0;
    float avg_tat = 0;

    fprintf(fp, "PID\tBT\tWT\tTAT\n");

    for (int i = 0; i < 5; i++)
    {
        fprintf(fp,
                "%d\t%d\t%d\t%d\n",
                i + 1,
                bt[i],
                wt[i],
                tat[i]);

        avg_wt += wt[i];
        avg_tat += tat[i];
    }

    fprintf(fp,
            "Average WT = %.2f\n",
            avg_wt / 5);

    fprintf(fp,
            "Average TAT = %.2f\n",
            avg_tat / 5);

    fclose(fp);
}

void simulate_priority()
{
    FILE *fp = fopen("logs/schedule_log.txt", "a");

    fprintf(fp, "\n===== Priority Scheduling =====\n");

    fprintf(fp,
            "Critical patients executed first based on triage priority.\n");

    fclose(fp);
}



// #include "common.h"

// void simulate_fcfs() {
//     FILE *fp = fopen("logs/schedule_log.txt", "a");
//     if (!fp) return;

//     fprintf(fp, "\n===== FCFS Scheduling Report =====\n");
//     // Implementation logic for FCFS visualization
//     fclose(fp);
// }

// void simulate_priority() {
//     FILE *fp = fopen("logs/schedule_log.txt", "a");
//     if (!fp) return;

//     fprintf(fp, "\n===== Priority Scheduling Report =====\n");
//     fprintf(fp, "Critical patients executed first based on triage priority.\n");
//     fclose(fp);
// }