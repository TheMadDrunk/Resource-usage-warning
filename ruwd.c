#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// Define the CPU usage threshold (percentage) at which notifications will be sent.
int CPU_THRESHOLD = 80;

// Define the memory (RAM) usage threshold (percentage) at which notifications will be sent.
int MEM_THRESHOLD = 85;

// Initializes the daemon process by forking, creating a new session,
// changing the working directory to the root directory, and closing
// standard file descriptors.
void init_start();

// The main daemon loop that continuously monitors CPU and memory usage
// and sends notifications when thresholds are exceeded.
void _daemon();

// Checks the system's memory usage by reading information from
// "/proc/meminfo" and returns 1 if memory usage is above a certain
// threshold, otherwise returns 0.
int check_memory();

// Checks the system's CPU usage by reading statistics from "/proc/stat"
// and calculates the CPU usage percentage. Returns 1 if CPU usage is
// above a certain threshold for consecutive checks, otherwise returns 0.
int check_cpu();

// Sends notifications based on CPU and memory usage. Uses the "notify-send"
// command to display notifications to the user. Limits the frequency of
// notifications to prevent excessive alerts.
void send_notif(int cpu, int mem);


long int last_cpustat[7];
long int last_sum;
double last_cpu_usage;
long int last_notif;

int main()
{
    init_start();
    _daemon();
    return 0;
}

void send_notif(int cpu,int mem){
    int current_time = time(NULL);

    if(current_time-last_notif < 180)
        return;

    if(cpu || mem)
        last_notif = current_time;
    
    char text[50];
    
    if(cpu && mem){
        system("notify-send \"Heavy CPU and memory usage !!\"");
        return;
    }
    

    if(cpu){
        sprintf(text,"notify-send \"CPU usage over %d%!!\"",CPU_THRESHOLD);
        system(text);

    }
    
    if(mem){
        sprintf(text,"notify-send \"RAM usage over %d%!!\"",MEM_THRESHOLD);
        system(text);
    }
}

int check_cpu()
{
    FILE *stat = fopen("/proc/stat", "r");
    if (stat == NULL)
    {
        exit(EXIT_FAILURE);
    }

    long int cpustat[7];
    fscanf(stat, "cpu %ld %ld %ld %ld %ld %ld %ld",
           &cpustat[0], &cpustat[1], &cpustat[2], &cpustat[3], &cpustat[4], &cpustat[5], &cpustat[6]);

    long int sum = 0;
    for (int i = 0; i < 7; i++)
    {
        sum += cpustat[i];
    }

    double cpu_idle = cpustat[3] - last_cpustat[3];
    double cpu_delta = sum - last_sum;
    double cpu_used = cpu_delta - cpu_idle;
    double cpu_usage = 100 * (cpu_used / cpu_delta);
    for (int i = 0; i < 7; i++)
    {
        last_cpustat[i] = cpustat[i];
    }
    last_sum = sum;
    last_cpu_usage = cpu_usage;
    fclose(stat);

    if(cpu_usage > CPU_THRESHOLD && last_cpu_usage > CPU_THRESHOLD){
        return 1;
    }

    return 0;
}

int check_memory()
{
    FILE *meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL)
    {
        exit(EXIT_FAILURE);
    }
    long int memdata[3];
    fscanf(meminfo, "MemTotal:        %ld kB\nMemFree:         %ld kB\nMemAvailable:    %ld kB",
           &memdata[0], &memdata[1], &memdata[2]);
    

    fclose(meminfo);

    if(100*memdata[2]/(double)memdata[0] <= 100-MEM_THRESHOLD){
        return 1;
    }

    return 0;
}

void _daemon()
{

    while (1)
    {
        int mem_state = check_memory();
        int cpu_state = check_cpu();

        send_notif(cpu_state,mem_state);

        sleep(30);
    }
}

void init_start()
{
    int pid = fork();
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    int sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (chdir("/") < 0)
    {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}