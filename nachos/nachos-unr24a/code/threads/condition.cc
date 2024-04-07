/// Routines for synchronizing threads.
///
/// The implementation for this primitive does not come with base Nachos.
/// It is left to the student.
///
/// When implementing this module, keep in mind that any implementation of a
/// synchronization routine needs some primitive atomic operation.  The
/// semaphore implementation, for example, disables interrupts in order to
/// achieve this; another way could be leveraging an already existing
/// primitive.
///
/// Copyright (c) 1992-1993 The Regents of the University of California.
///               2016-2021 Docentes de la Universidad Nacional de Rosario.
/// All rights reserved.  See `copyright.h` for copyright notice and
/// limitation of liability and disclaimer of warranty provisions.


#include "condition.hh"
#include "system.hh"

Condition::Condition(const char *debugName, Lock *conditionLock)
{
    lockCondition = conditionLock;
    name = debugName;
    threadCount = 0;
    threadContLock = new Lock("threadContLock");
    semaphore = new Semaphore("Semaphore", 0);
}

Condition::~Condition()
{
    delete semaphore;
    delete lockCondition;
}

const char *
Condition::GetName() const
{
    return name;
}

void
Condition::Wait() /// esperar a que la condicion asociada al lock se cumpla (por defecto no se cumple)
{

    lockCondition->Release();

    
    threadContLock->Acquire();
    threadCount++;
    threadContLock->Release();
    
    DEBUG('s', "El thread `%s` se duerme\n", currentThread->GetName() );
    semaphore->P();

    threadContLock->Acquire();
    threadCount--;
    threadContLock->Release();

    DEBUG('s', "El thread `%s` se levanta del wait del condicional %s\n", currentThread->GetName(), lockCondition->GetName()  );

    lockCondition->Acquire();
}

void
Condition::Signal() /// avisar que a alguna de las personas que la condicion se cumple
{
    DEBUG('s', "Se envio un Signal al condicional %s\n", lockCondition->GetName() );
    threadContLock->Acquire();
    semaphore->V();
    threadCount--;
    threadContLock->Release();
}

void
Condition::Broadcast() /// avisar a todos que la condicion se cumple
{
    DEBUG('s', "Se envio un Broadcast al condicional %s\n", lockCondition->GetName() );
    threadContLock->Acquire();

    for( int i = 0; i < threadCount; i++ )
        semaphore->V();
    
    threadCount = 0;
    threadContLock->Release();
}
