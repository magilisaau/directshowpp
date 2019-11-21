#include "../stdAfx.h"

#ifndef INCLUDE_CRITICALSECTION
#define INCLUDE_CRITICALSECTION

/************************************************************************/
/* CCriticalSection                                                     */
/************************************************************************/

class CMyCriticalSection
{
protected:
	CRITICAL_SECTION m_cs;
public:
	CMyCriticalSection() { InitializeCriticalSection(&m_cs); }
	~CMyCriticalSection() { DeleteCriticalSection(&m_cs); }

	void Enter() { EnterCriticalSection(&m_cs); }
	void Leave() { LeaveCriticalSection(&m_cs); }
};

#endif
