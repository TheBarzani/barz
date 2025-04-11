% Code Section

          % Processing function: func1
          % Entering function scope: func1
func1
          % Function prologue (func1)
          sw -4(r14),r15
          % processing: t1 := 111
          addi r1,r0,111
          sw -8(r14),r1
          % processing:: a := t1
          lw r1,-8(r14)
          sw -4(r14),r1
          lw r1,-4(r14)
          add r13,r1,r0
          % Return to caller
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Exiting function scope: func1

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % Function call: func1
          % Pushing parameters onto stack
          % Setting up activation record for function call
          addi r14,r14,0
          jl r15,func1
          % Function func1 returned, restoring stack
          subi r14,r14,0
          % Storing return value from r13
          sw -24(r14),r13
          % processing:: ret := t2
          lw r1,-24(r14)
          sw -16(r14),r1
          % processing: write(ret)
          lw r1,-16(r14)
          % put value on stack
          addi r14,r14,-64
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
          subi r14,r14,-64
          % processing: t3 := 3
          addi r2,r0,3
          sw -28(r14),r2
          % processing:: x := t3
          lw r2,-28(r14)
          sw -4(r14),r2
          % processing: t4 := 2
          addi r2,r0,2
          sw -32(r14),r2
          % processing:: y := t4
          lw r2,-32(r14)
          sw -8(r14),r2
          % processing: t5 := 0
          addi r2,r0,0
          sw -36(r14),r2
          % processing: t6 := x or t5
          lw r2,-4(r14)
          lw r1,-36(r14)
          bnz r2,logical_or_true_0
          bnz r1,logical_or_true_0
          addi r3,r0,0
          j logical_or_end_0
logical_or_true_0
          addi r3,r0,1
logical_or_end_0
          sw -40(r14),r3
          % processing:: z := t6
          lw r1,-40(r14)
          sw -12(r14),r1
          % if statement - evaluating condition
          lw r1,-4(r14)
          lw r2,-8(r14)
          clt r4,r1,r2
          sw -44(r14),r4
          add r2,r4,r0
          bz r2,else_2
          % then branch
then_1
          j endif_3
          % else branch
else_2
endif_3
          % end if
          % processing: t8 := 0
          addi r2,r0,0
          sw -48(r14),r2
          % processing:: i := t8
          lw r2,-48(r14)
          sw -44(r14),r2
while_start_4
          % while loop - evaluating condition
          % processing: t9 := 10
          addi r2,r0,10
          sw -52(r14),r2
          lw r2,-44(r14)
          lw r1,-52(r14)
          cle r5,r2,r1
          sw -56(r14),r5
          add r1,r5,r0
          bz r1,while_end_5
          % while loop - executing body
          % processing: t11 := 1
          addi r1,r0,1
          sw -60(r14),r1
          % processing: t12 := i + t11
          lw r1,-44(r14)
          lw r2,-60(r14)
          add r6,r1,r2
          sw -64(r14),r6
          % processing:: i := t12
          lw r2,-64(r14)
          sw -44(r14),r2
          j while_start_4
while_end_5
          % end while loop
          % Exiting function scope: main
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

