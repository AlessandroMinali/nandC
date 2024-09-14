// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

    @R2
    AMD=0   // r2 = 0
(LOOP)
    @R1
    DM=M-1  // --r1
    @END
    D;JLT   // while r1 >= 0

    @R0
    D=M
    @R2
    M=D+M   // r2 += r0
    @LOOP
    0;JMP
(END)
    @END
    0;JMP