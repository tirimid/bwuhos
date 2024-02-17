#include "katomic.h"

#if defined(K_ARCH_X86_64)
#include "arch/aatomic.h"
#endif

void
k_mutex_lock(k_mutex_t *mutex)
{
#if defined(K_ARCH_X86_64)
	aatomic_set(mutex);
#endif
}

void
k_mutex_unlock(k_mutex_t *mutex)
{
#if defined(K_ARCH_X86_64)
	aatomic_clr(mutex);
#endif
}
