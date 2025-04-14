% Code Section
          % Processing class method: A::print

          % Processing function: A::print
          % Using table for method print_int
          % Entering function scope: A_print
A_print
          % Function prologue (A_print)
          sw -4(r14),r15
          % processing: write(a)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-16
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
          subi r14,r14,-16
          % Processing dot access expression
          % Error: Could not determine offset for node
          % Loading 'self' object address
          lw r2,-16(r14)
          % Using 'self' object of class A
          subi r1,r2,-8
          sw -16(r14),r1
          % Member 'a' address stored at offset -16
          lw r3,-16(r14)
          lw r1,0(r3)
          % processing: write(t2)
          % put value on stack
          addi r14,r14,-16
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
          subi r14,r14,-16
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t3 := 699
          addi r2,r0,699
          sw -100(r14),r2
          lw r2,-100(r14)
          % Processing dot access expression
          addi r1,r14,-96
          % Processing array access
          add r3,r0,r1
          % Calculating array byte offset
          % Using dimensions from class member array: d
          addi r4,r0,0
          % processing: t4 := 1
          addi r5,r0,1
          sw -104(r14),r5
          lw r5,-104(r14)
          muli r6,r5,16
          add r4,r4,r6
          % processing: t5 := 2
          addi r6,r0,2
          sw -108(r14),r6
          lw r6,-108(r14)
          muli r5,r6,4
          add r4,r4,r5
          % Array byte offset calculated
          sub r5,r3,r4
          sw -112(r14),r5
          % Array access address stored in -112(r14)
          subi r5,r1,-12
          sw -116(r14),r5
          % Member 'd' address stored at offset -116
          lw r5,-116(r14)
          % Storing value into class member
          sw 0(r5),r2
          % Assignment completed
          % Processing dot access expression
          addi r2,r14,-96
          % Processing array access
          add r5,r0,r2
          % Calculating array byte offset
          % Using dimensions from class member array: d
          addi r1,r0,0
          % processing: t8 := 1
          addi r3,r0,1
          sw -120(r14),r3
          lw r3,-120(r14)
          muli r4,r3,16
          add r1,r1,r4
          % processing: t9 := 2
          addi r4,r0,2
          sw -124(r14),r4
          lw r4,-124(r14)
          muli r3,r4,4
          add r1,r1,r3
          % Array byte offset calculated
          sub r3,r5,r1
          sw -128(r14),r3
          % Array access address stored in -128(r14)
          subi r3,r2,-12
          sw -132(r14),r3
          % Member 'd' address stored at offset -132
          % processing: t12 := 100
          addi r3,r0,100
          sw -136(r14),r3
          % processing: t13 := t11 + t12
          lw r3,-132(r14)
          lw r3,0(r3)
          lw r2,-136(r14)
          add r5,r3,r2
          sw -140(r14),r5
          % processing: write(t13)
          lw r5,-140(r14)
          % put value on stack
          addi r14,r14,-168
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
          subi r14,r14,-168
          % processing: t14 := 89
          addi r2,r0,89
          sw -144(r14),r2
          lw r2,-144(r14)
          % Processing dot access expression
          addi r5,r14,-96
          subi r3,r5,-8
          sw -148(r14),r3
          % Member 'a' address stored at offset -148
          lw r3,-148(r14)
          % Storing value into class member
          sw 0(r3),r2
          % Assignment completed
          % Processing dot access expression
          addi r2,r14,-96
          subi r3,r2,-8
          sw -152(r14),r3
          % Member 'a' address stored at offset -152
          % processing: t17 := 1
          addi r3,r0,1
          sw -156(r14),r3
          % processing: t18 := t16 + t17
          lw r3,-152(r14)
          lw r3,0(r3)
          lw r2,-156(r14)
          add r5,r3,r2
          sw -160(r14),r5
          % processing: write(t18)
          lw r5,-160(r14)
          % put value on stack
          addi r14,r14,-168
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
          subi r14,r14,-168
          % Processing dot access expression
          addi r2,r14,-96
          % Member function call on object type: A
          % Function call: A::print
          % Pushing 'self' object address as first parameter
          sw -184(r14),r2
          % Pushing parameters onto stack
          % processing: t19 := 96
          addi r5,r0,96
          sw -164(r14),r5
          lw r5,-164(r14)
          sw -176(r14),r5
          % Setting up activation record for function call
          addi r14,r14,-168
          % Calling member function: A_print
          jl r15,A_print
          % Function returned, restoring stack
          subi r14,r14,-168
          % Error: Member not found or missing offset: print
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

