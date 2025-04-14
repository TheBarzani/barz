% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 6999
          addi r1,r0,6999
          sw -16(r14),r1
          lw r1,-16(r14)
          % Processing dot access expression
          % Processing array access
          subi r2,r14,12
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t2 := 1
          addi r4,r0,1
          sw -20(r14),r4
          lw r4,-20(r14)
          muli r5,r4,4
          add r3,r3,r5
          % Array byte offset calculated
          sub r5,r2,r3
          sw -24(r14),r5
          % Array access address stored in -24(r14)
          % Warning: Using direct offset metadata
          lw r5,-24(r14)
          subi r2,r5,0
          sw -28(r14),r2
          % Member 'a' address stored at offset -28
          lw r2,-28(r14)
          % Storing value into class member
          sw 0(r2),r1
          % Assignment completed
          % Processing dot access expression
          % Processing array access
          subi r1,r14,12
          % Calculating array byte offset
          addi r2,r0,0
          % processing: t5 := 1
          addi r5,r0,1
          sw -32(r14),r5
          lw r5,-32(r14)
          muli r3,r5,4
          add r2,r2,r3
          % Array byte offset calculated
          sub r3,r1,r2
          sw -36(r14),r3
          % Array access address stored in -36(r14)
          % Warning: Using direct offset metadata
          lw r3,-36(r14)
          subi r1,r3,0
          sw -40(r14),r1
          % Member 'a' address stored at offset -40
          lw r2,-40(r14)
          lw r1,0(r2)
          % processing: write(t7)
          % put value on stack
          addi r14,r14,-40
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
          subi r14,r14,-40
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

