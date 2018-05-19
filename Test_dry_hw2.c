#include <limits.h>
#include <stdlib.h>
#include <sched.h>
#include <unistd.h>
#include "hw2_syscalls.h"
#include "test_utilities.h"
#include <stdlib.h>
#include <string.h>

#define SCHED_OTHER		0
#define SCHED_FIFO		1
#define SCHED_RR		2
#define SCHED_LOTTERY		3

#define MAXSTRING 255
#define TEST_SIZE 10000
#define LOG_SIZE 100
#define MAX_TICKETS 140

bool test_log_loterry(){
    disable_logging();
    start_orig_scheduler();
    cs_log log[LOG_SIZE]; //init of the log (maybe we want to export it?)
    int parent_pid = getpid();
	struct sched_param param;
	param.sched_priority = 29; //prio = 70, num_of_tickets = 70
	sched_setscheduler(parent_pid, SCHED_FIFO, &param);
	
	int child_pid = fork();
	param.sched_priority = 29; //prio = 70, num_of_tickets = 70
	sched_setscheduler(child_pid, SCHED_FIFO, &param);
    set_max_tickets(MAX_TICKETS); //max tickets is now 140
	enable_logging(LOG_SIZE);
    start_lottery_scheduler();
    int i, j, k;
	unsigned long long t=0;
	for (i = 0; i < TEST_SIZE; ++i) {
	   for (j = 0; j < TEST_SIZE; ++j) {
	      for (k = 0; k < LOG_SIZE; ++k) {
			  t++;
	      }
	   }
	}
    if (child_pid == 0) {
	   exit(0);
	}
	wait(NULL);
    disable_logging();
    start_orig_scheduler();
    int logger_size = get_logger_records(log);

    FILE* log_results= fopen("log_result.txt","a");
    if(!log_results){
        printf("WTF??\n");
        return false;
    }
    fprintf(log_results, "\tprev \t next\t prev_priority\t next_priority\t prev_policy\t next_policy\t n_tickets\t switch_time \n\n");
    for(i=0; i<logger_size ; i++){
     fprintf(log_results, "log[%d]:\t %d\t %d\t %d\t %d\t%d\t %d\t %d\t %d \n\n", 
     i, log[i].prev, log[i].next, log[i].prev_priority, log[i].next_priority, log[i].prev_policy, log[i].next_policy, log[i].n_tickets, log[i].switch_time);
    }
    fclose(log_results);
    return true;
}

int main(){
    RUN_TEST(test_log_loterry);
	return 0;
}




