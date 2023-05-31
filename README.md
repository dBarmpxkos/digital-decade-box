# Digital Decade Box - v2.0.0r31052023
This repo hosts source files for an ESP32-based digital decade box.

### Serial communication example
The monitor listens at `9600-8n1`.

For adding a specific resistor to the total resistance, the following command should be issued:

`x,1\r\n`

where `x` is the resistor number (1-8).

Example transaction for activating resistors 3 and 7:

`3,1`

`[>] Resistor 3 | state: [ON]`

`[?] 00000100`

`7,1`

`[>] Resistor 7 | state: [ON]`

`[?] 01000100`

After each command the system responds with the current enabled resistors in a binary form, e.g.
`01000100` means the 7th and 3rd resistors are activated.

For removing a specific resistor from the total resistance, the following command should be issued:

`x,0\r\n`

where `x` , again, is the resistor number (1-8).
