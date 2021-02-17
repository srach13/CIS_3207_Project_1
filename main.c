#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define SEED 2
#define INIT_TIME 0
#define FIN_TIME 10000
#define ARRIVE_MIN 50
#define ARRIVE_MAX 60
#define QUIT_PROB 0.2
#define NETWORK_PROB 0.2
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











