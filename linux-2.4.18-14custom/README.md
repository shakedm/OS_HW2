# OS_TestPart2 🚀

## Caveat

These tests might not be 100% accurate, may contain mistakes. 
Please use it at your discretion.

## Requirements
In order to run this test will need to create and put in the same directory the following files: 
* `stream_log.txt` An empty file you must add. The test will use this file in order to write in the values of the log, so that you can check it in case of failure.
* `stream.txt` An empty file that you must add as well, for test internal use only.
* Your `hw2_syscalls.h` file.
* `test_utilities.h` 
## What this test will check and how
This test is composed by two functions: 
* `test_start_lottery_orig_scheduler` This function will check some easy return values and a little bit of stress on the system.
* `test_log_lottery` This function will instead test the actual content of the log while the LOTTERY_SCHEDULER is on. Please note: the Lottery Scheduler is conceived in a probabilistic way. What does it mean? This test will create a father process A with priority 70 and its son B having priority 90. Moreover, the system will hold 100 tickets. So probabilistically 70 times out of 100, the process A will take over (and of course the other 30 times B will). Thus, the tests after 500 context switches will return success if B will took over  between the 27% - 33% of the context switches amount. So there might be exceptions in which even if your kernel works correctly, the test might give you failure (but you need to be very, very unlucky :p). 

## Test timing
The test `test_log_lottery` should take aporximately 4-5 minutes, should not take any longer. 
If your machine is still frozen after such amount of time, it most likely means you have a bug somewhere :( 
