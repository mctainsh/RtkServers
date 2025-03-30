#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Global.h"
#include "HandyString.h"

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

		// Setup the task
		xTaskCreatePinnedToCore(
			TaskWrapper,
			"WifiBusyCountDown", // Task name
			5000,				 // Stack size (bytes)
			this,				 // Parameter
			25,					 // Task priority
			&_connectingTask,	 // Task handle
			APP_CPU_NUM);
	}

	///////////////////////////////////////////////////////////////////////////
	// Destructor
	// This should never happen
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
			const int blinkDuration = 200;	// Blink time in ms

			// Blink required number of times
			for( int n = 0; n < _state; n++)
			{
					//Serial.println("+++++ LedStateTask Connected");
					digitalWrite(LED_BUILTIN, HIGH);
					vTaskDelay(blinkDuration / portTICK_PERIOD_MS);
					digitalWrite(LED_BUILTIN, LOW);
					vTaskDelay(blinkDuration / portTICK_PERIOD_MS);
			}

			// Long delay to between blink sets
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Called to start the countdown
	void Set(int state)
	{
		_state = state;
	}
};
