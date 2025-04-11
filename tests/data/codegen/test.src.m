% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 2
          addi r1,r0,2
          sw -68(r14),r1
          lw r1,-68(r14)
          % Processing array access
          % processing: t3 := 2
          addi r2,r0,2
          sw -76(r14),r2
          lw r2,-76(r14)
          % Processing array access
          % processing: t2 := 1
          addi r4,r0,1
          sw -72(r14),r4
          lw r4,-72(r14)
          multi r5,r4,-64
          addi r6,r0,4
          mul r7,r4,r6
          add r8,r5,r7
          sw -76(r14),r8
          lw r3,-76(r14)
          addi r8,r0,4
          mul r7,r2,r8
          add r6,r3,r7
          sw -76(r14),r6
          lw r6,-76(r14)
          % Storing value into array element
          sw 0(r6),r1
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

