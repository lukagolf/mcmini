#include "MCCondWait.h"
#include "MCMINI.h"
#include "transitions/mutex/MCMutexTransition.h"
#include "transitions/mutex/MCMutexLock.h"
#include "MCTransitionFactory.h"

MCTransition*
MCReadCondWait(const MCSharedTransition *shmTransition, void *shmData, MCState *state)
{
    const auto shmCond = static_cast<MCSharedMemoryConditionVariable*>(shmData);
    const auto condInShm = shmCond->cond;
    const auto mutexInShm = shmCond->mutex;
    const auto condSystemId = (MCSystemID)condInShm;
    const auto mutexSystemId = (MCSystemID)mutexInShm;
    const auto condThatExists = state->getVisibleObjectWithSystemIdentity<MCConditionVariable>(condSystemId);
    const auto mutexThatExists = state->getVisibleObjectWithSystemIdentity<MCMutex>(mutexSystemId);

    MC_REPORT_UNDEFINED_BEHAVIOR_ON_FAIL(condThatExists != nullptr, "Attempting to wait on a condition variable that is uninitialized");
    MC_REPORT_UNDEFINED_BEHAVIOR_ON_FAIL(mutexThatExists != nullptr, "Attempting to wait on a condition variable with an uninitialized mutex");
    MC_REPORT_UNDEFINED_BEHAVIOR_ON_FAIL(!condThatExists->isDestroyed(), "Attempting to wait on a destroyed condition variable");

    const auto threadThatRanId = shmTransition->executor;
    const auto mutexAssociatedWithConditionVariable = condThatExists->mutex;

    if (mutexAssociatedWithConditionVariable != nullptr) {
        MC_REPORT_UNDEFINED_BEHAVIOR_ON_FAIL(*mutexThatExists == *mutexAssociatedWithConditionVariable,
                                               "A mutex has already been associated with this condition variable. Attempting "
                                               "to use another mutex with the same condition variable is undefined");
    }

    auto threadThatRan = state->getThreadWithId(threadThatRanId);
    return new MCCondWait(threadThatRan, condThatExists);
}

std::shared_ptr<MCTransition>
MCCondWait::staticCopy()
{
    auto threadCpy=
            std::static_pointer_cast<MCThread, MCVisibleObject>(this->thread->copy());
    auto condCpy =
            std::static_pointer_cast<MCConditionVariable, MCVisibleObject>(this->conditionVariable->copy());
    auto cpy = new MCCondWait(threadCpy, condCpy);
    return std::shared_ptr<MCTransition>(cpy);
}

std::shared_ptr<MCTransition>
MCCondWait::dynamicCopyInState(const MCState *state)
{
    std::shared_ptr<MCThread> threadInState = state->getThreadWithId(thread->tid);
    std::shared_ptr<MCConditionVariable> condInState = state->getObjectWithId<MCConditionVariable>(conditionVariable->getObjectId());
    auto cpy = new MCCondWait(threadInState, condInState);
    return std::shared_ptr<MCTransition>(cpy);
}

void
MCCondWait::applyToState(MCState *state)
{
    const auto threadId = this->getThreadId();
    this->conditionVariable->mutex->lock(threadId);
    this->conditionVariable->removeThread(threadId); /* When we actually apply the wait, we are moving out of it */
}


bool
MCCondWait::enabledInState(const MCState *) {
    const auto threadId = this->getThreadId();
    return this->thread->enabled() &&
            this->conditionVariable->threadCanExit(threadId) &&
            this->conditionVariable->mutex->canAcquire(threadId);
}

bool
MCCondWait::coenabledWith(std::shared_ptr<MCTransition> other)
{
    auto maybeCondWaitOperation = std::dynamic_pointer_cast<MCCondWait, MCTransition>(other);
    if (maybeCondWaitOperation) {
        /* Only one cond_wait will be able to acquire the mutex */
        return *maybeCondWaitOperation->conditionVariable != *this->conditionVariable;
    }

    auto maybeMutexOperation = std::dynamic_pointer_cast<MCMutexTransition, MCTransition>(other);
    if (maybeMutexOperation) {
        auto lockMutex = std::make_shared<MCMutexLock>(this->thread, this->conditionVariable->mutex);
        return MCTransitionFactory::transitionsCoenabledCommon(lockMutex, maybeMutexOperation);
    }

    return true;
}

bool
MCCondWait::dependentWith(std::shared_ptr<MCTransition> other)
{
    auto maybeCondOperation = std::dynamic_pointer_cast<MCCondTransition, MCTransition>(other);
    if (maybeCondOperation) {
        return *maybeCondOperation->conditionVariable == *this->conditionVariable;
    }

    auto maybeMutexOperation = std::dynamic_pointer_cast<MCMutexTransition, MCTransition>(other);
    if (maybeMutexOperation) {
        auto lockMutex = std::make_shared<MCMutexLock>(this->thread, this->conditionVariable->mutex);
        return MCTransitionFactory::transitionsDependentCommon(lockMutex, maybeMutexOperation);
    }
    return false;
}

void
MCCondWait::print()
{
    printf("thread %lu: pthread_cond_wait(%lu, %lu) (asleep)\n", this->thread->tid, this->conditionVariable->getObjectId(), this->conditionVariable->mutex->getObjectId());
}