#include <Arduino.h>
///////////////////////////////////////////////////////////////////////////////
//	  #########################################################################
//	  ###### DON'T FORGET TO UPDATE THE User_Setup.h FILE IN THE LIBRARY ######
//	  #########################################################################
//
///////////////////////////////////////////////////////////////////////////////

#include <SPI.h>

#include <WiFi.h>
#include <iostream>
#include <sstream>
#include <string>

#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

void SaveBaseLocation(std::string newBaseLocation);
void LoadBaseLocation();

#include "Global.h"
#include "HandyLog.h"
#include "HandyString.h"
#include "GpsParser.h"
#include "NTRIPServer.h"
#include "MyFiles.h"
#include <WebPortal.h>
#include "LedStateTask.h"

WiFiManager _wifiManager;

unsigned long _loopWaitTime = 0;		 // Time of last second
int _loopPersSecondCount = 0;			 // Number of times the main loops runs in a second
unsigned long _lastButtonPress = 0;		 // Time of last button press to turn off display on T-Display-S3
unsigned long _recheckWifiAliveTime = 0; // Time we last checked for wifi alive

WebPortal _webPortal;

uint8_t _button1Current = HIGH; // Top button on left
uint8_t _button2Current = HIGH; // Bottom button when

MyFiles _myFiles;
GpsParser _gpsParser;
NTRIPServer _ntripServer0(0);
NTRIPServer _ntripServer1(1);
NTRIPServer _ntripServer2(2);
std::string _baseLocation = "";

// WiFi monitoring states
wl_status_t _lastWifiStatus = wl_status_t::WL_NO_SHIELD;

bool IsButtonReleased(uint8_t button, uint8_t *pCurrent);
bool IsWifiConnected();
String MakeHostName();
LedStateTask _ledState;

///////////////////////////////////////////////////////////////////////////////
// Setup
void setup(void)
{

	perror("RTL Server - Starting");
	perror(APP_VERSION);

	// No logging before here
	Serial.begin(115200); // Using perror() instead

	SetupLog(); // Call this before any logging

	_ledState.Set(2);

	Logf("Starting %s", APP_VERSION);

	// Setup the serial buffer for the GPS port
	Logf("GPS Buffer size %d", Serial1.setRxBufferSize(GPS_BUFFER_SIZE));

	Logln("Enable RS232 pins");
	Serial1.begin(115200, SERIAL_8N1, 16, 17);

	Logln("Enable WIFI");
	WiFi.mode(WIFI_AP_STA);

	Logln("Enable Buttons");
	// pinMode(BUTTON_1, INPUT_PULLUP);
	// pinMode(BUTTON_2, INPUT_PULLUP);

	// Verify file IO (This can take up tpo 60s is SPIFFs not initialised)
	Logln("Setup SPIFFS");
	Logln("This can take up to 60 seconds ...");
	if (_myFiles.Setup())
		Logln("Test file IO");
	else
		Logln("E100 - File IO failed");
	LoadBaseLocation();

	_ledState.Set(3);

	// Load the NTRIP server settings
	Logln("Setup NTRIP Connections");
	_ntripServer0.LoadSettings();
	_ntripServer1.LoadSettings();
	_ntripServer2.LoadSettings();

	//	Logf("Display type %d", USER_SETUP_ID);

	// Reset Wifi Setup if needed (Do tis to clear out old wifi credentials)
	//_wifiManager.erase();

	// Setup host name to have RTK_ prefix
	WiFi.setHostname(MakeHostName().c_str());

	// Block here till we have WiFi credentials (good or bad)
	Logf("Start listening on '%s'", MakeHostName().c_str());

	// Try at ever increasing timeouts
	const unsigned long WIFI_CONNECT_TIMEOUTS[] = {15, 30, 30, 300};
	const int WIFI_CONNECT_TIMEOUTS_SIZE = sizeof(WIFI_CONNECT_TIMEOUTS) / sizeof(WIFI_CONNECT_TIMEOUTS[0]);
	int _connectIndex = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		if (_connectIndex >= WIFI_CONNECT_TIMEOUTS_SIZE)
			_connectIndex = WIFI_CONNECT_TIMEOUTS_SIZE - 1;
		_wifiManager.setConfigPortalTimeout(WIFI_CONNECT_TIMEOUTS[_connectIndex]);
		Logf("\tTry WIFI Connection %ds", WIFI_CONNECT_TIMEOUTS[_connectIndex]);
		_wifiManager.autoConnect(WiFi.getHostname(), AP_PASSWORD);
		_connectIndex++;
	}

	_ledState.Set(4);

	// Connected - we have a valid IP address
	Logf("================ Connected IP : %s ================", WiFi.localIP().toString().c_str());

	// Start the GPS system
	_gpsParser.Setup(&_ntripServer0, &_ntripServer1, &_ntripServer2);

	// Connected
	_webPortal.Setup();
	Logln("Setup complete");

	_ledState.Set(5);
}

///////////////////////////////////////////////////////////////////////////////
// Loop here
void loop()
{
	// Trigger something every second
	int t = millis();
	_loopPersSecondCount++;
	if ((t - _loopWaitTime) > 10000)
	{
		auto free = ESP.getFreeHeap();
		auto total = ESP.getHeapSize();
	
		// Update the loop performance counter
		Serial.printf("Loop %d G:%ld 1:%ld, 2:%ld 3:%ld Heap:%d%%\n",
					  _loopPersSecondCount,
					  _gpsParser.GetGpsBytesRec(),
					  _ntripServer0.GetPacketsSent(),
					  _ntripServer1.GetPacketsSent(),
					  _ntripServer2.GetPacketsSent(),
					  (int)(100.0 * free / total));
		_loopWaitTime = t;
		_loopPersSecondCount = 0;
	}

	// Check for push buttons
	if (IsButtonReleased(BUTTON_1, &_button1Current))
	{
		_lastButtonPress = t;
		Logln("Button 1");
		// Reset WIFI connection
		_wifiManager.disconnect();
		//			_wifiManager.autoconnect(WiFi.getHostname(), AP_PASSWORD);
	}
	//	if (IsButtonReleased(BUTTON_2, &_button2Current))
	//	{
	//		_lastButtonPress = t;
	//		Logln("Button 2");
	//	}

	// Check for new data GPS serial data
	if (IsWifiConnected())
	{
		// Check if we has active WIFI connections
		if ((t - _recheckWifiAliveTime) > 10000)
		{
			_recheckWifiAliveTime = t;
			// Check the NTRIP servers are alive
			if (!_gpsParser.HasGpsExpired(t) &&
				_ntripServer0.HasConnectionExpired(t) &&
				_ntripServer1.HasConnectionExpired(t) &&
				_ntripServer2.HasConnectionExpired(t))
			{
				Logln("E905 - All NTRIP servers expired (Suspect WIFI outage)");
				WiFi.disconnect(true, false);
				//_wifiManager.disconnect();
			}
		}

		// Process the GPS data
		if (_gpsParser.ReadDataFromSerial(Serial1))
		{
			// Record we have a connected serial post
		}
		_webPortal.Loop();
	}
}

//////////////////////////////////////////////////////////////////////////////
// Read the base location from the disk
void LoadBaseLocation()
{
	std::string text;
	if (_myFiles.ReadFile(BASE_LOCATION_FILENAME, text))
	{
		Logln(StringPrintf(" - Read config '%s'", text.c_str()).c_str());
		_baseLocation = text;
	}
	else
	{
		_baseLocation = "";
		Logln(StringPrintf(" - E742 - Cannot read saved Server setting '%s'", BASE_LOCATION_FILENAME).c_str());
	}
}
void SaveBaseLocation(std::string newBaseLocation)
{
	_baseLocation = newBaseLocation;
	_myFiles.WriteFile(BASE_LOCATION_FILENAME, newBaseLocation.c_str());
}

///////////////////////////////////////////////////////////////////////////////
// Check if the button is released. This takes < 1ms
// Note : Button is HIGH when released
bool IsButtonReleased(uint8_t button, uint8_t *pCurrent)
{
	if (*pCurrent != digitalRead(button))
	{
		*pCurrent = digitalRead(button);
		return *pCurrent == HIGH;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// Check Wifi and reconnect
bool IsWifiConnected()
{
	// Is the WIFI connected?
	wl_status_t status = WiFi.status();
	if (_lastWifiStatus != status)
	{
		Logf("Wifi Status %d %s", status, WifiStatus(status));

		if (status == WL_CONNECTED)
		{
			// Setup the access point to prevend device getting stuck on a nearby network
			_wifiManager.setConfigPortalTimeout(0);
			auto res = _wifiManager.startConfigPortal(WiFi.getHostname(), AP_PASSWORD);
			if (!res)
				Logln("Failed to start config Portal (Maybe cos non-blocked)");
			else
				Logln("Config portal started");
		}
	}
	_lastWifiStatus = status;

	if (status == WL_CONNECTED)
		return true;

	// Start the WIFI connection process
	// .. This will block till we have a connection or timeouts
	Logln("E310 - Try resetting WIfi");
	_ledState.Set(4);
	WiFi.mode(WIFI_AP_STA);
	_wifiManager.setConfigPortalTimeout(30);
	_wifiManager.autoConnect(WiFi.getHostname(), AP_PASSWORD);
	if (WiFi.status() == WL_CONNECTED)
	{
		Logln("E311 - Reconnected to WiFi");
		_ledState.Set(5);
		_ntripServer0.RestartConnectionAttempts();
		_ntripServer1.RestartConnectionAttempts();
		_ntripServer2.RestartConnectionAttempts();

		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
/// @brief Maker a unique host name based on the MAC address with Rtk prefix
String MakeHostName()
{
	auto mac = WiFi.macAddress();
	mac.replace(":", "");
	return "Rtk_" + mac;
}
