# PCB 
We designed the schematic and pcb design for a MSP430FR2311IPW16R to run independently on a pcb connected with headers on a breadboard.

## PCB Schematic
For the schematic, bypass and bulk capacitors were added between VSS and VCC.
The 100nF bypass capacitor will send low level AC noise to ground.
The 10uF bulk capacitor will regulate the voltage provided to the processor so that when it dips, the capacitor will discharge and level the voltage.
Reset circuitry was also added to this schematic. The reset circuitry consists of a 1nf capacitor going to ground to ensure debouncing and a 47k resistor used as a pull up resistor. A TSSOP_20 was chosen to model the msp since it has 10 pins on each side. 

## PCB Design
The board was modeled as 2 sided with an internal ground plane and has the dimensions of .8 inches by 1.45 inches in height. 
All traces were modeled at 13 mil thickness which is within PCBWAY limitations.
The difference in space betwen the headers on each side of the board is 0.6 inches so that the board will fit on a breadboard across the center line.
The pattern for the TSSOP_20 was compared to the pattern at the end of the MSP430FR2311 datasheet to ensure the IC would fit. 

## Bill of Materials
RefDes	Value	Name				Part	Manufacturer	Quantity
C4		100nF	CAP_0805			Part 1						1
C5		10uF	CAP_0805			Part 1						1
C6		1nF		CAP_0805			Part 1						1
J3				1-644456-0			Part 1	TE Connectivity		1
J4				1-644456-0			Part 1	TE Connectivity		1
R2		47k		RES_0805			Part 1						1
U2				MSP4302311IPW16R	Part 1						1
