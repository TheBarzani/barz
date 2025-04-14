% Code Section

          % Processing function: bubbleSort
          % Entering function scope: bubbleSort
bubbleSort
          % Function prologue (bubbleSort)
          sw -4(r14),r15
          lw r1,-12(r14)
          % processing:: n := 
          sw -16(r14),r1
          % Assignment completed
          % processing: t1 := 0
          addi r1,r0,0
          sw -32(r14),r1
          lw r1,-32(r14)
          % processing:: i := t1
          sw -20(r14),r1
          % Assignment completed
          % processing: t2 := 0
          addi r1,r0,0
          sw -36(r14),r1
          lw r1,-36(r14)
          % processing:: j := t2
          sw -24(r14),r1
          % Assignment completed
          % processing: t3 := 0
          addi r1,r0,0
          sw -40(r14),r1
          lw r1,-40(r14)
          % processing:: tem := t3
          sw -28(r14),r1
          % Assignment completed
while_start_0
          % while loop - evaluating condition
          % processing: t4 := 1
          addi r1,r0,1
          sw -44(r14),r1
          % processing: t5 := n - t4
          lw r1,-16(r14)
          lw r2,-44(r14)
          sub r3,r1,r2
          sw -48(r14),r3
          lw r3,-20(r14)
          lw r2,-48(r14)
          clt r1,r3,r2
          sw -52(r14),r1
          add r1,r1,r0
          bz r1,while_end_1
          % while loop - executing body
          % processing: t7 := 0
          addi r1,r0,0
          sw -56(r14),r1
          lw r1,-56(r14)
          % processing:: j := t7
          sw -24(r14),r1
          % Assignment completed
while_start_2
          % while loop - evaluating condition
          % processing: t8 := n - i
          lw r1,-16(r14)
          lw r2,-20(r14)
          sub r3,r1,r2
          sw -60(r14),r3
          % processing: t9 := 1
          addi r3,r0,1
          sw -64(r14),r3
          % processing: t10 := t8 - t9
          lw r3,-60(r14)
          lw r2,-64(r14)
          sub r1,r3,r2
          sw -68(r14),r1
          lw r1,-24(r14)
          lw r2,-68(r14)
          clt r3,r1,r2
          sw -72(r14),r3
          add r3,r3,r0
          bz r3,while_end_3
          % while loop - executing body
          % if statement - evaluating condition
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r3,-8(r14)
          % Calculating array byte offset
          addi r2,r0,0
          lw r1,-24(r14)
          muli r4,r1,4
          add r2,r2,r4
          % Array byte offset calculated
          sub r4,r3,r2
          sw -76(r14),r4
          % Array access address stored in -76(r14)
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r4,-8(r14)
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t13 := 1
          addi r2,r0,1
          sw -80(r14),r2
          % processing: t15 := j + t13
          lw r2,-24(r14)
          lw r1,-80(r14)
          add r5,r2,r1
          sw -88(r14),r5
          lw r5,-88(r14)
          muli r1,r5,4
          add r3,r3,r1
          % Array byte offset calculated
          sub r1,r4,r3
          sw -92(r14),r1
          % Array access address stored in -92(r14)
          % Warning: Using direct offset metadata
          % Warning: Using direct offset metadata
          lw r1,-76(r14)
          lw r1,0(r1)
          lw r4,-92(r14)
          lw r4,0(r4)
          cgt r3,r1,r4
          sw -96(r14),r3
          add r3,r3,r0
          bz r3,else_5
          % then branch
then_4
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r3,-8(r14)
          % Calculating array byte offset
          addi r4,r0,0
          lw r1,-24(r14)
          muli r5,r1,4
          add r4,r4,r5
          % Array byte offset calculated
          sub r5,r3,r4
          sw -100(r14),r5
          % Array access address stored in -100(r14)
          lw r3,-100(r14)
          lw r5,0(r3)
          sw -152(r14),r5
          lw r5,-152(r14)
          % processing:: tem := 
          sw -28(r14),r5
          % Assignment completed
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r5,-8(r14)
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t19 := 1
          addi r4,r0,1
          sw -104(r14),r4
          % processing: t21 := j + t19
          lw r4,-24(r14)
          lw r1,-104(r14)
          add r2,r4,r1
          sw -112(r14),r2
          lw r2,-112(r14)
          muli r1,r2,4
          add r3,r3,r1
          % Array byte offset calculated
          sub r1,r5,r3
          sw -116(r14),r1
          % Array access address stored in -116(r14)
          lw r5,-116(r14)
          lw r1,0(r5)
          sw -152(r14),r1
          lw r1,-152(r14)
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r5,-8(r14)
          % Calculating array byte offset
          addi r3,r0,0
          lw r2,-24(r14)
          muli r4,r2,4
          add r3,r3,r4
          % Array byte offset calculated
          sub r4,r5,r3
          sw -120(r14),r4
          % Array access address stored in -120(r14)
          % Storing value into array element
          add r4,r0,r0
          lw r4,-120(r14)
          sw 0(r4),r1
          % Assignment completed
          lw r1,-28(r14)
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r4,-8(r14)
          % Calculating array byte offset
          addi r5,r0,0
          % processing: t24 := 1
          addi r3,r0,1
          sw -124(r14),r3
          % processing: t26 := j + t24
          lw r3,-24(r14)
          lw r2,-124(r14)
          add r6,r3,r2
          sw -132(r14),r6
          lw r6,-132(r14)
          muli r2,r6,4
          add r5,r5,r2
          % Array byte offset calculated
          sub r2,r4,r5
          sw -136(r14),r2
          % Array access address stored in -136(r14)
          % Storing value into array element
          add r2,r0,r0
          lw r2,-136(r14)
          sw 0(r2),r1
          % Assignment completed
          j endif_6
          % else branch
else_5
endif_6
          % end if
          % processing: t28 := 1
          addi r1,r0,1
          sw -140(r14),r1
          % processing: t29 := j + t28
          lw r1,-24(r14)
          lw r2,-140(r14)
          add r4,r1,r2
          sw -144(r14),r4
          lw r4,-144(r14)
          % processing:: j := t29
          sw -24(r14),r4
          % Assignment completed
          j while_start_2
while_end_3
          % end while loop
          % processing: t30 := 1
          addi r4,r0,1
          sw -148(r14),r4
          % processing: t31 := i + t30
          lw r4,-20(r14)
          lw r2,-148(r14)
          add r1,r4,r2
          sw -152(r14),r1
          lw r1,-152(r14)
          % processing:: i := t31
          sw -20(r14),r1
          % Assignment completed
          j while_start_0
while_end_1
          % end while loop
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: printArray
          % Entering function scope: printArray
printArray
          % Function prologue (printArray)
          sw -4(r14),r15
          lw r1,-12(r14)
          % processing:: n := 
          sw -16(r14),r1
          % Assignment completed
          % processing: t32 := 0
          addi r1,r0,0
          sw -24(r14),r1
          lw r1,-24(r14)
          % processing:: i := t32
          sw -20(r14),r1
          % Assignment completed
while_start_7
          % while loop - evaluating condition
          lw r1,-20(r14)
          lw r2,-16(r14)
          clt r4,r1,r2
          sw -28(r14),r4
          add r4,r4,r0
          bz r4,while_end_8
          % while loop - executing body
          % Processing array access
          % Dynamic array - loading array address from memory
          lw r4,-8(r14)
          % Calculating array byte offset
          addi r2,r0,0
          lw r1,-20(r14)
          muli r5,r1,4
          add r2,r2,r5
          % Array byte offset calculated
          sub r5,r4,r2
          sw -32(r14),r5
          % Array access address stored in -32(r14)
          % Warning: Using direct offset metadata
          lw r2,-32(r14)
          lw r5,0(r2)
          % processing: write()
          % put value on stack
          addi r14,r14,-40
          sw -8(r14),r5
          % link buffer to stack
          addi r5,r0,buf
          sw -12(r14),r5
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-40
          % processing: t35 := 1
          addi r4,r0,1
          sw -36(r14),r4
          % processing: t36 := i + t35
          lw r4,-20(r14)
          lw r5,-36(r14)
          add r2,r4,r5
          sw -40(r14),r2
          lw r2,-40(r14)
          % processing:: i := t36
          sw -20(r14),r2
          % Assignment completed
          j while_start_7
while_end_8
          % end while loop
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t37 := 64
          addi r2,r0,64
          sw -36(r14),r2
          lw r2,-36(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r5,r14,32
          % Calculating array byte offset
          addi r4,r0,0
          % processing: t38 := 0
          addi r1,r0,0
          sw -40(r14),r1
          lw r1,-40(r14)
          muli r6,r1,4
          add r4,r4,r6
          % Array byte offset calculated
          sub r6,r5,r4
          sw -44(r14),r6
          % Array access address stored in -44(r14)
          % Storing value into array element
          add r6,r0,r0
          lw r6,-44(r14)
          sw 0(r6),r2
          % Assignment completed
          % processing: t40 := 34
          addi r2,r0,34
          sw -48(r14),r2
          lw r2,-48(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r6,r14,32
          % Calculating array byte offset
          addi r5,r0,0
          % processing: t41 := 1
          addi r4,r0,1
          sw -52(r14),r4
          lw r4,-52(r14)
          muli r1,r4,4
          add r5,r5,r1
          % Array byte offset calculated
          sub r1,r6,r5
          sw -56(r14),r1
          % Array access address stored in -56(r14)
          % Storing value into array element
          add r1,r0,r0
          lw r1,-56(r14)
          sw 0(r1),r2
          % Assignment completed
          % processing: t43 := 25
          addi r2,r0,25
          sw -60(r14),r2
          lw r2,-60(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r1,r14,32
          % Calculating array byte offset
          addi r6,r0,0
          % processing: t44 := 2
          addi r5,r0,2
          sw -64(r14),r5
          lw r5,-64(r14)
          muli r4,r5,4
          add r6,r6,r4
          % Array byte offset calculated
          sub r4,r1,r6
          sw -68(r14),r4
          % Array access address stored in -68(r14)
          % Storing value into array element
          add r4,r0,r0
          lw r4,-68(r14)
          sw 0(r4),r2
          % Assignment completed
          % processing: t46 := 12
          addi r2,r0,12
          sw -72(r14),r2
          lw r2,-72(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r4,r14,32
          % Calculating array byte offset
          addi r1,r0,0
          % processing: t47 := 3
          addi r6,r0,3
          sw -76(r14),r6
          lw r6,-76(r14)
          muli r5,r6,4
          add r1,r1,r5
          % Array byte offset calculated
          sub r5,r4,r1
          sw -80(r14),r5
          % Array access address stored in -80(r14)
          % Storing value into array element
          add r5,r0,r0
          lw r5,-80(r14)
          sw 0(r5),r2
          % Assignment completed
          % processing: t49 := 22
          addi r2,r0,22
          sw -84(r14),r2
          lw r2,-84(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r5,r14,32
          % Calculating array byte offset
          addi r4,r0,0
          % processing: t50 := 4
          addi r1,r0,4
          sw -88(r14),r1
          lw r1,-88(r14)
          muli r6,r1,4
          add r4,r4,r6
          % Array byte offset calculated
          sub r6,r5,r4
          sw -92(r14),r6
          % Array access address stored in -92(r14)
          % Storing value into array element
          add r6,r0,r0
          lw r6,-92(r14)
          sw 0(r6),r2
          % Assignment completed
          % processing: t52 := 11
          addi r2,r0,11
          sw -96(r14),r2
          lw r2,-96(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r6,r14,32
          % Calculating array byte offset
          addi r5,r0,0
          % processing: t53 := 5
          addi r4,r0,5
          sw -100(r14),r4
          lw r4,-100(r14)
          muli r1,r4,4
          add r5,r5,r1
          % Array byte offset calculated
          sub r1,r6,r5
          sw -104(r14),r1
          % Array access address stored in -104(r14)
          % Storing value into array element
          add r1,r0,r0
          lw r1,-104(r14)
          sw 0(r1),r2
          % Assignment completed
          % processing: t55 := 90
          addi r2,r0,90
          sw -108(r14),r2
          lw r2,-108(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r1,r14,32
          % Calculating array byte offset
          addi r6,r0,0
          % processing: t56 := 6
          addi r5,r0,6
          sw -112(r14),r5
          lw r5,-112(r14)
          muli r4,r5,4
          add r6,r6,r4
          % Array byte offset calculated
          sub r4,r1,r6
          sw -116(r14),r4
          % Array access address stored in -116(r14)
          % Storing value into array element
          add r4,r0,r0
          lw r4,-116(r14)
          sw 0(r4),r2
          % Assignment completed
          % Function call: printArray
          % Pushing parameters onto stack
          % processing: t58 := 7
          addi r2,r0,7
          sw -120(r14),r2
          lw r2,-120(r14)
          sw -140(r14),r2
          % Passing array address for parameter: arr
          subi r2,r14,32
          sw -136(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-128
          jl r15,printArray
          % Function returned, restoring stack
          subi r14,r14,-128
          % Function call: bubbleSort
          % Pushing parameters onto stack
          % processing: t59 := 7
          addi r2,r0,7
          sw -124(r14),r2
          lw r2,-124(r14)
          sw -140(r14),r2
          % Passing array address for parameter: arr
          subi r2,r14,32
          sw -136(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-128
          jl r15,bubbleSort
          % Function returned, restoring stack
          subi r14,r14,-128
          % Function call: printArray
          % Pushing parameters onto stack
          % processing: t60 := 7
          addi r2,r0,7
          sw -128(r14),r2
          lw r2,-128(r14)
          sw -140(r14),r2
          % Passing array address for parameter: arr
          subi r2,r14,32
          sw -136(r14),r2
          % Setting up activation record for function call
          addi r14,r14,-128
          jl r15,printArray
          % Function returned, restoring stack
          subi r14,r14,-128
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

