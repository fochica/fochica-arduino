// 
// 
// 

#include "IPersistentLog.h"

void IPersistentLog::printDate(Print & out, const DateTime & d)
{
	out.print(d.year(), DEC);
	out.print('/');
	out.print(d.month(), DEC);
	out.print('/');
	out.print(d.day(), DEC);
	out.print(' ');
	out.print(d.hour(), DEC);
	out.print(':');
	out.print(d.minute(), DEC);
	out.print(':');
	out.print(d.second(), DEC);
}
