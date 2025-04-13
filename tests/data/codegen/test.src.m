% Code Section

          % Processing function: plus
          % Entering function scope: plus
plus
          % Function prologue (plus)
          sw -4(r14),r15
          % processing: t1 := a + b
          lw r1,-12(r14)
          lw r2,-16(r14)
          add r3,r1,r2
          sw -20(r14),r3
          lw r2,-20(r14)
          sw 0(r14),r2
          add r13,r2,r0
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
          % Function call: plus
          % Found 2 parameters for function plus
          % Pushing parameters onto stack
          lw r2,-12(r14)
          sw -28(r14),r2
          lw r2,-12(r14)
          sw -32(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-16
          jl r15,plus
          % Function plus returned, restoring stack
          subi r14,r14,-16
          lw r2,-16(r14)
          sw 0(r14),r2
          add r13,r2,r0
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
          % Found 1 parameters for function double
          % Pushing parameters onto stack
          % processing: t3 := 11
          addi r2,r0,11
          sw -12(r14),r2
          lw r2,-12(r14)
          sw -36(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-24
          jl r15,double
          % Function double returned, restoring stack
          subi r14,r14,-24
          lw r2,-16(r14)
          % processing:: x := t4
          sw -8(r14),r2
          % Assignment completed
          % Function call: double
          % Found 1 parameters for function double
          % Pushing parameters onto stack
          % processing: t5 := 200
          addi r2,r0,200
          sw -20(r14),r2
          lw r2,-20(r14)
          sw -36(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-24
          jl r15,double
          % Function double returned, restoring stack
          subi r14,r14,-24
          % processing: write(t6)
          lw r2,-24(r14)
          % put value on stack
          addi r14,r14,-24
          sw -8(r14),r2
          % link buffer to stack
          addi r2,r0,buf
          sw -12(r14),r2
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-24
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

