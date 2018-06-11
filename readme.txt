Arduino-based APM meter for Melee which does not cause any delays or dropped inputs. Because I wanted to know how fast I was.
Still has issues with ports other than port 1. Gamecube will stop recognizing the controller after a second or so for unknown reasons.

Hardware setup: Savagely destroy a Gamecube controller extension wire, splice the data line to connect it to pin 13 and connect the 5v and GND wires (any GND works) to VCC and GND on the Arduino. Leave 3.3v and the other GND untouched. Add LEDs if desired (you want them, trust me). Upload sketch. Enjoy.

Better instructions coming very soon.
TODO: Output live stats to some sort of display. Flash LEDs on each action for extra style.

I'm using a slightly modified version of the Nintendo library by NicoHood (https://github.com/NicoHood/Nintendo) which is available here : https://github.com/jmontineri/Nintendo The modifications are necessary for this to work. The license information for the library can be found on its page.

(c) Copyright Jonathan Montineri, 2018. This project (and only this project, not including the modified Nintendo library) is licensed under the terms of the GPLv3 license.
