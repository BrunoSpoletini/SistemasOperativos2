/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2007-2009 Universidad de Las Palmas de Gran Canaria.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "thread_test_prod_cons.hh"


#include <stdio.h>
#include "system.hh"
#include <unistd.h>  

Lock *qlock = new Lock("lock");
Condition *condNoLleno = new Condition("qcond", qlock);
Condition *condNoVacio = new Condition("qcond", qlock);

List< int > queue;

bool bufferVacio(){
    return queue.Size() == 0;
}
bool bufferLLeno(){
    return queue.Size() == 3;
}

bool Done = false;

#define N 100

void productor(void *dummy){
    
    
    for(int i = 1; i < N; i++){
        
        qlock->Acquire();

        while( bufferLLeno() ){
            printf("Productor esperando (buffer lleno)\n");
            condNoLleno->Wait();
        }

        queue.Append(i);
        
        printf("Productor produce: %d en %d\n", i, queue.Size()-1);
        qlock->Release();

        condNoVacio->Signal();
    
    }
}

void consumidor(void *dummy){
    
    int elem;
    
    for(int i = 1; i < N; i++){

        qlock->Acquire();

        while ( bufferVacio() ){
            printf("Consumidor esperando (buffer vacio)\n");
            condNoVacio->Wait();
        }

        elem = queue.Pop();
        printf("Consumidor consume: %d en %d\n", elem, queue.Size() );
        qlock->Release();

        condNoLleno->Signal();

    }

    Done = true;

}

void
ThreadTestProdCons()
{
    Thread *prod = new Thread( "Productor" );
    prod->Fork(productor, NULL);
    
    Thread *cons = new Thread( "Consumidor" );
    cons->Fork(consumidor, NULL);

    while( !Done )
        currentThread->Yield();
}
