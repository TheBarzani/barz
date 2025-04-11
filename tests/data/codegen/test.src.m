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
          sw -112(r14),r1
          % processing: t2 := 2
          addi r1,r0,2
          sw -12(r14),r1
          lw r1,-12(r14)
          sw -108(r14),r1
          % Setting up activation record for function call
          addi r14,r14,-96
          jl r15,func1
          % Function func1 returned, restoring stack
          subi r14,r14,-96
          % Storing return value from r13
          sw -20(r14),r13
          lw r1,-20(r14)
          % processing:: ret := t4
          sw -8(r14),r1
          % processing: write(ret)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          % processing: t5 := 1
          addi r2,r0,1
          sw -36(r14),r2
          lw r2,-36(r14)
          % processing:: x := t5
          sw -24(r14),r2
          % processing: write(x)
          lw r2,-24(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          % processing: t6 := 2
          addi r1,r0,2
          sw -40(r14),r1
          lw r1,-40(r14)
          % processing:: y := t6
          sw -28(r14),r1
          % processing: write(y)
          lw r1,-28(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          % processing: t7 := 3
          addi r2,r0,3
          sw -44(r14),r2
          lw r2,-44(r14)
          % processing:: z := t7
          sw -32(r14),r2
          % processing: write(z)
          lw r2,-32(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          % processing: t8 := 0
          addi r1,r0,0
          sw -52(r14),r1
          lw r1,-52(r14)
          % processing:: i := t8
          sw -48(r14),r1
while_start_0
          % while loop - evaluating condition
          % processing: t9 := 10
          addi r1,r0,10
          sw -56(r14),r1
          lw r1,-48(r14)
          lw r2,-56(r14)
          clt r4,r1,r2
          sw -60(r14),r4
          add r2,r4,r0
          bz r2,while_end_1
          % while loop - executing body
          % processing: write(i)
          lw r2,-48(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          % processing: t11 := 2
          addi r1,r0,2
          sw -64(r14),r1
          % processing: t12 := i + t11
          lw r1,-48(r14)
          lw r2,-64(r14)
          add r5,r1,r2
          sw -68(r14),r5
          lw r2,-68(r14)
          % processing:: i := t12
          sw -48(r14),r2
          j while_start_0
while_end_1
          % end while loop
          % processing: t13 := 0
          addi r2,r0,0
          sw -72(r14),r2
          lw r2,-72(r14)
          % processing:: i := t13
          sw -48(r14),r2
while_start_2
          % while loop - evaluating condition
          % processing: t14 := 10
          addi r2,r0,10
          sw -76(r14),r2
          lw r2,-48(r14)
          lw r1,-76(r14)
          clt r6,r2,r1
          sw -80(r14),r6
          add r1,r6,r0
          bz r1,while_end_3
          % while loop - executing body
          % processing: write(i)
          lw r1,-48(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          % if statement - evaluating condition
          % processing: t16 := 5
          addi r2,r0,5
          sw -84(r14),r2
          lw r2,-48(r14)
          lw r1,-84(r14)
          ceq r7,r2,r1
          sw -88(r14),r7
          add r1,r7,r0
          bz r1,else_5
          % then branch
then_4
          % processing: write(i)
          lw r1,-48(r14)
          % put value on stack
          addi r14,r14,-96
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
          subi r14,r14,-96
          j endif_6
          % else branch
else_5
endif_6
          % end if
          % processing: t18 := 1
          addi r2,r0,1
          sw -92(r14),r2
          % processing: t19 := i + t18
          lw r2,-48(r14)
          lw r1,-92(r14)
          add r8,r2,r1
          sw -96(r14),r8
          lw r1,-96(r14)
          % processing:: i := t19
          sw -48(r14),r1
          j while_start_2
while_end_3
          % end while loop
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

