Arduino-based APM meter for Melee which does not cause any delays or dropped inputs. Because I wanted to know how fast I was.
Still has issues with ports other than port 1. Gamecube will stop recognizing the controller after a second or so for unknown reasons.

Hardware setup: Savagely destroy a Gamecube controller extension wire, splice the data line to connect it to pin 13 and connect the 5v and GND wires (any GND works) to VCC and GND on the Arduino. Leave 3.3v and the other GND untouched. Add LEDs if desired (you want them, trust me). Upload sketch. Enjoy.

Better instructions coming very soon.
TODO: Output live stats to some sort of display. Flash LEDs on each action for extra style.

I'm using a slightly modified version of the Nintendo library by NicoHood (https://github.com/NicoHood/Nintendo) which will be uploaded shortly. See the submodule (when I upload it) for license information.
It was modified to support reading both directions without writing to the controller or console, since I designed this to be as easy to build as possible (read - no hardware aside from an Arduino board and a Gamecube controller extension cord, and optionally some LEDs and resistors) and I don't think either the Gamecube or the controller would appreciate 5v in their data lines.

(c) Copyright Jonathan Montineri, 2018. This project is licensed under the terms of the GPLv3 license.
