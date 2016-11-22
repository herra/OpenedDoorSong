#Arduino IDE sketch for Esp8266 01

Simple schema with esp8266 01. Esp detects opened door and plays jingle bells with buzzer.

#WARNING
##This circuit could damage the buzzer!
There are several examples with arduino that recommend adding a capacitor.
http://blog.giacomocerquone.com/jingle-bell-with-an-arduino-and-a-buzzer/
##Power consumption is not measured! 

#Interesting 
##Sleep Mode
The sketch uses Light-sleep mode of Esp8266. More details for Esp8266 sleep modes can be found here:
http://www.espressif.com/sites/default/files/9b-esp8266-low_power_solutions_en_0.pdf

I made this working light-sleep solution by combining several sources from internet.
https://github.com/esp8266/Arduino/issues/1488

http://www.esp8266.com/wiki/doku.php?id=esp8266_power_usage

http://www.esp8266.com/viewtopic.php?f=32&t=2305

https://tzapu.com/minimalist-battery-powered-esp8266-wifi-temperature-logger/


#Schema:
https://circuits.io/circuits/3289511-play-jingle-bells-on-button-opened-door/embed#breadboard
or
https://circuits.io/circuits/3289511-play-jingle-bells-on-button-opened-door/embed#schematic
