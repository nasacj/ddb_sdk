#ifndef WINBASEIMP_EVENTSIGNAL_H_
#define WINBASEIMP_EVENTSIGNAL_H_

#include <Windows.h>
#include "../BaseIO.h"

class WinEvent : implements EventSignal
{
public:
	WinEvent();
	void setEvent();
	void reSetEvent();
	int wait(U32);
	~WinEvent();

private:
	HANDLE hEvent;
	//U32 timeout;
};

#endif

