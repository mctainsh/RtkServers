# UM982 RTK Server with WEMOS LOLIN S2 or S3 Mini and And some other ESP32 S3 (Multi miner)

This project connects a UM980 or UM982 RTK GNSS receiver to a WEMOS LOLIN S3 Mini allowing you to share RTK correction data with up to three networks at one time (Some give mining rewards). The ESP32 will automatically program the UM980/2 so there is no need to mess around with terminals or or the UPrecise software.

All up you it will cost about US$200 to make the station with GNSS receiver, antenna and ESP32 (Prices may have varied since I wrote this). 

If you want to send to more than three casters you can connect a second ESP32 in parallel to the TX port of the UM98x and power both ESP32's at the some time. No need for a second UM98x or expensive antennas or splitters.

There is no display just a box with blinking leds like the HAL 2000

NOTE : Although the S2 cpu is able to send data to three RTK casters, if one of the casters fails to receive the message (blocks) the other will be delayed. This is because the S2 only has a single CPU core. I highly recommend using an S3

```
This project is for people who already have the S2 or S3 chip. If you are buying a ESP32 for this project, I recommend the T-Display-S3 as it is about US$15 more but will save you of time and look nicer attracting a more desirable mate.
```
<div style="display: flex; gap:10px;">
<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/AssemblyFront.jpg?raw=true" Width=200/><br/>
<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/AssemblyBack.jpg?raw=true" Width=200/><br/>
</div>



## Table of Contents 
 
- [Project Overview](#project-overview)
- [Hardware](#hardware)  
  - [Components](#components)
  - [Wiring Diagram](#wiring-diagram) 
- [Software](#software)  
  - [Features](#features)
  - [Key Mappings](#key-mappings) 
  - [Config parameter](#config-parameter)
  - [Development Environment](#development-environment)
- [License](#license)

## Project Overview

This project enables a LOLIN ESP32 S3 Mini to act as an RTK server sending RTK corrections to up to three casters. Examples of these are be Onocoy, Rtk2Go or RtkDirect.

### Terms

| Name | Description |
| --- | --- |
| RTK Client | A device or software that receives RTK correction data from a server to improve positioning accuracy. |
| RTK Server | A server that processes and distributes RTK correction data to clients. (This project builds a RTK Server) |
| RTK Caster | A service that broadcasts RTK correction data over the internet using the NTRIP protocol. |

## Hardware 

### Components 
 
1. **UM982 with antenna** - Witte Intelligent WTRTK-982 high-precision positioning and orientation module. I got it from AliExpress for about A$220.00 [Not affiliate link. Find your own seller](https://www.aliexpress.com/item/1005007287184287.html)
 
2. **ESP32 S2 or S3 Mini**  

	<table>
		<tr>
			<td>
				<b>Wemos LOLIN S3 Min</b> - <i>(Preferred)</i> ESP32 S3. AliExpress for US$10.00.<br/> [Not an affiliate link shop with care](https://vi.aliexpress.com/item/1005005449219195.html)
			</td>
			<td>
				<img src="https://www.wemos.cc/en/latest/_static/boards/s3_mini_v1.0.0_1_16x16.jpg" Height='70px'/>
			</td>
		</tr>
		<tr>
			<td>
				<b>Wemos LOLIN S2 Mini</b> - Slow single threaded. AliExpress for US$5.00. <br/>[Not an affiliate link shop with care](https://vi.aliexpress.com/item/1005006818259492.html)
			</td>
			<td>
				<img src="https://www.wemos.cc/en/latest/_images/s2_mini_v1.0.0_1_16x16.jpg" Height='70px' />
			</td>
		</tr>
	</table>
		
 
3. **Wires and Protoboard** - Connects the ESP32 to receiver as described below.

4. **Fan** - Used a 5V fan cos the UM982 get hot in the sun. Not always necessary.
 

## Wiring Diagram

<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/Wiring.png?raw=true"/><br/>

### LOLIN S3 Mini
| LOLIN Mini Pin | Use | UM982 pin | Use |
| --- | --- | --- | --- |
| 5V | 5V| 2 | 5V |
| G | GND | 5 | GND |
| NC | |  |  |
| NC | |  |  |
| 17 | TX | 3 | RX |
| 16 | RX | 4 | TX |


## Software 

### Features 

- Connects to UM982.
 
- Connects to Wifi.

- Programs the UM982 to send generate RTK correction data

- Sends correction data to all RTK Casters

### ESP32 device setup

Depending on the device you will need to upload the binary

 - [Upload Binary for LOLIN S3 Mini](https://github.com/mctainsh/Esp32/blob/main/RtkServers/README_Upload_S3_Mini.md)

 - OR

 - [Upload Binary for LOLIN S2 Mini](https://github.com/mctainsh/Esp32/blob/main/RtkServers/README_Upload_S2_Mini.md)

### Connect WIFI

- Connect ESP32 to your HOME WiFi
	- Power up the ESP32 (GPS does not need to be connected)
	- Using your phone or computer
	- Connect to the WIFI access point Rtk_XXXXXXXX (Mac address shown on ESP32 screen)
	- Password will be `John123456`
	- Browse to http://192.168.4.1
	- Select "Configure WiFi"
	- Select your HOME WiFi network and enter credential
	- Device will restart
- Connect your phone or computer to you HOME WiFi 
	- You should be able to browse to the WiFi address if your device shown in the console during the programming process. If it is not there, Check your router for new devices

### Config parameters 

Config parameters are set in the "Configure Wifi" web page

Note : You don't need to sign up to all three. Leave the CASTER ADDRESS blank to only use one or two casters. 

| Parameter | Usage | 
| --- | --- | 
| SSID | Your WiFi network name. Note: Not all speed are supported by ESP32 |
| Password | Your Wifi password |
| CASTER 1 ADDRESS | Usually "ntrip.rtkdirect.com" |
| CASTER 1 PORT | Port usually 2101 |
| CASTER 1 CREDENTIAL | This is the reference station Credential. NOT the Mount point name |
| CASTER 1 PASSWORD | Sign up to get this from Onocoy |
| CASTER 2 ADDRESS | Usually "servers.onocoy.com" |
| CASTER 2 PORT | Port usually 2101 |
| CASTER 2 CREDENTIAL | This is the reference station Credential. NOT the Mount point name |
| CASTER 2 PASSWORD | Sign up to get this from Onocoy |
| CASTER_3 ADDRESS | Usually "rtk2go.com" |
| CASTER 3 PORT | Port usually 2101 |
| CASTER 3 CREDENTIAL | Mount point name |
| CASTER 3 PASSWORD | Create this with Rtk2Go signup |

WARNING :  Do not run without real credentials or your IP may be blocked!!

### Configuration 

1. Create the accounts with [Oncony register](https://console.onocoy.com/auth/register/personal), [RtkDirect](https://cloud.rtkdirect.com/) or [RTK2GO](http://rtk2go.com/sample-page/new-reservation/)

2. Don't wire up anything to start with (We can let the smoke out of it later)

3. Upload the program to your ESP32. 

4. Power it up and check display for WIFI connection status.

5. Following instruction at [WifiManager](https://github.com/tzapu/WiFiManager) to connect your ESP32 to your WIFI.

6. Configure the RTK Servers you want to use in the "Configure Wifi" Screen.

7. Wire up the ESP32 to UM98x. Power it fom UM98x (Sometime the ESP32 doesn't output enough beans).

8. Review the status and logs through the web interface (http://x.x.x.x/i)


### Development Environment

1. **Install VS Code** : Follow [Instructions](https://code.visualstudio.com/docs/setup/setup-overview)

2. **Install the PlatformIO IDE** : Download and install the [PlatformIO](https://platformio.org/install).
 
3. **Clone This Repository**

```bash
git clone https://github.com/mctainsh/RtkServers.git
```

or just copy the files from
```
https://github.com/mctainsh/RtkServers
```

### Assembled

This is the CPU, Receiver and Fan assembled on a custom PCB 

<img src="https://github.com/mctainsh/RtkServers/blob/main/Photos/AssembledPCB.jpg?raw=true"/><br/>

## License 
This project is licensed under the GNU General Public License - see the [LICENSE](https://github.com/mctainsh/Esp32/blob/main/LICENSE)  file for details.

---

