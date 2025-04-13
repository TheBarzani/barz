% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 66
          addi r1,r0,66
          sw -36(r14),r1
          lw r1,-36(r14)
          % Processing dot access expression
          addi r2,r14,-32
          subi r3,r2,-24
          sw -40(r14),r3
          % Member 'd' address stored at offset -40
          lw r3,-40(r14)
          % Storing value into class member
          sw 0(r3),r1
          % Assignment completed
          % Processing dot access expression
          addi r1,r14,-32
          subi r3,r1,-24
          sw -44(r14),r3
          % Member 'd' address stored at offset -44
          lw r2,-44(r14)
          lw r3,0(r2)
          % processing: write(t3)
          % put value on stack
          addi r14,r14,-44
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
          subi r14,r14,-44
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

