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
          subi r1,r2,0
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
          % Processing class method: A::printi

          % Processing function: A::printi
          % Using table for method printi_int
          % Entering function scope: A_printi
A_printi
          % Function prologue (A_printi)
          sw -4(r14),r15
          lw r2,-12(r14)
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
          % processing: t4 := 111
          addi r2,r0,111
          sw -12(r14),r2
          lw r2,-12(r14)
          % Processing dot access expression
          addi r1,r14,-8
          subi r3,r1,0
          sw -16(r14),r3
          % Member 'a' address stored at offset -16
          lw r3,-16(r14)
          % Storing value into class member
          sw 0(r3),r2
          % Assignment completed
          % Processing dot access expression
          addi r2,r14,-8
          % Member function call on object type: A
          % Function call: A::print
          % Pushing 'self' object address as first parameter
          sw -40(r14),r2
          % Pushing parameters onto stack
          % processing: t6 := 999
          addi r3,r0,999
          sw -20(r14),r3
          lw r3,-20(r14)
          sw -32(r14),r3
          % Setting up activation record for function call
          addi r14,r14,-24
          % Calling member function: A_print
          jl r15,A_print
          % Function returned, restoring stack
          subi r14,r14,-24
          % Error: Member not found or missing offset: print
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

