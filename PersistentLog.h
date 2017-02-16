// PersistentLog.h

#ifndef _PERSISTENTLOG_h
#define _PERSISTENTLOG_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "IPersistentLog.h"

extern IPersistentLog * PersistentLog;

#define PersistentLogWrite(X) if(PersistentLog) {Print * f=PersistentLog->open(); if(f) {f->println(X); PersistentLog->close();}}

#endif

