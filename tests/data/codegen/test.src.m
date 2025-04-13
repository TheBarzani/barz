% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 77
          addi r1,r0,77
          sw -112(r14),r1
          lw r1,-112(r14)
          % processing:: c := t1
          sw -8(r14),r1
          % Assignment completed
          lw r1,-8(r14)
          % Processing array access
          subi r2,r14,108
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t2 := 1
          addi r4,r0,1
          sw -116(r14),r4
          % processing: t3 := 1
          addi r4,r0,1
          sw -120(r14),r4
          % processing: t5 := t2 + t3
          lw r4,-116(r14)
          lw r5,-120(r14)
          add r6,r4,r5
          sw -128(r14),r6
          lw r5,-128(r14)
          muli r4,r5,48
          add r3,r3,r4
          % processing: t6 := 2
          addi r4,r0,2
          sw -132(r14),r4
          lw r4,-132(r14)
          muli r5,r4,16
          add r3,r3,r5
          % processing: t7 := 3
          addi r5,r0,3
          sw -136(r14),r5
          lw r5,-136(r14)
          muli r4,r5,4
          add r3,r3,r4
          % Array byte offset calculated
          sub r4,r2,r3
          sw -140(r14),r4
          % Array access address stored in -140(r14)
          % Storing value into array element
          add r4,r0,r0
          lw r4,-140(r14)
          sw 0(r4),r1
          % Assignment completed
          % Processing array access
          subi r1,r14,108
          % Calculating array byte offset
          addi r4,r0,0
          % processing: t9 := 2
          addi r2,r0,2
          sw -144(r14),r2
          lw r2,-144(r14)
          muli r3,r2,48
          add r4,r4,r3
          % processing: t10 := 2
          addi r3,r0,2
          sw -148(r14),r3
          lw r3,-148(r14)
          muli r2,r3,16
          add r4,r4,r2
          % processing: t11 := 3
          addi r2,r0,3
          sw -152(r14),r2
          lw r2,-152(r14)
          muli r3,r2,4
          add r4,r4,r3
          % Array byte offset calculated
          sub r3,r1,r4
          sw -156(r14),r3
          % Array access address stored in -156(r14)
          % processing: t13 := 3
          addi r3,r0,3
          sw -160(r14),r3
          % processing: t14 :=  + t13
          lw r3,-156(r14)
          lw r3,0(r3)
          lw r1,-160(r14)
          add r4,r3,r1
          sw -164(r14),r4
          lw r1,-164(r14)
          % processing:: b := t14
          sw -12(r14),r1
          % Assignment completed
          % processing: write(b)
          lw r1,-12(r14)
          % put value on stack
          addi r14,r14,-180
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
          subi r14,r14,-180
          % Processing array access
          subi r3,r14,108
          % Calculating array byte offset
          addi r1,r0,0
          % processing: t15 := 2
          addi r2,r0,2
          sw -168(r14),r2
          lw r2,-168(r14)
          muli r5,r2,48
          add r1,r1,r5
          % processing: t16 := 2
          addi r5,r0,2
          sw -172(r14),r5
          lw r5,-172(r14)
          muli r2,r5,16
          add r1,r1,r2
          % processing: t17 := 3
          addi r2,r0,3
          sw -176(r14),r2
          lw r2,-176(r14)
          muli r5,r2,4
          add r1,r1,r5
          % Array byte offset calculated
          sub r5,r3,r1
          sw -180(r14),r5
          % Array access address stored in -180(r14)
          lw r1,-180(r14)
          lw r5,0(r1)
          % processing: write()
          % put value on stack
          addi r14,r14,-180
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
          subi r14,r14,-180
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

