#include "Locker.h"

WinLocker::WinLocker()
{
	InitializeCriticalSection(&(this->locker));
}
void WinLocker::lock(void)
{
	EnterCriticalSection(&(this->locker));
}
void WinLocker::unlock(void)
{
	LeaveCriticalSection(&(this->locker));
}
WinLocker::~WinLocker()
{
	DeleteCriticalSection(&(this->locker));
}