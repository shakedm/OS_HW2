todo list:

log:
1) log struct wrapper OK	
2) log struct 			OK
3) update the log in context switch OK
4) field for enabling the log		OK
5) Start the global variabel in init_sched OK

schedule:
1) see how to recalculate the time slice when disabling the lottery.
2) make a field to save the prev policy of the process 						OK
3) make global variabels NT and SET_NT
4) make changes to setscheduler so the next task will be random lottery if  
SCHED_LOTTERY  is enabled
5) when enabling the policy go through all processes to change tasks policy
move all runqueue tasks to active and make sure they stay there!
6) how to calculate the NT when task is changing its prio? or on exit?
//maybe go on all the RUNNIG tasks and count?
//function that update NT??
7) make sure no process can go to expired
8) make a search function to find the task that won
9) make a random function to return a random number							OK
10) make sure all waking processes join active on our LOTTERY_SCHED

