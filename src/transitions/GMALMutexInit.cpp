#include "MCMutexInit.h"

MCTransition*
MCReadMutexInit(const MCSharedTransition *shmTransition, void *shmData, MCState *state)
{
    auto mutexInShm = static_cast<MCMutexShadow*>(shmData);
    auto systemId = (MCSystemID)mutexInShm->systemIdentity;
    auto mutexThatExists = state->getVisibleObjectWithSystemIdentity<MCMutex>(systemId);

    if (mutexThatExists == nullptr) {
        auto newMutex = new MCMutex(*mutexInShm);
        auto newMutexSharedPtr = std::shared_ptr<MCVisibleObject>(newMutex);
        state->registerVisibleObjectWithSystemIdentity(systemId, newMutexSharedPtr);
        mutexThatExists = std::static_pointer_cast<MCMutex, MCVisibleObject>(newMutexSharedPtr);
    }

    tid_t threadThatRanId = shmTransition->executor;
    auto threadThatRan = state->getThreadWithId(threadThatRanId);
    return new MCMutexInit(threadThatRan, mutexThatExists);
}

std::shared_ptr<MCTransition>
MCMutexInit::staticCopy()
{
    auto threadCpy=
            std::static_pointer_cast<MCThread, MCVisibleObject>(this->thread->copy());
    auto mutexCpy =
            std::static_pointer_cast<MCMutex, MCVisibleObject>(this->mutex->copy());
    auto mutexInit = new MCMutexInit(threadCpy, mutexCpy);
    return std::shared_ptr<MCTransition>(mutexInit);
}

std::shared_ptr<MCTransition>
MCMutexInit::dynamicCopyInState(const MCState *state)
{
    std::shared_ptr<MCThread> threadInState = state->getThreadWithId(thread->tid);
    std::shared_ptr<MCMutex> mutexInState = state->getObjectWithId<MCMutex>(mutex->getObjectId());
    auto cpy = new MCMutexInit(threadInState, mutexInState);
    return std::shared_ptr<MCTransition>(cpy);
}

void
MCMutexInit::applyToState(MCState *state)
{
    this->mutex->init();
}

bool
MCMutexInit::coenabledWith(std::shared_ptr<MCTransition> other)
{
    return true;
}

bool
MCMutexInit::dependentWith(std::shared_ptr<MCTransition> other)
{
    auto maybeMutexOperation = std::dynamic_pointer_cast<MCMutexTransition, MCTransition>(other);
    if (maybeMutexOperation) {
        return *maybeMutexOperation->mutex == *this->mutex;
    }
    return false;
}

void
MCMutexInit::print()
{
    printf("thread %lu: pthread_mutex_init(%lu, _)\n", this->thread->tid, this->mutex->getObjectId());
}