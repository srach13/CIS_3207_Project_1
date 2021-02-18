
                                                                      HOW THE PROGRAM WORKS
                                                                      
A job arrives to the main event queue. The event queue is a priority queue where the earliest times have the highest priority. The job spends a certain amount of time using the CPU; the time is determined at runtime as a random interval uniformly distributed between two defined constants. Then, the job has two choices. It can either perform I/O on one of the two disks or send a message to the network and return to use the CPU, or it can terminate. The path the job takes is determined by a probability function. When a job needs to do disk I/O, it will use the disk that’s the least busy, or the disk with the shortest queue. If the disk queues are of equal length, the program chooses one of the disks at random. When a job reaches a component i.e. CPU, disk1, disk2, or network, the job immediately begins service on the component if that component is free. If the component is busy servicing another task, the job must wait in that component’s queue. The queue for each system component is FIFO.
                                                                 PARAMETERS TAKEN FROM FILE INPUT
                                                                  
Program reads from text file every time it runs, grabs value of each parameter, and stores each parameter somewhere in the program.
                                                                       
                                                                       PROBABILITY FUNCTIONS

Srand(int) runs at beginning of program to set a seed for RNG
Rand() returns a random number between a given max and min to find amount of time that a job spends using the component (determined at runtime for each component arrival)
Once a job has finished using the CPU, the probability that it completes and exits the system instead of doing a disk read or network send is QUIT_PROB
Once a job has been determined to continue executing, the probability that it d oes disk I/O or uses the network is NET_PROB

                                                                    STRUCTS FOR QUEUES AND EVENTS

                                                          1 FIFO queue for each component (CPU, disk1, disk2, network)
                                                          1 priority queue to store events
struct queue {
int size;
int busy; // busy = 1; not busy = 0;
struct event *array;
};

struct event {
 	int time;
int job_number;
int event_type;
};

                                                                             TIME
                                                                             
Simulation begins at INIT_TIME (0). Time functions as arbitrary units rather than systematic and linear. EX: Job 6 entered the CPU at time 15 and spent 10 units of time at the CPU. New jobs enter the system with an interarrival time that is uniformly distributed between ARRIVE_MIN and ARRIVE_MAX. Time an event finishes using the CPU = the time of arrival + the time spent using the CPU. The event time of an event removed from the priority queue establishes a new “current time” for the simulation. Simulation terminates at FIN_TIME.

                                                                          MAIN FUNCTION

1. Removes events from priority queue 
2. Handles the event
-> Switch statement with different job types 
-> If else block with different instructions for each job type
While the priority queue is not empty and the time is not fin_time, remove an event from the priority queue. 
Example: If the job type is a CPU_ARRIVAL, create a new job, push the job to the priority queue, and push the job to the CPU queue. Otherwise, work with the existing job based on whether the prob says the job should not quit, go to disk, go to network. If it goes to disk, create new job for the smallest disk queue and add job to the priority queue. If the job goes to network, create new job for the network queue and add job to priority queue. If the job quits, then the job finishes. If the CPU queue is not empty and CPU does not have a job running, deque the job from the queue. Create new CPU_FINISHED job with random execute time. Push finished job to priority queue. Set CPU status to busy. 

                                                                            EVENT HANDLERS
New Job Arrival Handler
Create a new NEW_JOB event, push NEW_JOB event to priority queue, create a new CPU_ARRIVAL event, push CPU_ARRIVAL event to the priority queue

CPU Arrival Handler
If the CPU is busy, add the CPU_ARRIVAL event to the CPU Queue. Otherwise, create new CPU_FINISHED event, push new CPU_FINISHED event to the priority queue, set CPU equal to busy

CPU Finished Handler
If the job should be terminated, create JOB_TERMINATED event and push it to the priority queue. If the job should go to the network, create NETWORK_ARRIVAL event and push it to the priority queue. If the job should go to disk, see which queue is smaller. If disk1 queue is smaller than disk2 queue, create DISK1_ARRIVAL event and push it to the priority queue. If disk2 queue is smaller than disk1 queue, create DISK2_ARRIVAL event and push it to the priority queue
		
DISK Arrival Handlers (One handler for DISK1, one for DISK2)
If the disk is busy, add the DISK_ARRIVAL event to the queue. Otherwise, create new DISK_FINISHED event. Push new DISK_FINISHED event to the priority queue. Set DISK = to busy.

DISK Finished Handler
Remove the job from the disk, return the job to CPU. If the disk’s queue isn’t empty, remove a job from it, determine how long the job will require using the disk, create a new “disk finished” event and add it to the event queue.

Network Arrival Handler
If the network is busy, add the NETWORK_ARRIVAL event to the queue. Otherwise, create new NETWORK_FINISHED event. Push new NETWORK_FINISHED event to the priority queue. Set NETWORK = to busy.

Network Finished Handler
Remove the job from the network, return the job to CPU. If the network’s queue isn’t empty, remove a job from it, determine how long the job will require using the network, create a new “network finished” event and add it to the event queue.
