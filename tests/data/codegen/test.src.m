% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 9
          addi r1,r0,9
          sw -112(r14),r1
          lw r1,-112(r14)
          % Processing dot access expression
          % Processing dot access expression
          addi r2,r14,-108
          subi r3,r2,0
          sw -116(r14),r3
          % Member 'b' address stored at offset -116
          addi r3,r14,-116
          subi r2,r3,0
          sw -120(r14),r2
          % Member 'c' address stored at offset -120
          lw r2,-120(r14)
          % Storing value into class member
          sw 0(r2),r1
          % Assignment completed
          % Processing dot access expression
          % Processing dot access expression
          addi r1,r14,-108
          subi r2,r1,0
          sw -124(r14),r2
          % Member 'b' address stored at offset -124
          addi r2,r14,-124
          subi r1,r2,0
          sw -128(r14),r1
          % Member 'c' address stored at offset -128
          lw r3,-128(r14)
          lw r1,0(r3)
          % processing: write(t5)
          % put value on stack
          addi r14,r14,-128
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
          subi r14,r14,-128
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

