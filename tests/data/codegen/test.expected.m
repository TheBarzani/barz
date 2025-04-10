          % Brief: this program is for memsize allocation and write/read operations.
          
          align
main      
          entry
          addi r14,r0,topaddr

          % processing: t1 := 69
          addi r1,r0,69
          sw -12(r14), r1

          % processing: x := t1
          lw r2,-12(r14)
          sw -4(r14), r2

          % processing: t2 := 70
          addi r2,r0,70
          sw -16(r14),r2
          % processing: y := t2
          lw r3,-16(r14)
          sw -8(r14),r3


          %============================================
          % processing: write(x)
          lw r1, -4(r14)

          % put value on stack
          % custom comment: -16 here is the scope offset
          addi r14,r14,-16
          sw -8(r14),r1

          % link buffer to stack
          addi r1,r0, buf
          sw -12(r14),r1

          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13

          % output to console
          jl r15, putstr
          jl r15,putnl         % Add newline
          subi r14,r14,-16
          

          %============================================
          % Processing: read(y)
          % Allocate buffer space (could be in data section)
          addi r1,r0,buf       % r1 points to buffer
          sw -8(r14),r1        % Pass buffer address as argument

          % Read string from stdin
          addi r14,r14,-16      % Adjust stack for function call
          jl r15,getstr        % Call getstr library function
          subi r14,r14,-16      % Restore stack

          % Convert string to integer
          addi r1,r0,buf       % r1 points to buffer again
          sw -8(r14),r1        % Pass buffer address as argument

          addi r14,r14,-16      % Adjust stack for function call
          jl r15,strint        % Call strint library function
          subi r14,r14,-16      % Restore stack

          % Now r13 contains the integer value
          sw -8(r14),r13       % Store result in variable y


          %============================================
          % processing: write(y)
          lw r1, -8(r14)

          % put value on stack
          % custom comment: -16 here is the scope offset
          addi r14,r14,-16
          sw -8(r14),r1

          % link buffer to stack
          addi r1,r0, buf
          sw -12(r14),r1

          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13

          % output to console
          jl r15, putstr
          jl r15,putnl         % Add newline
          subi r14,r14,-16

          % End of program
          hlt

          % buffer space used for console output
buf       res 20