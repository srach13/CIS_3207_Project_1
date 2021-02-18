#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "config_reader.c"

#define SEED 2
#define INIT_TIME 0
#define FIN_TIME 10000
#define ARRIVE_MIN 50
#define ARRIVE_MAX 60
#define QUIT_PROB 0.2
#define NET_PROB 0.2
#define CPU_MIN 50
#define CPU_MAX 60
#define DISK1_MIN 20
#define DISK1_MAX 60
#define DISK2_MIN 60
#define DISK2_MAX 100
#define NETWORK_MIN 20
#define NETWORK_MAX 40


// EVENT STRUCTURES

// LINKED LIST FOR QUEUE
struct Event {
    int eventNum;
    int time;
    int eventType;
    struct Event* nextPQ;
    struct Event* nextFIFO;
};

// FIFO QUEUE
struct FIFOQueue {
    int totalJobs;
    int size;
    int occupied;
    struct Event *front, *rear;
};

// CREATE A NEW EVENT
struct Event* newEvent(int number) {
    struct Event* temp = (struct Event*)malloc(sizeof(struct Event));
    temp->eventNum = number;
    temp->nextPQ = NULL;
    return temp;
}

// CREATE AN EMPTY FIFO QUEUE
struct FIFOQueue* createQueue() {
    struct FIFOQueue* q = (struct FIFOQueue*)malloc(sizeof(struct FIFOQueue));
    q->front = q->rear = NULL;
    q->size = 0;
    q->totalJobs = 0;
    return q;
}

// ADD AN EVENT TO FIFO QUEUE
void add(struct FIFOQueue* q, struct Event* event) {

    // if FIFO Queue is empty, then new node is front and rear
    if (q->rear == NULL) {
        q->front = event;
        q->rear = event;
        q->size =1;
        q->totalJobs = (q->totalJobs)+1;
        return;
    }

    if(q->front == NULL) {
        q->front = event;
        q->rear = event;
        q->size = 1;
    }

    // if it is not empty, add the new node at the end of FIFO Queue and change rear
    q->size = q->size +1;
    q->rear->nextFIFO = event;
    q->rear = event;
    q->totalJobs = (q->totalJobs)+1;
}

// REMOVE A KEY FROM FIFO QUEUE
struct Event* removeQ(struct FIFOQueue* q) {

    // if queue is empty, return NULL
    if (q->front == NULL) {
        return NULL;
    }

    // store previous front and move front one node ahead
    struct Event* temp = q->front;
    q->front = q->front->nextFIFO;
    q->size = q->size -1;

    // if front is NULL, then change rear also to NULL
    if (q->front == NULL) {
        q->rear = NULL;
        q->size = 0;
    }
    return temp;
}

// PRIORITY QUEUE DATA STRUCTURE FOR EVENT HANDLING

// CREATE STRUCT FOR PRIORITY QUEUE
struct PQueue {
    struct Event *front, *rear;
};

// CREATE EMPTY PRIORITY QUEUE
struct PQueue* createPQueue() {
    struct PQueue* q = (struct PQueue*)malloc(sizeof(struct PQueue));
    q->front = q->rear = NULL;
    return q;
}

// ADD NEW EVENT TO PRIORITY QUEUE
void addPQ(struct PQueue* q, struct Event* event) {

    // if priority queue is empty, then new event is front and rear
    if((q->front == NULL) && (q->rear == NULL)){
        q->front = q->rear = event;
        return;
    }

    // if queue is not empty, then add the new event to the queue based off the time (lower time = higher priority)
    int sorted = 1;
    int timeOfNew = event->time;

    struct Event* placement = q->front;

    int loop = 0;

    while(sorted == 1) {
        loop++;
        if (placement->time < timeOfNew) {
            // if at rear, add the event as the new rear
            if (q->rear == placement)  {
                placement->nextPQ = event;
                q->rear =event;
                sorted = 0;
                return;
            }

            // if the next event's time is greater than or equal
            if (placement->nextPQ->time >= timeOfNew) {
                struct Event* temp = placement->nextPQ;
                placement->nextPQ = event;
                event->nextPQ = temp;
                sorted = 0;
                return;
            }

            // cannot sort here
            sorted = 1;
            placement = placement->nextPQ;
            continue;
        }

        // if the placement to check is the same as the event trying to be added, sort
        if (placement->time == timeOfNew) {
            // if not at the rear, point to event after that
            if (q->rear != placement) {
                struct Event* temp = placement->nextPQ;
                placement->nextPQ = event;
                event->nextPQ = temp;
                sorted = 0;
                return;
            }

            // if at the rear, set as the rear after placement
            else {
                placement->nextPQ = event;
                q->rear = event;
                sorted = 0;
                return;
            }
        }

        // if the placement is bigger than the event trying to be added, sort if at rear
        if (placement->time > timeOfNew) {
            struct Event* temp = placement;
            q->front = event;
            event->nextPQ = temp;
            sorted = 0;
            return;
        }
    }
}

// REMOVE A KEY FROM QUEUE
struct Event* removePQ(struct PQueue* q) {

    if (q->front == NULL) {
        return NULL;
    }

    // store previous front and move front one node ahead
    struct Event* temp = q->front;
    q->front = q->front->nextPQ;

    // if front is null, then change rear to NULL
    if (q->front == NULL) {
        q->rear = NULL;
    }

    return temp;
}

//                                       LOG FILE CREATION


FILE* createLogFile(char* name) {
    FILE *logFile = fopen(name, "w");
    if(logFile == NULL) {
        printf("Unable to create log file.\n");
        exit(1);
    }
    else {
        return logFile;
    }
}

// WRITE TO LOG FILE
void addLog(FILE* logFile, struct Event* event) {

    int eNum = event->eventNum;
    int eTime = event->time;

    if (event->eventType == 0) {
        fprintf(logFile, ("At time %d, process %d enters system.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 1) {
        fprintf(logFile, ("At time %d, process %d enters CPU.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 2) {
        fprintf(logFile, ("At time %d, process %d exit CPU.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 3) {
        fprintf(logFile, ("At time %d, process %d begins I/O on Disk 1.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 4) {
        fprintf(logFile, ("At time %d, process %d ends I/O on Disk 1.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 5) {
        fprintf(logFile, ("At time %d, process %d begins I/O on Disk 2.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 6) {
        fprintf(logFile, ("At time %d, process %d ends I/O on Disk 2.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 7) {
        fprintf(logFile, ("At time %d, process %d begins I/O on network.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 8) {
        fprintf(logFile, ("At time %d, process %d ends I/O on network.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 9) {
        fprintf(logFile, ("At time %d, process %d exits the system.\n"), eTime, eNum);
        return;
    }
    if(event->eventType == 10) {
        fprintf(logFile, ("At time %d, process %d ends the simulation.\n"), eTime, eNum);
        return;
    }
    else {
        fprintf(logFile, ("Something went wrong, event type %d\n"), event->eventType);
        return;
    }

}

// CREATE STATISTICS
struct STATS {
    struct FIFOQueue* component;
    double averageSize;
    double maxSize;
    struct Event* forEventNumSIZE;
    struct FIFOQueue* sizes;

    double avgResponseTime;
    double maxResponseTime;
    int entered;
    int exit;
    struct Event* forEventNumRTIME;
    struct FIFOQueue* responseTimes;

    double utilization;
    int totalResponseTime;

    double throughput;
    int jobs;
};

struct STATS* createSTATS(struct FIFOQueue* component) {
    struct STATS* new = (struct STATS*)malloc(sizeof(struct STATS));
    new->sizes = createQueue();
    new->responseTimes = createQueue();
    new->component = component;
    new->jobs = 0;
    return new;
}

struct STATS* updateStats(struct STATS* component1, struct Event* event) {
    component1->jobs = component1->jobs +1;
    int toUpdate = event->eventType;
    if((toUpdate == 1) || (toUpdate == 3) || (toUpdate == 5)|| (toUpdate == 7)) {
        component1->entered = event->time;
    }

    if(toUpdate == 2 || toUpdate == 4 || toUpdate == 6 || toUpdate == 8) {
        component1->exit = event->time;
        int difference = (component1->exit)-(component1->entered);
        component1->forEventNumRTIME = newEvent(difference);
        add(component1->responseTimes, component1->forEventNumRTIME);
    }

    // find size of input queue at this event and add to queue
    struct Event* sizeIsAdded = newEvent(component1->component->size);
    component1->forEventNumSIZE = sizeIsAdded;
    add(component1->sizes, component1->forEventNumSIZE);
    return component1;
}

void finishStats(FILE* statsFile, struct STATS* completed, int type) {
    // get average and maximum size of queue
    int currentSize;
    int biggestSize = -1;
    int totalSize = 0;
    int jobs = 0;
    // event to check
    struct Event* check;
    int doWeCalculate = 0;
    if(completed->sizes->front == NULL) {
        doWeCalculate = 1;
    }

    while (completed->sizes->front != NULL) {
        check = removeQ(completed->sizes);
        currentSize = check->eventNum;
        totalSize = totalSize + currentSize;
        if(currentSize > biggestSize) {
            biggestSize = currentSize;
        }
        jobs = jobs +1;
    }

    completed->jobs = jobs;
    completed->throughput = (completed->jobs)/FIN_TIME;
    if(doWeCalculate == 0) {
        completed->averageSize = (totalSize/jobs);
        completed->maxSize = biggestSize;
    }
    else {
        completed->averageSize = 0;
        completed->maxSize = 0;
    }

    int currentResponseTime;
    int biggestResponseTime = -1;
    int totalResponseTime = 0;
    doWeCalculate = 0;
    if(completed->responseTimes->front == NULL) {
        doWeCalculate = 1;
    }

    // get average and maximum response time of queue and find utilization
    while (completed->responseTimes->front != NULL) {
        check = removeQ(completed->responseTimes);
        currentResponseTime = check->eventNum;
        totalResponseTime = totalResponseTime + currentResponseTime;
        if(currentResponseTime > biggestResponseTime) {
            biggestResponseTime = currentResponseTime;
        }
    }
    if (doWeCalculate == 0) {
        completed->avgResponseTime = (totalResponseTime/jobs);
        completed->maxResponseTime = biggestResponseTime;
    }
    else {
        completed->avgResponseTime = 0;
        completed->maxResponseTime = 0;
    }

    completed->utilization = (totalResponseTime/FIN_TIME);
    if(jobs != 0){
        completed->throughput = (jobs/FIN_TIME);
    }
    else {
        completed->throughput = 0;
    }

    fprintf(statsFile, ("Stats for %d Queue: \n"), type);
    fprintf(statsFile, ("Average size of %d- %f\n"), type, completed->averageSize);
    fprintf(statsFile, ("Max size of %d- %f\n"), type, completed->maxSize);
    fprintf(statsFile, ("Utilization of %d- %f\n"), type, completed->utilization);
    fprintf(statsFile, ("Average response time of %d- %f\n"), type, completed->avgResponseTime);
    fprintf(statsFile, ("Max response time of %d- %f\n"), type, completed->maxResponseTime);
    fprintf(statsFile, ("Throughput of %d- %f\n"), type, completed->throughput);
}

// DETERMINE ARRIVAL TIME OF EVENTS
int determineTime(int min, int max) {
    int timeNeeded;
    timeNeeded = (rand()%(max-min+1))+min;
    return timeNeeded;
}

// DETERMINE PROBABILITY FOR QUIT, NETWORK, I/O
int determineProbability(double probType) {
    int boolean;
    double place;
    place = (rand()%100);
    if (place >= (probType*100)) {
        boolean = 0; // 0 executes
    }
    else {
        boolean = 1; // 1 doesn't execute
    }
    return boolean;
}

// DETERMINE WHICH DISK EVENT GOES TO (1 OR 2)
int checkWhichDisk(struct FIFOQueue* disk1, struct FIFOQueue* disk2) {
    int whichDisk = -1;

    // disk 1 isn't busy
    if(disk1->front == NULL) {
        whichDisk = 1;
    }

    // disk 2 isn't busy
    if(disk2->front == NULL){
        whichDisk = 2;
    }

    // if both are busy, see which one is less busy
    else {
        int disk1Count = 0;
        int disk2Count = 0;
        struct Event* placement1 = disk1->front;
        struct Event* placement2 = disk2->front;;

        while (placement1 != NULL) {
            disk1Count = disk1Count +1;
            placement1 = placement1->nextFIFO;
        }

        while (placement2 != NULL) {
            disk2Count = disk2Count +1;
            placement2 = placement2->nextFIFO;
        }

        if (disk1Count <= disk2Count) {
            whichDisk = 1;
            return whichDisk;
        }

        else {
            whichDisk = 2;
        }
    }
    return whichDisk;
}

// HANDLE ARRIVAL EVENT
void handleArrival(struct PQueue* priority, struct FIFOQueue* CPU, struct Event* job) {
    if((CPU->occupied == 0) || (CPU->front != NULL)) {
        add(CPU, job);
    }
    // if the CPU is not busy, send event to CPU
    else{
        // add to priority queue
        job->eventType = 1;
        CPU->occupied = 0;
        addPQ(priority, job);
    }

    // create new event for arrival to continue with processes
    struct Event* new = newEvent(job->eventNum+1);
    new->eventType = 0;
    new->time = job->time + determineTime(ARRIVE_MIN, ARRIVE_MAX);
    addPQ(priority, new);
}

// HANDLE CPU ARRIVAL EVENT
void handleArrivalCPU(struct PQueue* priority, struct Event* job) {
    job->eventType = 2;
    job->time = job->time + determineTime(CPU_MIN, CPU_MAX);
    addPQ(priority, job);
}

// HANDLE CPU FINISH EVENT
void handleFinishCPU(struct PQueue* priority, struct FIFOQueue* CPU, struct FIFOQueue* disk1, struct FIFOQueue* disk2, struct FIFOQueue* network, struct Event* job) {
    CPU->occupied = 1;
    // determine prob of exiting system
    int quitProb = determineProbability(QUIT_PROB);
    // if it quits, add the next to the priority queue for processing
    if (quitProb == 0) {
        job->eventType = 9;
        addPQ(priority, job);
    }

    // if it doesn't quit, check if it will go to network or disk
    else {
        int networkProb = determineProbability(NET_PROB);

        if (networkProb == 0) {

            // if the network is free, add new event to priority queue
            if ((network->occupied == 1) || (network->front == NULL)) {
                job->eventType = 7;
                addPQ(priority, job);
                network->occupied = 0;
            }

            // if the network is busy, add to the network queue
            else {
                add(network, job);
            }

        }

        // if it doesn't go to network, call function to determine which disk it goes to
        else {

            int diskChoice = checkWhichDisk(disk1, disk2);


            if (diskChoice == 1) {
                if ((disk1->occupied == 1) || (disk1->front == NULL)) {
                    job->eventType = 3;
                    addPQ(priority, job);
                    disk1->occupied = 0;
                }

                else {
                    add(disk1, job);
                }
            }

            // diskChoice == 2
            else {

                if ((disk2->occupied == 1) || (disk2->front == NULL)) {
                    job->eventType = 5;
                    addPQ(priority, job);
                    disk2->occupied = 0;
                }

                else {
                    add(disk2, job);
                }
            }
        }
    }

    if (CPU->front != NULL) {
        struct Event* nextProcessed = removeQ(CPU);
        if (nextProcessed->eventType != 1) {
            nextProcessed->eventType = 1;
            nextProcessed->time = job->time;
        }
        addPQ(priority, nextProcessed);
        CPU->occupied = 0;
    }

}

// HANDLE DISK 1 ARRIVAL EVENT
void handleArrivalDisk1(struct PQueue* priority, struct Event* job) {
    job->time = job->time + determineTime(DISK1_MIN, DISK1_MAX);
    job->eventType = 4;
    addPQ(priority, job);
}

// HANDLE DISK 1 FINISH EVENT
void handleFinishDisk1(struct PQueue* priority, struct FIFOQueue* CPU, struct FIFOQueue* disk1, struct Event* job) {
    disk1->occupied = 1;

    int count = 0;
    while (count != 1) {
        if ((CPU->occupied == 0) || (CPU->front != NULL)) {
            add(CPU, job);
        }
        else {
            job->eventType = 1;
            addPQ(priority, job);
            CPU->occupied = 0;
        }
        count = count+1;
    }

    if (disk1->front != NULL) {
        struct Event* nextProcessed = removeQ(disk1);
        addPQ(priority, nextProcessed);
        disk1->occupied = 0;
    }
}

// HANDLE DISK 2 ARRIVAL EVENT
void handleArrivalDisk2(struct PQueue* priority, struct Event* job) {
    job->time = job->time + determineTime(DISK2_MIN, DISK2_MAX);
    job->eventType = 6;
    addPQ(priority, job);

}

// HANDLE DISK 2 FINISH EVENT
void handleFinishDisk2(struct PQueue* priority, struct FIFOQueue* CPU, struct FIFOQueue* disk2, struct Event* job) {
    disk2->occupied = 1;

    int count = 0;
    while (count != 1) {
        if ((CPU->occupied == 0) || (CPU->front != NULL)) {
            add(CPU, job);
        }
        else {
            job->eventType = 1;
            addPQ(priority, job);
            CPU->occupied = 0;
        }
        count = count +1;
    }
    if (disk2->front != NULL) {
        struct Event* nextProcessed = removeQ(disk2);
        addPQ(priority, nextProcessed);
        disk2->occupied = 0;
    }
}

// HANDLE NETWORK ARRIVAL EVENT
void handleArrivalNetwork(struct PQueue* priority, struct Event* job) {
    job->time = job->time + determineTime(NETWORK_MIN, NETWORK_MAX);
    job->eventType = 8;
    addPQ(priority, job);

}

// HANDLE NETWORK FINISH EVENT
void handleFinishNetwork(struct PQueue* priority, struct FIFOQueue* CPU, struct FIFOQueue* network, struct Event* job) {
    network->occupied = 1;

    int count = 0;
    while (count != 1) {
        if ((CPU->occupied == 0) || (CPU->front != NULL)) {
            add(CPU, job);
        }
        else {
            job->eventType = 1;
            addPQ(priority, job);
            CPU->occupied = 0;
        }
        count = count+1;
    }

    if (network->front != NULL) {
        struct Event* nextProcessed = removeQ(network);
        addPQ(priority, nextProcessed);
        network->occupied = 0;
    }
}

// HANDLE EXIT SYSTEM EVENT
void handleExitSystem(struct Event* job) {
    return;
}

// HANDLE FINISH EVENT
void handleFinish(FILE* statsFile, FILE* logFile, struct PQueue* priority, struct Event* job) {
    priority->front = NULL;
    fclose(logFile);
    fclose(statsFile);
}

// MAIN FUNCTION
int main(int argc, char **argv) {
    readFile("config.txt");
    time_t seed;
    srand((unsigned)time(&seed));
    struct FIFOQueue* CPU = createQueue();
    CPU->occupied = 1;
    struct FIFOQueue* disk1 = createQueue();
    disk1->occupied = 1;
    struct FIFOQueue* disk2 = createQueue();
    disk2->occupied = 1;
    struct FIFOQueue* network = createQueue();
    network->occupied = 1;
    struct PQueue* priority = createPQueue();
    FILE* logFile = createLogFile("log_file");
    FILE* statsFile = createLogFile("stats_file");
    struct STATS* sCPU = createSTATS(CPU);
    struct STATS* sDisk1 = createSTATS(disk1);
    struct STATS* sDisk2 = createSTATS(disk2);
    struct STATS* sNetwork = createSTATS(network);

    // initialize priority queue
    struct Event* event1 = newEvent(1);
    event1->eventType = 0;
    event1->time = INIT_TIME;
    addPQ(priority, event1);

    // end for priority queue
    struct Event* eventLast = newEvent(0);
    eventLast->eventType = 10;
    eventLast->time = FIN_TIME;
    addPQ(priority, eventLast);

    // event to be pulled from queue
    struct Event* event;

    while (priority->front != NULL) {

        event = removePQ(priority);

        if (event->eventType == 0) {
            addLog(logFile, event);
            handleArrival(priority, CPU, event);
            continue;
        }

        if (event->eventType == 1) {
            addLog(logFile, event);
            sCPU = updateStats(sCPU, event);
            handleArrivalCPU(priority, event);
            continue;
        }

        if (event->eventType == 2) {
            addLog(logFile, event);
            sCPU = updateStats(sCPU, event);
            handleFinishCPU(priority, CPU, disk1, disk2, network, event);
            continue;
        }

        if (event->eventType == 3) {
            addLog(logFile, event);
            sDisk1 = updateStats(sDisk1, event);
            handleArrivalDisk1(priority, event);
            continue;
        }

        if (event->eventType == 4) {
            addLog(logFile, event);
            sDisk1 = updateStats(sDisk1, event);
            handleFinishDisk1(priority, CPU, disk1, event);
            continue;

        }
        if (event->eventType == 5) {
            addLog(logFile, event);
            sDisk2 = updateStats(sDisk2, event);
            handleArrivalDisk2(priority, event);
            continue;

        }
        if (event->eventType == 6) {
            addLog(logFile, event);
            sDisk2= updateStats(sDisk2, event);
            handleFinishDisk2(priority, CPU, disk2, event);
            continue;

        }
        if (event->eventType == 7) {
            addLog(logFile, event);
            sNetwork = updateStats(sNetwork, event);
            handleArrivalNetwork(priority, event);
            continue;

        }
        if (event->eventType == 8) {
            addLog(logFile, event);
            sNetwork = updateStats(sNetwork, event);
            handleFinishNetwork(priority, CPU, network, event);
            continue;

        }
        if (event->eventType == 9) {
            addLog(logFile, event);
            handleExitSystem(event);
            continue;

        }
        else {
            addLog(logFile, event);
            finishStats(statsFile, sCPU, 1);
            finishStats(statsFile, sDisk1, 2);
            finishStats(statsFile, sDisk2, 3);
            finishStats(statsFile, sNetwork, 4);
            handleFinish(statsFile, logFile, priority, event);

        }
    }
    return 0;
}








