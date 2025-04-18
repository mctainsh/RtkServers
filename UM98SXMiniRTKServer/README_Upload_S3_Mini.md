## Upload the binary to the WEMO S3 Mini

```
IMPORTANT : This applies to ESP32-S3 only. Not other devices like ESP32 or ESP32-S2
```

You can also use https://espressif.github.io/esp-launchpad/ which does not require anything to be downloaded to your computer.


- Download the files from [One Drive](https://1drv.ms/f/s!Avrf6GYUWqyFhtUYAQn6uUMdvsv8OQ?e=aEFKXA) in the S3_Mini folder

- Plug in your ESP32
- Browse to https://espressif.github.io/esp-launchpad/ 
- Use connect button (See Screen shoot 1)
- Select your ESP32 in the popup
- Select DIY
- ERASE your device (Only if you hare having problems)
  - Wait for erase to complete
  - Disconnect
  - Connect
Select the Addresses and files


	| File | Address | 
	| --- | --- | 
 	| bootloader.bin|0x0 | 
 	| partitions.bin|0x8000 | 
 	| boot_app0.bin |0xe000 |
 	| firmware.bin |0x10000|

	<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/LauncherFilesS3.png?raw=true" width="300"/>

- Select Program
- If the ESP32 LED does not start flashing
  - Select RESET
- ESP32 Will start double blinking
- Wait 90 seconds till it starts blinking 4 times per set
- In the console you should see the device WIFI host name starting with "RTK_........" (Screen shot 2)
- Later you will see the IP address of the device after you complete the WIFI login. (Screen shot 3)

```
NOTE : The first time you install it may take up to 90 seconds to startup after reset. Please be patient
```
<hr/>
<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/S3_Connect.png?raw=true" width="600"/><br/>
Screen Shot 1
<hr/>


<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/S3-Mac_Address.png?raw=true" width="600"/>
<br/>
Screen Shot 2
<hr/>
<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/S3_IP_Address.png?raw=true" width="600"/>
<br/>
Screen Shot 3
<hr/>