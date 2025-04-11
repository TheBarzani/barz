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
          sw -20(r14),r1
          % processing:: x := t1
          lw r1,-20(r14)
          sw -4(r14),r1
          % processing: t2 := 2
          addi r1,r0,2
          sw -24(r14),r1
          % processing:: y := t2
          lw r1,-24(r14)
          sw -8(r14),r1
          % processing: t3 := 0
          addi r1,r0,0
          sw -28(r14),r1
          % processing: t4 := x or t3
          lw r1,-4(r14)
          lw r2,-28(r14)
          bnz r1,logical_or_true_0
          bnz r2,logical_or_true_0
          addi r3,r0,0
          j logical_or_end_0
logical_or_true_0
          addi r3,r0,1
logical_or_end_0
          sw -32(r14),r3
          % processing:: z := t4
          lw r2,-32(r14)
          sw -12(r14),r2
          % if statement - evaluating condition
          lw r2,-4(r14)
          lw r1,-8(r14)
          clt r4,r2,r1
          sw -36(r14),r4
          add r1,r4,r0
          bz r1,else_2
          % then branch
then_1
          % processing: t6 := x - y
          lw r1,-4(r14)
          lw r2,-8(r14)
          sub r5,r1,r2
          sw -40(r14),r5
          % processing: write(t6)
          lw r2,-40(r14)
          % put value on stack
          addi r14,r14,-60
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
          subi r14,r14,-60
          j endif_3
          % else branch
else_2
          % processing: write(y)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-60
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
          subi r14,r14,-60
          % processing: write(x)
          lw r2,-4(r14)
          % put value on stack
          addi r14,r14,-60
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
          subi r14,r14,-60
endif_3
          % end if
          % processing: t7 := 0
          addi r1,r0,0
          sw -44(r14),r1
          % processing:: i := t7
          lw r1,-44(r14)
          sw -40(r14),r1
while_start_4
          % while loop - evaluating condition
          % processing: t8 := 10
          addi r1,r0,10
          sw -48(r14),r1
          lw r1,-40(r14)
          lw r2,-48(r14)
          cle r6,r1,r2
          sw -52(r14),r6
          add r2,r6,r0
          bz r2,while_end_5
          % while loop - executing body
          % processing: write(i)
          lw r2,-40(r14)
          % put value on stack
          addi r14,r14,-60
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
          subi r14,r14,-60
          % processing: t10 := 1
          addi r1,r0,1
          sw -56(r14),r1
          % processing: t11 := i + t10
          lw r1,-40(r14)
          lw r2,-56(r14)
          add r7,r1,r2
          sw -60(r14),r7
          % processing:: i := t11
          lw r2,-60(r14)
          sw -40(r14),r2
          j while_start_4
while_end_5
          % end while loop
          % processing: write(z)
          lw r2,-12(r14)
          % put value on stack
          addi r14,r14,-60
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
          subi r14,r14,-60
          % Exiting function scope: main
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

