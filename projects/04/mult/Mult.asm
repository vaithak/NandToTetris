// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)


// Initialize num1=R1
@R1
D = M
@num1
M = D

// Initialize bitchecker=1
@bitchecker
M = 1

// Initialize R2=0
@R2
M = 0

(LOOP)
    // End loop if bitchecker > R0
    @R0
    D = M
    @bitchecker
    D = D - M       // D = R0-bitchecker
    @END
    D; JLT

    // Add num1 to R2 if (bitchecker & R0 != 0)
    @R0
    D = M
    @bitchecker
    D = D & M
    @ADD
    D; JNE 

    (FINISHCHECK)

    // num1 = num1*2
    @num1
    D = M
    M = D + M

    // bitchecker = bitchecker*2
    @bitchecker
    D = M
    M = D + M

    // iterate again
    @LOOP
    0; JMP

(ADD)
    @num1
    D = M
    @R2
    M = D + M

    @FINISHCHECK
    0; JMP

(END)
    @END
    0; JMP