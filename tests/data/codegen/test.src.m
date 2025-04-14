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
          % processing: t10 := 100
          addi r3,r0,100
          sw -148(r14),r3
          % processing: t11 := t9 + t10
          lw r3,-144(r14)
          lw r3,0(r3)
          lw r1,-148(r14)
          add r5,r3,r1
          sw -152(r14),r5
          % processing: write(t11)
          lw r1,-152(r14)
          % put value on stack
          addi r14,r14,-192
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
          subi r14,r14,-192
          % processing: t12 := 100
          addi r3,r0,100
          sw -156(r14),r3
          lw r3,-156(r14)
          % Processing dot access expression
          % Processing dot access expression
          addi r1,r14,-108
          subi r2,r1,0
          sw -160(r14),r2
          % Member 'b' address stored at offset -160
          lw r2,-160(r14)
          subi r1,r2,-8
          sw -164(r14),r1
          % Member 'c' address stored at offset -164
          lw r1,-164(r14)
          % Storing value into class member
          sw 0(r1),r3
          % Assignment completed
          % Processing dot access expression
          % Processing dot access expression
          addi r3,r14,-108
          subi r1,r3,0
          sw -168(r14),r1
          % Member 'b' address stored at offset -168
          lw r1,-168(r14)
          subi r3,r1,-8
          sw -172(r14),r3
          % Member 'c' address stored at offset -172
          lw r2,-172(r14)
          lw r3,0(r2)
          % processing: write(t16)
          % put value on stack
          addi r14,r14,-192
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
          subi r14,r14,-192
          % processing: t17 := 69
          addi r1,r0,69
          sw -176(r14),r1
          lw r1,-176(r14)
          % Processing dot access expression
          addi r3,r14,-108
          subi r2,r3,-20
          sw -180(r14),r2
          % Member 'a' address stored at offset -180
          lw r2,-180(r14)
          % Storing value into class member
          sw 0(r2),r1
          % Assignment completed
          % Processing dot access expression
          addi r1,r14,-108
          subi r2,r1,-20
          sw -184(r14),r2
          % Member 'a' address stored at offset -184
          % processing: t20 := 1
          addi r2,r0,1
          sw -188(r14),r2
          % processing: t21 := t19 + t20
          lw r2,-184(r14)
          lw r2,0(r2)
          lw r1,-188(r14)
          add r3,r2,r1
          sw -192(r14),r3
          % processing: write(t21)
          lw r1,-192(r14)
          % put value on stack
          addi r14,r14,-192
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
          subi r14,r14,-192
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

