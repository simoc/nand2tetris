// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

@R2
M=0 // R2=0
@R1
D=M
@R5
M=D // R5=R1
(LOOP)
@R5
D=M
@END
D;JLE // if R5<=0 GOTO END
@R0
D=M
@R2
M=M+D // R2=R2+R0
@R5
M=M-1 // R5=R5-1
@LOOP
0;JMP // GOTO LOOP
(END)
