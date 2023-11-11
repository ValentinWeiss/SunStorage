# SunStorage

## Installation

### Prerequisites

Install CP210x Windows Drivers: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers?tab=downloads

### PlatformIO
- Install the PlatformIO extension in VS Code
- Open the SunStorage project in VS Code and open the PlatformIO tab in the side menu
- Wait until PlaformIO has installed all dependencies for the SunStorage project

## Using ESP-IDF

### Tasks
Create a task for each application. To do that you have to follow the following steps:

1. Declare your application main function (choose a speaking name, not "main")
2. Add a corresponding function call of ```xTaskCreate``` to the ```app_main``` function in the file ```main.c```. The pcName can only be an array up to 16 bytes and for the stack size use a number as small as possible.<br/>
(read the Documentation for detailed information: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/freertos_idf.html?highlight=xtaskcreate#_CPPv411xTaskCreate14TaskFunction_tPCKcK22configSTACK_DEPTH_TYPEPCv11UBaseType_tPC12TaskHandle_t)
2. Only create those tasks in ```app_main``` function that you want to run continuously, because the ```app_main``` function will return when all the tasks have been created. Therefore do not use the returned handle and do not create a task with parameters there!!!


## Compiling

Compile flags can be set in the platformio.ini file.

-DTX: compile code for ESP1 (TX)

-DRX: compile code for ESP2 (RX)

## Flashing

PlatformIO: -> Press Build -> Press Upload and Monitor -> While "Connecting..." press BOOT Button then EN Button on the ESP board. https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html#functional-description

## Ports
If you want to reserve a port add it to the Dokumentation\Architektur\Pinout.md file.
