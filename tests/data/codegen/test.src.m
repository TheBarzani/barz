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
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

