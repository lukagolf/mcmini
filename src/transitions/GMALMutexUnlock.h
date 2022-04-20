#ifndef GMAL_GMALMUTEXUNLOCK_H
#define GMAL_GMALMUTEXUNLOCK_H

#include "GMALMutexTransition.h"
#include <memory>

GMALTransition* GMALReadMutexUnlock(const GMALSharedTransition*, void*, GMALState*);

struct GMALMutexUnlock : public GMALMutexTransition {
public:

    GMALMutexUnlock(std::shared_ptr<GMALThread> thread, std::shared_ptr<GMALMutex> mutex) : GMALMutexTransition(thread, mutex) {}



    std::shared_ptr<GMALTransition> staticCopy() override;
    std::shared_ptr<GMALTransition> dynamicCopyInState(const GMALState*) override;
    void applyToState(GMALState *) override;
    void unapplyToState(GMALState *) override;
    bool coenabledWith(std::shared_ptr<GMALTransition>) override;
    bool dependentWith(std::shared_ptr<GMALTransition>) override;
    bool enabledInState(const GMALState *) override;
};

#endif //GMAL_GMALMUTEXUNLOCK_H
