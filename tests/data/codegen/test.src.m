% Code Section

          % Processing function: func1
          % Entering function scope: func1
func1
          % Function prologue (func1)
          sw -4(r14),r15
          % Function epilogue
          lw r15,-4(r14)
          jr r15
          % Exiting function scope: func1

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 69
          addi r1,r0,69
          sw -12(r14),r1
          % processing:: x := t1
          lw r1,-12(r14)
          sw -4(r14),r1
          % processing: t2 := 60
          addi r1,r0,60
          sw -16(r14),r1
          % processing:: y := t2
          lw r1,-16(r14)
          sw -8(r14),r1
          % processing: t3 := 2
          addi r1,r0,2
          sw -20(r14),r1
          % processing: t4 := x + t3
          lw r1,-4(r14)
          lw r2,-20(r14)
          add r3,r1,r2
          sw -24(r14),r3
          % processing: write(t4)
          lw r2,-24(r14)
          % put value on stack
          addi r14,r14,-24
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
          subi r14,r14,-24
          % processing: write(y)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-24
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
          subi r14,r14,-24
          % Processing: read(y)
          % Allocate buffer space for input
          addi r2,r0,buf
          sw -8(r14),r2
          % Read string from console
          addi r14,r14,-24
          jl r15,getstr
          subi r14,r14,-24
          % Convert string to integer
          addi r2,r0,buf
          sw -8(r14),r2
          addi r14,r14,-24
          jl r15,strint
          subi r14,r14,-24
          % Store read value into r13
          sw -8(r14),r13
          % processing: write(y)
          lw r2,-8(r14)
          % put value on stack
          addi r14,r14,-24
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
          subi r14,r14,-24
          % Exiting function scope: main
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

