% Code Section

          % Processing function: hi
          % Entering function scope: hi
hi
          % Function prologue (hi)
          sw -4(r14),r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % Processing array access
          addi r1,r14,-128
          % Calculating array byte offset
          addi r2,r0,0
          lw r3,-4(r14)
          addi r4,r0,4
          mul r5,r3,r4
          add r2,r2,r5
          sw -172(r14),r2
          lw r2,-172(r14)
          sub r5,r1,r2
          sw -172(r14),r5
          lw r5,-172(r14)
          % processing:: c2 := 
          sw -168(r14),r5
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

