// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// program clears the screen, i.e. writes "white" in every pixel.

//TODO: investigate why screen flickers between black and white while a key is held down.

@keyPressed
M=0 // keyPressed=0 -- initialiase current keyboard state to no key pressed
@lastKeyPressed
M=0 // lastKeyPressed=0 -- initialise previous keyboard state to no key pressed

(MAIN_LOOP)

@keyPressed
M=0

@KBD
D=M

@NOT_PRESSED
D;JEQ
@keyPressed
M=1 // keyPressed=1 if key pressed, 0 if no key pressed
(NOT_PRESSED) // D=1 if key pressed, D=0 if no key pressed

@keyPressed
D=M
@lastKeyPressed
D=D-M
@MAIN_LOOP
D;JEQ // if (keyPressed==lastKeyPressed) goto MAIN_LOOP -- no change in keyboard state

@pixelColour
M=0 // pixelColour=0 -- bytes for white pixels when no key pressed

@keyPressed
D=M
@FROM_PRESSED_TO_NOT_PRESSED
D;JEQ

@30000
D=A
@30000
D=D+A
@5535
D=D+A
@pixelColour
M=D // pixelColour=65535 -- bytes for black pixels when key pressed

(FROM_PRESSED_TO_NOT_PRESSED)


@i
M=0 // i=0
@SCREEN
D=A
@screenMemory
M=D // screenMemory=@SCREEN
(PAINT_LOOP)
@pixelColour
D=M


@screenMemory
A=M
M=D // RAM[screenMemory]=pixelColour
@screenMemory
M=M+1 // screenMemory=screenMemory+1
@i
M=M+1 // i=i+1

@i
D=M
@8192 // 256*512 = 131072 pixels or 8192 16-bit values
D=D-A
@PAINT_LOOP
D;JLT // if (i-8192)<0 goto PAINT_LOOP

@keyPressed
D=M
@lastKeyPressed
M=D // lastKeyPressed=keyPressed -- save new previous keyboard state

@MAIN_LOOP
0;JMP
