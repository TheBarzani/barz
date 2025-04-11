% Code Section

          % Processing function: func1
          % Entering function scope: func1
func1
          % Function prologue (func1)
          sw -4(r14),r15
          % processing: write(a)
          lw r1,-12(r14)
          % put value on stack
          addi r14,r14,-20
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
          subi r14,r14,-20
          % processing: t1 := a + b
          lw r2,-12(r14)
          lw r1,-16(r14)
          add r3,r2,r1
          sw -20(r14),r3
          lw r1,-20(r14)
          sw -8(r14),r1
          add r13,r1,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % Function call: func1
          % Pushing parameters onto stack
          % processing: t3 := 3
          addi r1,r0,3
          sw -16(r14),r1
          lw r1,-16(r14)
          sw -60(r14),r1
          % processing: t2 := 2
          addi r1,r0,2
          sw -12(r14),r1
          lw r1,-12(r14)
          sw -56(r14),r1
          % Setting up activation record for function call
          addi r14,r14,-44
          jl r15,func1
          % Function func1 returned, restoring stack
          subi r14,r14,-44
          % Storing return value from r13
          sw -20(r14),r13
          % processing:: ret := t4
          lw r1,-20(r14)
          sw -8(r14),r1
          % processing: write(ret)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-44
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
          subi r14,r14,-44
          % processing: t5 := 1
          addi r2,r0,1
          sw -36(r14),r2
          % processing:: x := t5
          lw r2,-36(r14)
          sw -24(r14),r2
          % processing: write(x)
          lw r2,-24(r14)
          % put value on stack
          addi r14,r14,-44
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
          subi r14,r14,-44
          % processing: t6 := 2
          addi r1,r0,2
          sw -40(r14),r1
          % processing:: y := t6
          lw r1,-40(r14)
          sw -28(r14),r1
          % processing: write(y)
          lw r1,-28(r14)
          % put value on stack
          addi r14,r14,-44
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
          subi r14,r14,-44
          % processing: t7 := 3
          addi r2,r0,3
          sw -44(r14),r2
          % processing:: z := t7
          lw r2,-44(r14)
          sw -32(r14),r2
          % processing: write(z)
          lw r2,-32(r14)
          % put value on stack
          addi r14,r14,-44
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
          subi r14,r14,-44
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

