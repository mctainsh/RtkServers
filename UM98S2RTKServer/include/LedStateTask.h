#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Global.h"
#include "HandyString.h"

#define LED1 13
#define LED2 11
#define LED3 9
#define LED4 7
#define LED5 3
#define LED6 1
#define LED_COUNT 6

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

	const int LEDS[LED_COUNT] = {LED1, LED2, LED3, LED4, LED5, LED6};
	unsigned long _ledOnTime[LED_COUNT] = {0, 0, 0, 0, 0, 0};
	uint8_t _ledState[LED_COUNT] = {LOW, LOW, LOW, LOW, LOW, LOW};

public:
	/////////////////////////////////////////////////////////////////////////////
	// Constructor
	// .. Create task
	// .. Start task
	LedStateTask()
	{
		// Setup LED and turn it on
		pinMode(LED_BUILTIN, OUTPUT);
		pinMode(LED1, OUTPUT);
		pinMode(LED2, OUTPUT);
		pinMode(LED3, OUTPUT);
		pinMode(LED4, OUTPUT);
		pinMode(LED5, OUTPUT);
		pinMode(LED6, OUTPUT);

		digitalWrite(LED_BUILTIN, HIGH);
		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, HIGH);
		digitalWrite(LED3, HIGH);
		digitalWrite(LED4, HIGH);
		digitalWrite(LED5, HIGH);
		digitalWrite(LED6, HIGH);

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
		int loopNo = 0;
		while (true)
		{
			// Don't work this thread too hard
			vTaskDelay(50 / portTICK_PERIOD_MS);

			loopNo++;
			digitalWrite(LED_BUILTIN, ( loopNo / 20 ) % 2);
			//if (loopNo % 10 = 0)
			//{
			//	loopNo = 0;
				// Serial.println("+++++ LedStateTask Looping");
			//}
			// const int blinkDuration = 200; // Blink time in ms

			// // Blink required number of times
			// for (int n = 0; n < _state; n++)
			// {
			// 	// Serial.println("+++++ LedStateTask Connected");
			// 	digitalWrite(LED_BUILTIN, HIGH);
			// 	vTaskDelay(blinkDuration / portTICK_PERIOD_MS);
			// 	digitalWrite(LED_BUILTIN, LOW);
			// 	vTaskDelay(blinkDuration / portTICK_PERIOD_MS);
			// }

			for (int n = 0; n < LED_COUNT; n++)
			{
				if (_ledOnTime[n] == 0)
					continue;
				auto age = millis() - _ledOnTime[n];
				if (age < 100)
					continue;
				if (_ledState[n] != LOW)
				{
					digitalWrite(LEDS[n], LOW);
					_ledState[n] = LOW;
				}
				if (age < 500)
					continue;
				_ledOnTime[n] = 0;
			}
		}
	}

	///////////////////////////////////////////////////////////////////////////
	// Called to start the countdown
	void Set(int state)
	{
		_state = state;
		if (0 <= state && state < LED_COUNT)
			digitalWrite(LEDS[state], LOW);
	}

	void BlinkGps()
	{
		Blink(2);
	}
	void BlinkWifi()
	{
		Blink(1);
	}
	void BlinkNtrip(int index)
	{
		Blink(3 + index);
	}

	///////////////////////////////////////////////////////////////////////////////
	// Blink turn on the led for 100ms then does not allow it to be touched for 500ms
	// .. This is to prevent the LED from being turned on and off too fast
	void Blink(int led)
	{
		if (led < 0 || led >= LED_COUNT)
		{
			Logf("E100 - LED %d out of range", led);
			return;
		}

		// Already on - don't do anything
		if (_ledOnTime[led] != 0)
			return;

		// auto lastOn = millis() - _ledOnTime[led];
		// if (lastOn < 500)
		//	return;
		digitalWrite(LEDS[led], HIGH);
		_ledState[led] = HIGH;
		_ledOnTime[led] = millis();
	}
};
