% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 1
          addi r1,r0,1
          sw -20(r14),r1
          % processing: t2 := 2
          addi r1,r0,2
          sw -24(r14),r1
          % processing: t3 := 3
          addi r1,r0,3
          sw -28(r14),r1
          lw r1,-24(r14)
          lw r2,-28(r14)
          % processing: t4 := t2 * t3
          mul r3,r1,r2
          sw -32(r14),r3
          % processing: t5 := t1 + t4
          lw r3,-20(r14)
          lw r2,-32(r14)
          add r1,r3,r2
          sw -36(r14),r1
          lw r1,-36(r14)
          % processing:: y := t5
          sw -12(r14),r1
          % Assignment completed
          % Processing: read(x)
          addi r14,r14,-84
          % Allocate buffer space for input
          addi r1,r0,buf
          sw -8(r14),r1
          % Read string from console
          jl r15,getstr
          % Convert string to integer
          addi r1,r0,buf
          sw -8(r14),r1
          jl r15,strint
          subi r14,r14,-84
          % Store read value into r13
          sw -8(r14),r13
          % processing: write(x)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-84
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
          subi r14,r14,-84
          % if statement - evaluating condition
          % processing: t6 := 10
          addi r2,r0,10
          sw -40(r14),r2
          % processing: t7 := y + t6
          lw r2,-12(r14)
          lw r1,-40(r14)
          add r3,r2,r1
          sw -44(r14),r3
          lw r3,-8(r14)
          lw r1,-44(r14)
          cgt r2,r3,r1
          sw -48(r14),r2
          add r2,r2,r0
          bz r2,else_1
          % then branch
then_0
          % processing: t9 := 10
          addi r2,r0,10
          sw -52(r14),r2
          % processing: t10 := x + t9
          lw r2,-8(r14)
          lw r1,-52(r14)
          add r3,r2,r1
          sw -56(r14),r3
          % processing: write(t10)
          lw r3,-56(r14)
          % put value on stack
          addi r14,r14,-84
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
          subi r14,r14,-84
          j endif_2
          % else branch
else_1
          % processing: t11 := 1
          addi r1,r0,1
          sw -60(r14),r1
          % processing: t12 := x + t11
          lw r1,-8(r14)
          lw r3,-60(r14)
          add r2,r1,r3
          sw -64(r14),r2
          % processing: write(t12)
          lw r2,-64(r14)
          % put value on stack
          addi r14,r14,-84
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
          subi r14,r14,-84
endif_2
          % end if
          % processing: t13 := 0
          addi r3,r0,0
          sw -68(r14),r3
          lw r3,-68(r14)
          % processing:: z := t13
          sw -16(r14),r3
          % Assignment completed
while_start_3
          % while loop - evaluating condition
          % processing: t14 := 10
          addi r3,r0,10
          sw -72(r14),r3
          lw r3,-16(r14)
          lw r2,-72(r14)
          cle r1,r3,r2
          sw -76(r14),r1
          add r1,r1,r0
          bz r1,while_end_4
          % while loop - executing body
          % processing: write(z)
          lw r1,-16(r14)
          % put value on stack
          addi r14,r14,-84
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
          subi r14,r14,-84
          % processing: t16 := 1
          addi r2,r0,1
          sw -80(r14),r2
          % processing: t17 := z + t16
          lw r2,-16(r14)
          lw r1,-80(r14)
          add r3,r2,r1
          sw -84(r14),r3
          lw r3,-84(r14)
          % processing:: z := t17
          sw -16(r14),r3
          % Assignment completed
          j while_start_3
while_end_4
          % end while loop
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

