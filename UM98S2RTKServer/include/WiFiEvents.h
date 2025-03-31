#pragma once

#include <WiFi.h>
#include "HandyLog.h"

////////////////////////////////////////////////////////////////////////////////
// Setup the wifi events
void SetupWiFiEvents()
{
	// Setup the wifi events
	WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) 
	{
		switch (event)
		{
		case SYSTEM_EVENT_STA_START:
			Logln("WIFI - Start");
			break;
		case SYSTEM_EVENT_STA_STOP:
			Logln("WIFI - Stop");
			break;
		case SYSTEM_EVENT_STA_CONNECTED:
			Logln("WIFI - Connected");
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			Logln("WIFI - Disconnected");
			break;
		case SYSTEM_EVENT_STA_GOT_IP:
			Logln("WIFI - Got IP");
			break;
		default:
			Logf("WIFI - %d", event);
			break;
		}
	});
	
}