#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Global.h"
#include "HandyString.h"

extern WiFiManager _wifiManager;

///////////////////////////////////////////////////////////////////////////
// Class create a task to update the display showing a count down during
// .. wifi connection. This allows the autoconnect to have a time out
// .. and still see how much we have to go
// NOTE : This is a bit risky as the display is not thread safe. Only
//		  use if display is static
class LedStateTask
{
private:
	int _state = 1;
	TaskHandle_t _connectingTask = NULL;
	SemaphoreHandle_t _xMutex = NULL;

public:
	/////////////////////////////////////////////////////////////////////////////
	// Constructor
	// .. Create task
	// .. Start task
	LedStateTask()
	{
		// Setup LED and turn it on
		pinMode(LED_BUILTIN, OUTPUT);
		digitalWrite(LED_BUILTIN, HIGH);

		// Setup the mutex for the task
		_xMutex = xSemaphoreCreateMutex();
		xTaskCreatePinnedToCore(
			TaskWrapper,
			"WifiBusyCountDown", // Task name
			5000,				 // Stack size (bytes)
			this,				 // Parameter
			1,					 // Task priority
			&_connectingTask,	 // Task handle
			APP_CPU_NUM);
	}

	///////////////////////////////////////////////////////////////////////////
	// Destructor
	// .. Delete the task
	// .. Turn on the LED
	~LedStateTask()
	{
		Serial.println("----- LedStateTask Terminating");
		digitalWrite(LED_BUILTIN, HIGH);
		vTaskDelete(_connectingTask);
	}

	static void TaskWrapper(void *param)
	{
		// Cast the parameter back to MyClass and call the member function
		LedStateTask *instance = static_cast<LedStateTask *>(param);
		instance->TaskFunction();
	}

	///////////////////////////////////////////////////////////////////////////
	// Loop forever displaying count down on screen
	void TaskFunction()
	{
		Serial.println("+++++ LedStateTask Starting");
		while (true)
		{
			//xSemaphoreTake(_xMutex, portMAX_DELAY);
			//_state--;
			//xSemaphoreGive(_xMutex);

			const int quick = 200;
			// Blink required number of times
			for( int n = 0; n < _state; n++)
			{
					//Serial.println("+++++ LedStateTask Connected");
					digitalWrite(LED_BUILTIN, HIGH);
					vTaskDelay(quick / portTICK_PERIOD_MS);
					digitalWrite(LED_BUILTIN, LOW);
					vTaskDelay(quick / portTICK_PERIOD_MS);
			}

			//			auto message = StringPrintf("%s -> Retry %ds", APP_VERSION, _wifiConnectingCountDown);
			//			auto message = StringPrintf("%s -> Retry %ds", _wifiManager.getWiFiSSID(), _wifiConnectingCountDown);
			//auto message = StringPrintf("[R:%d] %s", _wifiConnectingCountDown, WifiStatus(WiFi.status()));

			//Serial.println(message.c_str());
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Called to start the countdown
	void Set(int state)
	{
		xSemaphoreTake(_xMutex, portMAX_DELAY);
		_state = state;
		xSemaphoreGive(_xMutex);
	}
};
