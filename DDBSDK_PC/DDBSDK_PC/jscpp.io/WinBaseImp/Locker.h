#ifndef WINBASEIMP_LOCKER_H_
#define WINBASEIMP_LOCKER_H_

#include <Windows.h>
#include "../BaseIO.h"

class WinLocker : implements Locker
{
public:
	
	WinLocker();
	void lock(void);
	void unlock(void);
	~WinLocker();

private:
	CRITICAL_SECTION locker;
};

#endif