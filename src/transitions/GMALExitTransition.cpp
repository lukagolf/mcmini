#include "GMALExitTransition.h"

GMALTransition* GMALReadExitTransition(const GMALSharedTransition *shmTransition, void *shmStart, GMALState *programState)
{
    auto executor = programState->getThreadWithId(shmTransition->executor);
    auto exitCode = *(int*)shmStart;
    return new GMALExitTransition(executor, exitCode);
}

std::shared_ptr<GMALTransition>
GMALExitTransition::staticCopy() {
    // INVARIANT: Target and the thread itself are the same
    auto threadCpy=
            std::static_pointer_cast<GMALThread, GMALVisibleObject>(this->thread->copy());
    auto threadStartCpy = new GMALExitTransition(threadCpy, exitCode);
    return std::shared_ptr<GMALTransition>(threadStartCpy);
}

std::shared_ptr<GMALTransition>
GMALExitTransition::dynamicCopyInState(const GMALState *state) {
    // INVARIANT: Target and the thread itself are the same
    std::shared_ptr<GMALThread> threadInState = state->getThreadWithId(thread->tid);
    auto cpy = new GMALExitTransition(threadInState, exitCode);
    return std::shared_ptr<GMALTransition>(cpy);
}

bool
GMALExitTransition::dependentWith(std::shared_ptr<GMALTransition>)
{
    return false;
}

bool
GMALExitTransition::enabledInState(const GMALState *)
{
    return false; // Never enabled
}

void
GMALExitTransition::print()
{
    puts("************************");
    puts(" -- EXIT PROGRAM -- ");
    this->thread->print();
    puts("************************");
}

bool
GMALExitTransition::ensuresDeadlockIsImpossible()
{
    return true;
}

