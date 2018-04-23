/*
Fochica™ - Forgotten Child in Car Alert - http://fochica.com/
Copyright (c) 2017, AY Garage Ltd.  All rights reserved.

Please see DISCLAIMER.md for important legal terms.

This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// 
// 
// 

#include "ConfigVariationBase.h"

#include "EventHandlerNotifyClientConnectionChange.h"
#include "EventHandlerWriteToPersistentLog.h"
#include "EventHandlerExternalAlertTrigger.h"
#include "EventHandlerFallbackReminder.h"
#include "EventHandlerConnectedSensorStateChange.h"
#include "EventHandlerDisconnectedStateChange.h"
#include "EventHandlerConnectedStateChange.h"

ConfigVariationBase::ConfigVariationBase()
{
}

int ConfigVariationBase::getEventHandlerCount()
{
	return 5;
}

void ConfigVariationBase::registerEventHandlers(IEventHandlerManager & handlerManager, CalibratedSensor & carEngineState)
{
	// event handlers are allocated once on init and never released

	// makes a sound on seat state changes, which are aggregated states, if also connected to client
	//EventHandlerConnectedStateChange * ehConnectedStateChange=new EventHandlerConnectedStateChange();
	// makes a sound on sensor changes, which are lower level events, if also connected to client
	EventHandlerConnectedSensorStateChange * ehConnectedStateChange = new EventHandlerConnectedSensorStateChange();
	handlerManager.addEventHandler(ehConnectedStateChange);

	EventHandlerDisconnectedStateChange * ehDisconnectedStateChange = new EventHandlerDisconnectedStateChange();
	handlerManager.addEventHandler(ehDisconnectedStateChange);

	// doesn't work for cars that turn the engine off automatically during stops
	EventHandlerFallbackReminder * ehFallbackReminder = new EventHandlerFallbackReminder(carEngineState);
	handlerManager.addEventHandler(ehFallbackReminder);

	// example of an external alert trigger. turn on-board led (13) as an indication of alert. if using, make sure there is no conflict with other uses of pin 13 (alive LED, SPI for SD card, etc).
	//EventHandlerExternalAlertTrigger * ehAlertLed=new EventHandlerExternalAlertTrigger(carEngineState, 10000, LED_BUILTIN);
	//ehAlertLed.begin();
	//handlerManager.addEventHandler(&ehAlertLed);

	// writes events to persistent log
	EventHandlerWriteToPersistentLog * ehPersistentLog = new EventHandlerWriteToPersistentLog();
	handlerManager.addEventHandler(ehPersistentLog);

	// makes a sound notification when an adapter to a client device connects or disconnects.
	EventHandlerNotifyClientConnectionChange * ehClientConnectionChange = new EventHandlerNotifyClientConnectionChange();
	handlerManager.addEventHandler(ehClientConnectionChange);

}
