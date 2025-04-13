% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 77
          addi r1,r0,77
          sw -108(r14),r1
          lw r1,-108(r14)
          % processing:: c := t1
          sw -8(r14),r1
          % Assignment completed
          lw r1,-8(r14)
          % Processing array access
          subi r2,r14,104
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t2 := 2
          addi r4,r0,2
          sw -112(r14),r4
          lw r4,-112(r14)
          muli r5,r4,48
          add r3,r3,r5
          % processing: t3 := 2
          addi r5,r0,2
          sw -116(r14),r5
          lw r5,-116(r14)
          muli r4,r5,16
          add r3,r3,r4
          % processing: t4 := 3
          addi r4,r0,3
          sw -120(r14),r4
          lw r4,-120(r14)
          muli r5,r4,4
          add r3,r3,r5
          % Array byte offset calculated
          sub r5,r2,r3
          sw -124(r14),r5
          % Array access address stored in -124(r14)
          % Storing value into array element
          add r5,r0,r0
          lw r5,-124(r14)
          sw 0(r5),r1
          % Assignment completed
          % Processing array access
          subi r1,r14,104
          % Calculating array byte offset
          addi r5,r0,0
          % processing: t6 := 2
          addi r2,r0,2
          sw -128(r14),r2
          lw r2,-128(r14)
          muli r3,r2,48
          add r5,r5,r3
          % processing: t7 := 2
          addi r3,r0,2
          sw -132(r14),r3
          lw r3,-132(r14)
          muli r2,r3,16
          add r5,r5,r2
          % processing: t8 := 3
          addi r2,r0,3
          sw -136(r14),r2
          lw r2,-136(r14)
          muli r3,r2,4
          add r5,r5,r3
          % Array byte offset calculated
          sub r3,r1,r5
          sw -140(r14),r3
          % Array access address stored in -140(r14)
          lw r5,-140(r14)
          lw r3,0(r5)
          % processing: write()
          % put value on stack
          addi r14,r14,-140
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
          subi r14,r14,-140
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

