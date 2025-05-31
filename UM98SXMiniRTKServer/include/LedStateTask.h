#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "Global.h"
#include "HandyString.h"

// S3 Mini
#ifdef S3_MINI
#define LED1 9
#define LED2 11
#define LED3 13
#define LED4 12
#define LED5 2
#define LED6 1
#endif

// S3 Zero
#ifdef S3_ZERO
#include <Adafruit_NeoPixel.h>
#define LED_PIN 21
#define PIXEL_BRIGHTNESS 127
#endif

// S2 Mini
#ifdef S2_MINI
#define LED1 13
#define LED2 11
#define LED3 9
#define LED4 7
#define LED5 3
#define LED6 1
#endif

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
	bool _mainLoopRunning = false;
#ifndef S3_ZERO
	const int LEDS[LED_COUNT] = {LED1, LED2, LED3, LED4, LED5, LED6};
	unsigned long _ledOnTime[LED_COUNT] = {0, 0, 0, 0, 0, 0};
#endif
	uint8_t _ledState[LED_COUNT] = {LOW, LOW, LOW, LOW, LOW, LOW};

public:
	inline void StartMainLoopTask() { _mainLoopRunning = true; }

	/////////////////////////////////////////////////////////////////////////////
	// Setup the task and start it
	// .. Create task
	// .. Start task
	void Setup()
	{
#ifdef S3_ZERO

#else
		// Setup LED and turn it on
		pinMode(LED_BUILTIN, OUTPUT);
		pinMode(LED1, OUTPUT);
		pinMode(LED2, OUTPUT);
		pinMode(LED3, OUTPUT);
		pinMode(LED4, OUTPUT);
		pinMode(LED5, OUTPUT);
		pinMode(LED6, OUTPUT);

		// digitalWrite(LED_BUILTIN, HIGH);	// Fuck knows why this line of code kills the CPU if call before Setup()
		digitalWrite(LED1, HIGH);
		digitalWrite(LED2, HIGH);
		digitalWrite(LED3, HIGH);
		digitalWrite(LED4, HIGH);
		digitalWrite(LED5, HIGH);
		digitalWrite(LED6, HIGH);
#endif

		// Setup the task
		xTaskCreatePinnedToCore(
			TaskWrapper,
			"LedFlasher",	  // Task name
			5000,			  // Stack size (bytes)
			this,			  // Parameter
			25,				  // Task priority
			&_connectingTask, // Task handle
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

#ifdef S3_ZERO
		Adafruit_NeoPixel pixels{2, LED_PIN, NEO_RGB + NEO_KHZ800};
		pixels.begin();
		pixels.setBrightness(PIXEL_BRIGHTNESS);
		pixels.show(); // Initialize all pixels to 'off'
#endif

		Serial.println("+++++ LedStateTask Starting");
		int loopNo = 0;
		while (true)
		{

#ifdef S3_ZERO

			// If not running main loop wait till we are ready with state colour
			if (!_mainLoopRunning)
			{
				vTaskDelay(50 / portTICK_PERIOD_MS);
				pixels.setPixelColor(0, GetStateColor(pixels, _state));
				pixels.show();
				continue;
			}

			// Blink the led states
			for (int n = 0; n < LED_COUNT; n++)
			{
				// ON if active, OFF if not
				if (_ledState[n] != LOW || n == 0)
				{
					pixels.setPixelColor(0, GetStateColor(pixels, n));
					pixels.show();
				}
				vTaskDelay(250 / portTICK_PERIOD_MS);

				pixels.setPixelColor(0, pixels.Color(0, 0, 0));
				pixels.show();
				vTaskDelay(250 / portTICK_PERIOD_MS);

				_ledState[n] = LOW;
			}

			// Long delay before next blink set
			pixels.setPixelColor(0, pixels.Color(0, 0, 1));
			pixels.show();
			vTaskDelay(1000 / portTICK_PERIOD_MS);

#else
			// Blink the build in LED the number of times matching the startup state
			loopNo++;
			auto divider = loopNo / 4;
			auto blinkCount = divider % 18;
			if (blinkCount > _state * 2)
				digitalWrite(LED_BUILTIN, LOW);
			else
				digitalWrite(LED_BUILTIN, divider % 2);

			// Turn the additional LEDs off after they have been on for 100ms
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
#endif
		}
	}

#ifdef S3_ZERO

	uint32_t GetStateColor(Adafruit_NeoPixel &pixels, int state)
	{
		switch (state)
		{
		case 1:
			return pixels.Color(0, 255, 0); // Green
		case 2:
			return pixels.Color(0, 0, 255); // Blue
		case 3:
			return pixels.Color(255, 255, 0); // Yellow
		case 4:
			return pixels.Color(255, 0, 255); // Magenta
		case 5:
			return pixels.Color(0, 255, 255); // Cyan
		case 6:
			return pixels.Color(255, 128, 0); // Orange
		case 7:
			return pixels.Color(128, 0, 255); // Purple
		case 8:
			return pixels.Color(255, 0, 0); // Red
		}
		return pixels.Color(255, 255, 255); // White
	}

	///////////////////////////////////////////////////////////////////////////
	// Called to start the countdown
	void Set(int state)
	{
		_state = state;
		On(state);
		// delay(2000);
	}

	///////////////////////////////////////////////////////////////////////////
	// Turn on the LED permanently
	void On(int state)
	{
		//	pixels.setPixelColor(0, GetStateColor(state));
		//	pixels.show();
	}
#else
	///////////////////////////////////////////////////////////////////////////
	// Called to start the countdown
	void Set(int state)
	{
		_state = state;
		if (0 <= state && state < LED_COUNT)
			digitalWrite(LEDS[state], LOW);
	}

	///////////////////////////////////////////////////////////////////////////
	// Turn on the LED permanently
	void On(int state)
	{
		if (0 <= state && state < LED_COUNT)
			digitalWrite(LEDS[state], HIGH);
	}
#endif

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
#ifdef S3_ZERO
		// pixels.setPixelColor(0, pixels.Color(255, 0, 0));
		// pixels.show();
#else

		// Already on - don't do anything
		if (_ledOnTime[led] != 0)
			return;

		// auto lastOn = millis() - _ledOnTime[led];
		// if (lastOn < 500)
		//	return;
		digitalWrite(LEDS[led], HIGH);
		_ledOnTime[led] = millis();
#endif
		_ledState[led] = HIGH;
	}
};
