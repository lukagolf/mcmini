#include "GMAL.h"
#include "GMALSemaphoreTransitionWrappers.h"
#include "transitions/GMALTransitionsShared.h"
#include "transitions/GMALSemWait.h"
#include "transitions/GMALSemPost.h"
#include "transitions/GMALSemInit.h"

extern "C" {
    #include "GMALSharedLibraryWrappers.h"
}

int
gmal_sem_init(sem_t *sem, int pshared, unsigned int count)
{
    // TODO: We don't support shared semaphores
    GMAL_ASSERT(pshared == 0);
    auto newlyCreatedSemaphore = GMALSemaphoreShadow(sem, count);
    thread_post_visible_operation_hit<GMALSemaphoreShadow>(typeid(GMALSemInit), &newlyCreatedSemaphore);
    thread_await_gmal_scheduler();
    return __real_sem_init(sem, pshared, count);
}

int
gmal_sem_post(sem_t *sem)
{
    // NOTE: GMALReadSemPost doesn't use the count passed by the child; any
    // value suffices
    auto newlyCreatedSemaphore = GMALSemaphoreShadow(sem, 0);
    thread_post_visible_operation_hit<GMALSemaphoreShadow>(typeid(GMALSemPost), &newlyCreatedSemaphore);
    thread_await_gmal_scheduler();
    return __real_sem_post(sem);
}

int
gmal_sem_wait(sem_t *sem)
{
    // NOTE: GMALReadSemWait doesn't use the count passed by the child; any
    // value suffices
    auto newlyCreatedSemaphore = GMALSemaphoreShadow(sem, 0);
    thread_post_visible_operation_hit<GMALSemaphoreShadow>(typeid(GMALSemWait), &newlyCreatedSemaphore);
    thread_await_gmal_scheduler();
    return __real_sem_wait(sem);
}
