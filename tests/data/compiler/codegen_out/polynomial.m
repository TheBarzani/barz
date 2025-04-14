% Code Section
          % Processing class method: POLYNOMIAL::evaluate

          % Processing function: POLYNOMIAL::evaluate
          % Using table for method evaluate_int
          % Entering function scope: POLYNOMIAL_evaluate
POLYNOMIAL_evaluate
          % Function prologue (POLYNOMIAL_evaluate)
          sw -4(r14),r15
          % processing: t2 := 0
          addi r1,r0,0
          sw -20(r14),r1
          lw r1,-20(r14)
          sw 0(r14),r1
          add r13,r1,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Processing class method: LINEAR::build

          % Processing function: LINEAR::build
          % Using table for method build_int_int
          % Entering function scope: LINEAR_build
LINEAR_build
          % Function prologue (LINEAR_build)
          sw -4(r14),r15
          % processing: t4 := 1
          addi r1,r0,1
          sw -32(r14),r1
          % processing: t5 := 1
          addi r1,r0,1
          sw -36(r14),r1
          % processing: t6 := t4 + t5
          lw r1,-32(r14)
          lw r2,-36(r14)
          add r3,r1,r2
          sw -40(r14),r3
          lw r3,-40(r14)
          % processing:: c := t6
          sw -28(r14),r3
          % Assignment completed
          lw r3,-16(r14)
          % Processing dot access expression
          addi r2,r14,-48
          subi r1,r2,0
          sw -52(r14),r1
          % Member 'a' address stored at offset -52
          lw r1,-52(r14)
          % Storing value into class member
          sw 0(r1),r3
          % Assignment completed
          lw r3,-20(r14)
          % Processing dot access expression
          addi r1,r14,-48
          subi r2,r1,-4
          sw -56(r14),r2
          % Member 'b' address stored at offset -56
          lw r2,-56(r14)
          % Storing value into class member
          sw 0(r2),r3
          % Assignment completed
          lw r3,-16(r14)
          % processing:: a := 
          sw -4(r14),r3
          % Assignment completed
          lw r3,-48(r14)
          sw 0(r14),r3
          add r13,r3,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Processing class method: LINEAR::evaluate

          % Processing function: LINEAR::evaluate
          % Using table for method evaluate_int
          % Entering function scope: LINEAR_evaluate
LINEAR_evaluate
          % Function prologue (LINEAR_evaluate)
          sw -4(r14),r15
          % processing: t10 := 0
          addi r3,r0,0
          sw -24(r14),r3
          lw r3,-24(r14)
          % processing:: result := t10
          sw -20(r14),r3
          % Assignment completed
          lw r3,-4(r14)
          lw r2,-12(r14)
          % processing: t11 := a * x
          mul r1,r3,r2
          sw -28(r14),r1
          % processing: t12 := t11 + b
          lw r1,-28(r14)
          lw r2,-8(r14)
          add r3,r1,r2
          sw -32(r14),r3
          lw r3,-32(r14)
          % processing:: result := t12
          sw -20(r14),r3
          % Assignment completed
          lw r3,-20(r14)
          sw 0(r14),r3
          add r13,r3,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Processing class method: QUADRATIC::evaluate

          % Processing function: QUADRATIC::evaluate
          % Using table for method evaluate_int
          % Entering function scope: QUADRATIC_evaluate
QUADRATIC_evaluate
          % Function prologue (QUADRATIC_evaluate)
          sw -4(r14),r15
          lw r3,-4(r14)
          % processing:: result := 
          sw -20(r14),r3
          % Assignment completed
          lw r3,-20(r14)
          lw r2,-12(r14)
          % processing: t14 := result * x
          mul r1,r3,r2
          sw -24(r14),r1
          % processing: t15 := t14 + b
          lw r1,-24(r14)
          lw r2,-8(r14)
          add r3,r1,r2
          sw -28(r14),r3
          lw r3,-28(r14)
          % processing:: result := t15
          sw -20(r14),r3
          % Assignment completed
          lw r3,-20(r14)
          lw r2,-12(r14)
          % processing: t16 := result * x
          mul r1,r3,r2
          sw -32(r14),r1
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r1,r14,32
          % Calculating array byte offset
          addi r2,r0,0
          % processing: t17 := 1
          addi r3,r0,1
          sw -36(r14),r3
          lw r3,-36(r14)
          muli r4,r3,12
          add r2,r2,r4
          % processing: t18 := 1
          addi r4,r0,1
          sw -40(r14),r4
          lw r4,-40(r14)
          muli r3,r4,4
          add r2,r2,r3
          % Array byte offset calculated
          sub r3,r1,r2
          sw -44(r14),r3
          % Array access address stored in -44(r14)
          % processing: t20 := t16 + 
          lw r3,-32(r14)
          lw r1,-44(r14)
          lw r1,0(r1)
          add r2,r3,r1
          sw -48(r14),r2
          lw r2,-48(r14)
          % processing:: result := t20
          sw -20(r14),r2
          % Assignment completed
          lw r2,-20(r14)
          sw 0(r14),r2
          add r13,r2,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Processing class method: QUADRATIC::build

          % Processing function: QUADRATIC::build
          % Using table for method build_int_int_int
          % Entering function scope: QUADRATIC_build
QUADRATIC_build
          % Function prologue (QUADRATIC_build)
          sw -4(r14),r15
          lw r2,-40(r14)
          % Processing dot access expression
          addi r1,r14,-84
          subi r3,r1,0
          sw -88(r14),r3
          % Member 'a' address stored at offset -88
          lw r3,-88(r14)
          % Storing value into class member
          sw 0(r3),r2
          % Assignment completed
          lw r2,-44(r14)
          % Processing dot access expression
          addi r3,r14,-84
          subi r1,r3,-4
          sw -92(r14),r1
          % Member 'b' address stored at offset -92
          lw r1,-92(r14)
          % Storing value into class member
          sw 0(r1),r2
          % Assignment completed
          lw r2,-48(r14)
          % Processing dot access expression
          addi r1,r14,-84
          % Processing array access
          add r3,r0,r1
          % Calculating array byte offset
          % Using dimensions from class member array: c
          addi r4,r0,0
          % processing: t24 := 1
          addi r5,r0,1
          sw -96(r14),r5
          lw r5,-96(r14)
          muli r6,r5,12
          add r4,r4,r6
          % processing: t25 := 1
          addi r6,r0,1
          sw -100(r14),r6
          lw r6,-100(r14)
          muli r5,r6,4
          add r4,r4,r5
          % Array byte offset calculated
          sub r5,r3,r4
          sw -104(r14),r5
          % Array access address stored in -104(r14)
          subi r5,r1,-8
          sw -108(r14),r5
          % Member 'c' address stored at offset -108
          lw r5,-108(r14)
          % Storing value into class member
          sw 0(r5),r2
          % Assignment completed
          lw r2,-84(r14)
          sw 0(r14),r2
          add r13,r2,r0
          lw r15,-4(r14)
          jr r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t28 := 2
          addi r2,r0,2
          sw -172(r14),r2
          lw r2,-172(r14)
          % Processing dot access expression
          addi r5,r14,-132
          subi r1,r5,0
          sw -176(r14),r1
          % Member 'a' address stored at offset -176
          lw r1,-176(r14)
          % Storing value into class member
          sw 0(r1),r2
          % Assignment completed
          % processing: t30 := 3
          addi r2,r0,3
          sw -180(r14),r2
          lw r2,-180(r14)
          % Processing dot access expression
          addi r1,r14,-132
          subi r5,r1,-4
          sw -184(r14),r5
          % Member 'b' address stored at offset -184
          lw r5,-184(r14)
          % Storing value into class member
          sw 0(r5),r2
          % Assignment completed
          % Processing dot access expression
          addi r2,r14,-132
          % Member function call on object type: LINEAR
          % Function call: LINEAR::evaluate
          % Pushing 'self' object address as first parameter
          sw -228(r14),r2
          % Pushing parameters onto stack
          % processing: t32 := 10
          addi r5,r0,10
          sw -188(r14),r5
          lw r5,-188(r14)
          sw -208(r14),r5
          % Setting up activation record for function call
          addi r14,r14,-196
          % Calling member function: LINEAR_evaluate
          jl r15,LINEAR_evaluate
          % Function returned, restoring stack
          subi r14,r14,-196
          % Error: Member not found or missing offset: evaluate
          lw r1,-28(r14)
          lw r2,0(r1)
          % processing: write(t34)
          % put value on stack
          addi r14,r14,-196
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
          subi r14,r14,-196
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

