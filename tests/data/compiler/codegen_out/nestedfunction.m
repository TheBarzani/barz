% Code Section

          % Processing function: plus
          % Entering function scope: plus
plus
          % Function prologue (plus)
          sw -4(r14),r15
          % processing: write(a)
          lw r1,-12(r14)
          % put value on stack
          addi r14,r14,-20
          sw -8(r14),r1
          % link buffer to stack
          addi r1,r0,buf
          sw -12(r14),r1
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-20
          % processing: t1 := a + b
          lw r2,-12(r14)
          lw r1,-16(r14)
          add r3,r2,r1
          sw -20(r14),r3
          lw r3,-20(r14)
          sw 0(r14),r3
          add r13,r3,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: double
          % Entering function scope: double
double
          % Function prologue (double)
          sw -4(r14),r15
          % processing: write(a)
          lw r3,-12(r14)
          % put value on stack
          addi r14,r14,-20
          sw -8(r14),r3
          % link buffer to stack
          addi r3,r0,buf
          sw -12(r14),r3
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-20
          % Function call: plus
          % Pushing parameters onto stack
          lw r1,-12(r14)
          sw -36(r14),r1
          lw r1,-12(r14)
          sw -32(r14),r1
          % Setting up activation record for function call
          addi r14,r14,-20
          jl r15,plus
          % Function returned, restoring stack
          subi r14,r14,-20
          lw r1,-20(r14)
          % processing:: x := t2
          sw -16(r14),r1
          % Assignment completed
          % processing: write(x)
          lw r1,-16(r14)
          % put value on stack
          addi r14,r14,-20
          sw -8(r14),r1
          % link buffer to stack
          addi r1,r0,buf
          sw -12(r14),r1
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-20
          lw r3,-16(r14)
          sw 0(r14),r3
          add r13,r3,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % Function call: double
          % Pushing parameters onto stack
          % processing: t3 := 11
          addi r3,r0,11
          sw -12(r14),r3
          lw r3,-12(r14)
          sw -28(r14),r3
          % Setting up activation record for function call
          addi r14,r14,-16
          jl r15,double
          % Function returned, restoring stack
          subi r14,r14,-16
          lw r3,-16(r14)
          % processing:: x := t4
          sw -8(r14),r3
          % Assignment completed
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

