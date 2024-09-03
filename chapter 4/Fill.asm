// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

    @R0
    M=0     // index = 0

(LOOP)      // while(1) {
(COLOR)
    @R2
    M=0     // color = 0
    @KBD
    D=M
    @DRAW
    D;JEQ
    @R2
    M=-1    // if KBD then color = -1

(DRAW)
    @R0
    D=M
    @SCREEN
    D=D+A   // offset = index + SCREEN
    @R1
    M=D
    @R2
    D=M
    @R1
    A=M
    M=D     // MEMORY[offset] = color
    @R0
    D=M
    MD=D+1  // ++index

(CLIP)
    @R1
    D=M
    @KBD
    D=D-A
    @LOOP
    D;JNE
    @R0
    M=0     // if offset == KBD { index = 0 }
    @LOOP
    0;JMP   // }