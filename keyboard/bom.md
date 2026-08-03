# DELTA Redux keyboard - BOM

Remember to use a socket for all the ICs! Or, at the very least, the MCU!

| Component              | Qty | Type / Value                                            | Notes                                                                |
| ---------------------- | --- | ------------------------------------------------------- | -------------------------------------------------------------------- |
| C1, C2, C4, C5, C6     |  5  | 100nF/25V+ MLCC capacitor, 5mm pitch                    |                                                                      |
| C3                     |  1  | 680nF/25V+ MLCC capacitor, 5mm pitch                    |                                                                      |
| C7, C9                 |  2  | 470pF/25V+ MLCC capacitor, 5mm pitch                    |                                                                      |
| C8                     |  1  | 4.7uF/25V+ electrolytic capacitor, 2.5mm pitch          |                                                                      |
| C10                    |  1  | 10uF/25V+ MLCC capacitor, 5mm pitch                     |                                                                      |
| D1, D3                 |  2  | THT LED, 5mm diameter                                   | Ideally, use different colours for the two LEDs                      |
| D2                     |  1  | 1N4148 DO-35                                            |                                                                      |
| D4                     |  1  | LED SMD 0603 footprint                                  | Optional, if you want a LED under the caps-lock key                  |
| F1                     |  1  | Resettable fuse, 16V/~500mA                             | E.g. Bourns MF-RHT070                                                |
| J1                     |  1  | PinHeader, 16P 2x08, 2.54mm pitch, right angle          |                                                                      |
| J2                     |  1  | PinHeader, 1P 2.54mm pitch, vertical                    | Unused, do not populate                                              |
| J3                     |  1  | PinHeader, 16P 2x07, 2.54mm pitch, right angle          |                                                                      |
| JP1                    |  1  | PinHeader, 3P 1x03, 2.54mm pitch, right angle           |                                                                      |
| JP2                    |  1  | PinHeader, 3P 1x02, 2.54mm pitch, vertical              | Unused, do not populate                                              |
| R1, R2, R9             |  3  | 1Kohm resistor, 1/4W                                    |                                                                      |
| R3                     |  1  | 470ohm resistor, 1/4W                                   |                                                                      |
| R4, R7                 |  2  | 100ohm resistor, 1/4W                                   |                                                                      |
| R8                     |  1  | 100ohm resistor, 1/4W                                   | Unused, do not populate                                              |
| R5                     |  1  | 220Kohm resistor, 1/4W                                  |                                                                      |
| R6                     |  1  | 30Kohm resistor, 1/4W                                   |                                                                      |
| R10                    |  1  | 100Kohm resistor, 1/4W                                  |                                                                      |
| R11                    |  1  | 4.7Kohm resistor, 1/4W                                  |                                                                      |
| R12                    |  1  | 680ohm resistor, 1/4W                                   | Optional, install if you populate D4.                                |
| R13                    |  1  | 150ohm resistor, 1/4W                                   |                                                                      |
| RV1                    |  1  | ~500K Bourns 3386F vertical potentiometer               | https://www.mouser.it/ProductDetail/652-3386F-1-504TLF               |
| SW1 - SW65             | 65  | CherryMX or compatible keyboard switches                |                                                                      |
| SW66 - SW70            |  5  | 6mm THT push-button                                     |                                                                      |
| R13                    |  1  | 150ohm resistor, 1/4W                                   |                                                                      |
| U1					 |	1  | ATmega644P DIP40 MCU                                    | Needs to be flashed with the firmware you can find in this repo.     |
| U2					 |	1  | 74LS74 DIP-14                                           | An 'HCT works fine here, probably an HC too.                         |
| U3					 |	1  | 555 timer DIP-8                                         | NE555P tester                                                        |
| U4					 |	1  | 74LS14 DIP-14                                           | An 'HCT works fine here                                              |

You will need keycaps: I used [these](https://www.amazon.it/dp/B0CKPJCBVW) for my build.

Also, you can find the labels in [here](labels/key_labels.svg). If you design new ones, and wish them added to the repository, let me know!
