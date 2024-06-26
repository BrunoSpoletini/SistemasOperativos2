/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "thread_test_simple.hh"
#include "system.hh"
#include "semaphore.hh"

#include <stdio.h>
#include <string.h>

#include <iostream>
#include <string>

#include <stdlib.h>

/// Loop 10 times, yielding the CPU to another ready thread each iteration.
///
/// * `name` points to a string with a thread name, just for debugging
///   purposes.

int numThreads = 5;
int threadNDone[5];

#ifdef SEMAPHORE_TEST 
    static Semaphore *semaphore = new Semaphore("test", 3);
#endif

void
SimpleThread(void *name_)
{
    #ifdef SEMAPHORE_TEST
        DEBUG('s', "El thread `%s` aumenta el semaforo en uno\n", currentThread->GetName());
        semaphore->V();
    #endif
    // If the lines dealing with interrupts are commented, the code will
    // behave incorrectly, because printf execution may cause race
    // conditions.
    for (unsigned num = 0; num < 10; num++) {
        printf("*** Thread `%s` is running: iteration %u\n", currentThread->GetName(), num);
        currentThread->Yield();
    }
    int nThread = atoi(currentThread->GetName())-1;
	threadNDone[nThread] = 1;

    #ifdef SEMAPHORE_TEST
        DEBUG('s', "El thread `%s` disminuye el semaforo en uno\n", currentThread->GetName());
        semaphore->P();
    #endif

    printf("!!! Thread `%s` has finished SimpleThread\n", currentThread->GetName());
 
}

/// Set up a ping-pong between several threads.
///
/// Do it by launching one thread which calls `SimpleThread`, and finally
/// calling `SimpleThread` on the current thread.
void
ThreadTestSimple()
{   
    for (int i = 1; i < numThreads; i++) {
        threadNDone[i] = 0;
        char* threadName = new char[10];
        sprintf(threadName, "%d", i+1);
        Thread *newThread = new Thread( threadName );
        newThread->Fork(SimpleThread, NULL);
    }
    
    //the "main" thread also executes the same function
    SimpleThread(NULL);

    #ifdef SEMAPHORE_TEST
        DEBUG('s', "Main disminuye el semaforo en tres\n", currentThread->GetName());
        for (int i=0; i<3; i++){
            semaphore->P();
        }
    #endif

   //Wait for every thread to finish if needed
   for (int i = 1 ; i < numThreads; i++) {
       while (threadNDone[i] == 0) {
           currentThread->Yield();
       }
   }

    printf("Test finished\n");
}
