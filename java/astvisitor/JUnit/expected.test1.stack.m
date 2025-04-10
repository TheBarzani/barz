          % processing function definition: 
f         sw -4(r14),r15
          % processing: put(p1)
          lw r1,-12(r14)
          % put value on stack
          addi r14,r14,-32
          sw -8(r14),r1
          % link buffer to stack
          addi r1,r0, buf
          sw -12(r14),r1
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-32
          % processing: put(p2)
          lw r2,-16(r14)
          % put value on stack
          addi r14,r14,-32
          sw -8(r14),r2
          % link buffer to stack
          addi r2,r0, buf
          sw -12(r14),r2
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-32
          % processing: put(p3)
          lw r1,-20(r14)
          % put value on stack
          addi r14,r14,-32
          sw -8(r14),r1
          % link buffer to stack
          addi r1,r0, buf
          sw -12(r14),r1
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-32
          % processing: t1 := p2 * p3
          lw r1,-16(r14)
          lw r3,-20(r14)
          mul r4,r1,r3
          sw -28(r14),r4
          % processing: t2 := p1 + t1
          lw r3,-12(r14)
          lw r1,-28(r14)
          add r2,r3,r1
          sw -32(r14),r2
          % processing: retval := t2
          lw r1,-32(r14)
          sw -24(r14),r1
          % processing: put(retval)
          lw r3,-24(r14)
          % put value on stack
          addi r14,r14,-32
          sw -8(r14),r3
          % link buffer to stack
          addi r3,r0, buf
          sw -12(r14),r3
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-32
          % processing: return(retval)
          lw r1,-24(r14)
          sw 0(r14),r1
          lw r15,-4(r14)
          jr r15
          entry
          addi r14,r0,topaddr
          % processing: t3 := 1
          addi r1,r0,1
          sw -16(r14),r1
          % processing: a := t3
          lw r3,-16(r14)
          sw -4(r14),r3
          % processing: put(a)
          lw r2,-4(r14)
          % put value on stack
          addi r14,r14,-44
          sw -8(r14),r2
          % link buffer to stack
          addi r2,r0, buf
          sw -12(r14),r2
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-44
          % processing: t4 := 2
          addi r3,r0,2
          sw -20(r14),r3
          % processing: b := t4
          lw r2,-20(r14)
          sw -8(r14),r2
          % processing: put(b)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-44
          sw -8(r14),r1
          % link buffer to stack
          addi r1,r0, buf
          sw -12(r14),r1
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-44
          % processing: t5 := 3
          addi r2,r0,3
          sw -24(r14),r2
          % processing: c := t5
          lw r1,-24(r14)
          sw -12(r14),r1
          % processing: put(c)
          lw r3,-12(r14)
          % put value on stack
          addi r14,r14,-44
          sw -8(r14),r3
          % link buffer to stack
          addi r3,r0, buf
          sw -12(r14),r3
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-44
          % processing: function call to f 
          lw r1,-4(r14)
          sw -56(r14),r1
          lw r1,-8(r14)
          sw -60(r14),r1
          lw r1,-12(r14)
          sw -64(r14),r1
          addi r14,r14,-44
          jl r15,f
          subi r14,r14,-44
          lw r1,-44(r14)
          sw -28(r14),r1
          % processing: t7 := t6 * b
          lw r3,-28(r14)
          lw r2,-8(r14)
          mul r4,r3,r2
          sw -32(r14),r4
          % processing: t8 := t7 + c
          lw r2,-32(r14)
          lw r3,-12(r14)
          add r1,r2,r3
          sw -36(r14),r1
          % processing: a := t8
          lw r3,-36(r14)
          sw -4(r14),r3
          % processing: t9 := 4
          addi r2,r0,4
          sw -40(r14),r2
          % processing: t10 := a + t9
          lw r3,-4(r14)
          lw r1,-40(r14)
          add r4,r3,r1
          sw -44(r14),r4
          % processing: put(t10)
          lw r4,-44(r14)
          % put value on stack
          addi r14,r14,-44
          sw -8(r14),r4
          % link buffer to stack
          addi r4,r0, buf
          sw -12(r14),r4
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-44
          hlt

          % buffer space used for console output
buf       res 20

