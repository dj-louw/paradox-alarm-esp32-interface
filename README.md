# paradox-alarm-esp32-interface
A hardware and software project that allows you to connect your Paradox Magellan alarm system to the rest of the world.

## Description
This project makes use of an ESP32 module to act as a wifi to serial bridge between the Paradox Magellan control panel and an instance of [Paradox Alarm Interface](https://github.com/ParadoxAlarmInterface/pai).

PAI is then used to publish states via MQTT, and it also allows the user to control the panel via MQTT. This opens up the option to link the panel to smart honme systems such as HomeAssistant and so on.

## Hardware
![alarm interface with esp32-Wiring](https://user-images.githubusercontent.com/1496180/136033597-a7340014-3fc4-4476-8f46-6e8df0f41923.png)

## Software
Have a look in the src folder. In there is a pretty simple Arduino Sketch that creates a serial to telnet bridge that you can connect to via Telnet.
