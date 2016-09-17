#ifndef ScopeGuard_h
#define ScopeGuard_h

#include <functional>

class ScopeGuard
{
public:
    explicit ScopeGuard(std::function<void()> onExitScope)
        : fOnExitScope(onExitScope), mDismissed(false)
    { }

    ~ScopeGuard() {
        if (!mDismissed) {
            fOnExitScope();
        }
    }

    void dismiss() {
        mDismissed = true;
    }

private:
    std::function<void()> fOnExitScope;
    bool mDismissed;

private: // noncopyable
    ScopeGuard(ScopeGuard const&);
    ScopeGuard& operator=(ScopeGuard const&);
};

#define SCOPEGUARD_LINENAME_CAT(name, line) name##line
#define SCOPEGUARD_LINENAME(name, line) SCOPEGUARD_LINENAME_CAT(name, line)

#define ON_SCOPE_EXIT(callback) ScopeGuard SCOPEGUARD_LINENAME(EXIT, __LINE__)(callback)

#endif /* ScopeGuard_h */
