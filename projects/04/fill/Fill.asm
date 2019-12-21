// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed. 
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.


// Intialize current state to all 0's mean white
@curr   
M = 0

(LOOP)
    // no need to do anything if curr is black and key pressed
    @KBD
    D = M
    @curr
    D = D & M
    @LOOP
    D; JNE

    // no need to do anything if curr is white and key not pressed
    @KBD
    D = M
    @curr
    D = D | M
    @LOOP
    D; JEQ

    // If pressed a key, goto BLACKEN, else goto WHITEN
    @KBD
    D = M
    @BLACKEN
    D; JNE
    @WHITEN
    D; JEQ


(BLACKEN)
    // Loop from screen memory map start until keyboard start
    @SCREEN
    D = A
    @screenaddr     // screenaddr stores base address of screen
    M = D
    @i
    M = 0

    (BLACKSCREENLOOP)
        @screenaddr
        D = M
        @i
        A = D + M

        M = -1      // blacken the 16 pixels
        
        @i
        M = M + 1
        D = D + M

        @KBD        // continue if (screenaddr + i) - KBD < 0
        D = D - A
        @BLACKSCREENLOOP
        D; JLT

    @curr
    M = -1
    @LOOP
    0; JMP


(WHITEN)
    // Loop from screen memory map start until keyboard start
    @SCREEN
    D = A
    @screenaddr     // screenaddr stores base address of screen
    M = D
    @i
    M = 0
    
    (WHITESCREENLOOP)
        @screenaddr
        D = M
        @i
        A = D + M

        M = 0      // blacken the 16 pixels
        
        @i
        M = M + 1
        D = D + M

        @KBD        // continue if (screenaddr + i) - KBD < 0
        D = D - A
        @WHITESCREENLOOP
        D; JLT

    @curr
    M = 0
    @LOOP
    0; JMP