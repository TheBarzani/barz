% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 699
          addi r1,r0,699
          sw -112(r14),r1
          lw r1,-112(r14)
          % Processing dot access expression
          addi r2,r14,-108
          % Processing array access
          add r3,r0,r2
          % Calculating array byte offset
          % Using dimensions from class member array: d
          addi r4,r0,0
          % processing: t2 := 1
          addi r5,r0,1
          sw -116(r14),r5
          lw r5,-116(r14)
          muli r6,r5,16
          add r4,r4,r6
          % processing: t3 := 2
          addi r6,r0,2
          sw -120(r14),r6
          lw r6,-120(r14)
          muli r5,r6,4
          add r4,r4,r5
          % Array byte offset calculated
          sub r5,r3,r4
          sw -124(r14),r5
          % Array access address stored in -124(r14)
          subi r5,r2,-24
          sw -128(r14),r5
          % Member 'd' address stored at offset -128
          lw r5,-128(r14)
          % Storing value into class member
          sw 0(r5),r1
          % Assignment completed
          % Processing dot access expression
          addi r1,r14,-108
          % Processing array access
          add r5,r0,r1
          % Calculating array byte offset
          % Using dimensions from class member array: d
          addi r2,r0,0
          % processing: t6 := 1
          addi r3,r0,1
          sw -132(r14),r3
          lw r3,-132(r14)
          muli r4,r3,16
          add r2,r2,r4
          % processing: t7 := 2
          addi r4,r0,2
          sw -136(r14),r4
          lw r4,-136(r14)
          muli r3,r4,4
          add r2,r2,r3
          % Array byte offset calculated
          sub r3,r5,r2
          sw -140(r14),r3
          % Array access address stored in -140(r14)
          subi r3,r1,-24
          sw -144(r14),r3
          % Member 'd' address stored at offset -144
          lw r5,-144(r14)
          lw r3,0(r5)
          % processing: write(t9)
          % put value on stack
          addi r14,r14,-144
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
          subi r14,r14,-144
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

