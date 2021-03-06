#include "core/mutex.h"
#include <stdexcept>

Semaphore::Semaphore(uint32_t count)
{
    if(sem_init(&m_Semaphore, 0, count)) {
        throw std::logic_error("sem_init error");
    }
}


Semaphore::~Semaphore() {
    sem_destroy(&m_Semaphore);
}


void Semaphore::wait() {
    if(sem_wait(&m_Semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}

