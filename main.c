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

// CREATE A NEW EVENT BASED ON PRIORITY (LOWER NUMBER -> HIGHER PRIORITY)
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







