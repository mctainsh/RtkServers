#pragma once

#include "LedStateTask.h"

#define APP_VERSION "3.208"

// Disables logging to serial
#define SERIAL_LOG

#define BUTTON_1 0

#define MAX_LOG_LENGTH (200)
#define MAX_LOG_SIZE (MAX_LOG_LENGTH * 80)
#define MAX_LOG_ROW_LENGTH 127

#define TEMP_HISTORY_SIZE (24*60) // 1 day of history at 60 second intervals

#define RTK_SERVERS 3

#define GPS_BUFFER_SIZE (16*1024)

// One 1 buttons on right, 3 Buttons on left
#define TFT_ROTATION  3

// WiFi access point password
#define AP_PASSWORD "John123456"
#define BASE_LOCATION_FILENAME "/BaseLocn.txt"

extern LedStateTask _ledState;
