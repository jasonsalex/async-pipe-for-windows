#ifndef _CFASTMUTEX_H
#define _CFASTMUTEX_H

#ifdef _WIN32

#include <Windows.h>

class CFastMutex
{
public:
	CFastMutex() {
		InitializeCriticalSection(&m_cs);
	};
	virtual ~CFastMutex() {
		DeleteCriticalSection(&m_cs);
	};
public:
	void Lock() {
		EnterCriticalSection(&m_cs);
	}
	void Unlock() {
		LeaveCriticalSection(&m_cs);
	}
private:
	CFastMutex(const CFastMutex&);
	CFastMutex& operator=(const CFastMutex&);
private:
	CRITICAL_SECTION m_cs;
};

#endif

class CMutexAutoLock
{
public:
	CMutexAutoLock(CFastMutex *pXMutex){
		m_pXMutex = pXMutex;
		if (m_pXMutex)
			m_pXMutex->Lock();
	}
	~CMutexAutoLock(){
		if (m_pXMutex)
			m_pXMutex->Unlock();
	}
private:
	CMutexAutoLock(const CMutexAutoLock&);
	CMutexAutoLock& operator=(const CMutexAutoLock&);
private:
	CFastMutex *m_pXMutex;
};

#endif