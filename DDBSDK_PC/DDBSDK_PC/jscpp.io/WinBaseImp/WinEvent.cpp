#include "WinEvent.h"

WinEvent::WinEvent()
{
	this->hEvent = CreateEvent(NULL,FALSE,FALSE,NULL);
}
void WinEvent::setEvent()
{
	SetEvent(this->hEvent);
}
void WinEvent::reSetEvent()
{
	ResetEvent(this->hEvent);
}
int WinEvent::wait(U32 timeout)
{
	return WaitForSingleObject(this->hEvent, timeout);
	
}
WinEvent::~WinEvent()
{
	CloseHandle(this->hEvent);
}