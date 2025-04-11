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
          % processing: t1 := 3
          addi r1,r0,3
          sw -16(r14),r1
          % processing:: x := t1
          lw r1,-16(r14)
          sw -4(r14),r1
          % processing: t2 := 2
          addi r1,r0,2
          sw -20(r14),r1
          % processing:: y := t2
          lw r1,-20(r14)
          sw -8(r14),r1
          % processing: t3 := x * y
          lw r1,-4(r14)
          lw r2,-8(r14)
          mul r3,r1,r2
          sw -24(r14),r3
          % processing:: z := t3
          lw r2,-24(r14)
          sw -12(r14),r2
          % if statement - evaluating condition
          % processing: t4 := 1
          addi r2,r0,1
          sw -28(r14),r2
          % processing: t5 := y + t4
          lw r2,-8(r14)
          lw r1,-28(r14)
          add r4,r2,r1
          sw -32(r14),r4
          lw r1,-4(r14)
          lw r2,-32(r14)
          cne r5,r1,r2
          sw -36(r14),r5
          add r2,r5,r0
          bz r2,else1
          % then branch
then0
          % processing: t7 := x - y
          lw r2,-4(r14)
          lw r1,-8(r14)
          sub r6,r2,r1
          sw -40(r14),r6
          % processing: write(t7)
          lw r1,-40(r14)
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
          j endif2
          % else branch
else1
          % processing: write(y)
          lw r2,-8(r14)
          % put value on stack
          addi r14,r14,-40
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
          subi r14,r14,-40
          % processing: write(x)
          lw r1,-4(r14)
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
endif2
          % end if
          % processing: write(z)
          lw r2,-12(r14)
          % put value on stack
          addi r14,r14,-40
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
          subi r14,r14,-40
          % Exiting function scope: main
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

