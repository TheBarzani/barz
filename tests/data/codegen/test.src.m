% Code Section

          % Processing function: func1
          % Entering function scope: func1
func1
          % Function prologue (func1)
          sw -4(r14),r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Exiting function scope: func1

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 69
          addi r1,r0,69
          sw -12(r14),r1
          % processing:: x := t1
          lw r2,-12(r14)
          sw -4(r14),r2
          % processing: t2 := 70
          addi r2,r0,70
          sw -16(r14),r2
          % processing:: y := t2
          lw r3,-16(r14)
          sw -8(r14),r3
          % processing: t3 := 1
          addi r3,r0,1
          sw -20(r14),r3
          % processing: t4 := x + t3
          lw r4,-4(r14)
          lw r5,-20(r14)
          add r6,r4,r5
          sw -24(r14),r6
          % Exiting function scope: main
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

