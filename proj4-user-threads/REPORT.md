# Project 4: User-level threads
* Name:	Michael Bruno
* Hours Spent: 15
* Level attempted: 75

## Summary
The behavior of the test program runs as expected. After the threads are first created, _ut_join_ is called on thread C which is the shortest running thread. Thread C runs till _ut_finish_ is called. The program then proceeds to calling _ut_join_ thread B. Both thread A and B take the same amount of time to run and so they call _ut_finish_ at the same time. _ut_join_ is then called on thread B which has already completed which is then ignored. Thread D runs longer so then continues and completes as expected. Because of the _ut_join_ calls, the code to print the return values of each thread does not run until each thread has completed.

* test.prog.c

User-level Threads Scheduler
Hello! I'm the main thread, my ID is 0.
Now I'll create another 3 threads and wait until their jobs finish!
Joining thread (3).
 -=A1=-
 -=B2=-
 -=C3=-
 -=D4=-
 -=C3=-
 -=C3=-
 -=C3=-
 -=C3=-
 -=C3=-
 -=C3=-
 -=C3=-
 -=C3=-
 -=C3=-
Thread (3) finishing.
Joining thread (2).
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
 -=A1=-
 -=B2=-
 -=D4=-
Thread (1) finishing.
Thread (2) finishing.
 -=D4=-
Joining thread (1).
Joining thread (4).
 -=D4=-
 -=D4=-
 -=D4=-
 -=D4=-
 -=D4=-
 -=D4=-
 -=D4=-
 -=D4=-
 -=D4=-
Thread (4) finishing.


Thread A returned 0
Thread B returned 0
Thread C returned 0
Thread D returned 0
I`m also done ! Bye ...


## Academic Integrity Statement
* I verify that I have recieved no help from anyone other than the help indicated.