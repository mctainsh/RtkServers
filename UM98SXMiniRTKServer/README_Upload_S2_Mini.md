## Upload the binary to the TTGO ESP32 S2 Mini

```
IMPORTANT : This applies to ESP32 only. Not other devices like ESP32-C2 or ESP32-S3
```

- Download the files from [One Drive](https://1drv.ms/f/s!Avrf6GYUWqyFhtUYAQn6uUMdvsv8OQ?e=aEFKXA). Pick the folder for S2_Mini
- Get the Espressif Flash Download Tools from https://www.espressif.com/en/support/download/other-tools
	- The tool is called Flash Download Tools
	- If the tool doesn't work out of the box watch https://www.youtube.com/watch?v=BVJlc4hio6A to get the correct drivers
- Launch flash_download_tool_3.9.7.exe (Version number may be a little different)
- Select ChipType: ESP32

	| File | Address | 
	| --- | --- | 
 	| bootloader.bin|0x1000 | 
 	| partitions.bin|0x8000 | 
 	| boot_app0.bin |0xe000 |
 	| firmware.bin |0x10000|

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display/Flash-ChipSelect.png?raw=true" width="200"/>
- Add each file to the list
- Be sure to add a tick next to each file and the correct address

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display/Flash-FileAssignment.png?raw=true" width="300"/>
- Double check your work
- Press `START`
- If all goes well it should look like

	<img src="https://github.com/mctainsh/Esp32/blob/main/UM98RTKServer/Photos/TTGO-Display/Flash-CompleteResult.png?raw=true" />

Power cycle the ESP32. It should blink 3 times every 5 seconds. It may start with only 1 or 2 blinks for the first 90 seconds.

```
NOTE : The first time you install it may take up to 90 seconds to startup after reset. Please be patient
```