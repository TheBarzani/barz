% Code Section

          % Processing function: count
          % Entering function scope: count
count
          % Function prologue (count)
          sw -4(r14),r15
          % if statement - evaluating condition
          lw r1,-12(r14)
          lw r2,-16(r14)
          ceq r3,r1,r2
          sw -20(r14),r3
          add r2,r3,r0
          bz r2,else_1
          % then branch
then_0
          lw r2,-16(r14)
          sw 0(r14),r2
          add r13,r2,r0
          lw r15,-4(r14)
          jr r15
          j endif_2
          % else branch
else_1
          % processing: write(a)
          lw r2,-12(r14)
          % put value on stack
          addi r14,r14,-32
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
          subi r14,r14,-32
endif_2
          % end if
          % Function call: count
          % Found 2 parameters for function count
          % Pushing parameters onto stack
          lw r1,-16(r14)
          sw -48(r14),r1
          % processing: t2 := 1
          addi r1,r0,1
          sw -24(r14),r1
          % processing: t3 := a + t2
          lw r1,-12(r14)
          lw r2,-24(r14)
          add r4,r1,r2
          sw -28(r14),r4
          lw r2,-28(r14)
          sw -44(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-32
          jl r15,count
          % Function count returned, restoring stack
          subi r14,r14,-32
          lw r2,-32(r14)
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
          % Function call: count
          % Found 2 parameters for function count
          % Pushing parameters onto stack
          % processing: t6 := 10
          addi r2,r0,10
          sw -12(r14),r2
          lw r2,-12(r14)
          sw -32(r14),r2
          % processing: t5 := 0
          addi r2,r0,0
          sw -8(r14),r2
          lw r2,-8(r14)
          sw -28(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-16
          jl r15,count
          % Function count returned, restoring stack
          subi r14,r14,-16
          % processing: write(t7)
          lw r2,-16(r14)
          % put value on stack
          addi r14,r14,-16
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
          subi r14,r14,-16
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

