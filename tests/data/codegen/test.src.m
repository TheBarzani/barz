% Code Section
          entry
          % Set up stack frame pointer
          addi r14,r0,topaddr
          % Load integer literal: 69
          addi r1,r0,69
          % Assignment to x
          % Load integer literal: 70
          addi r1,r0,70
          % Assignment to y
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

