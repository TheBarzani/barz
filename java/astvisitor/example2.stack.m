          entry
          addi r14,r0,topaddr
          % processing: t1 := 1
          addi r1,r0,1
          sw -16(r14),r1
          % processing: a := t1
          lw r2,-16(r14)
          sw -4(r14),r2
          % processing: put(a)
          lw r3,-4(r14)
          % put value on stack
          addi r14,r14,-40
          sw -8(r14),r3
          % link buffer to stack
          addi r3,r0, buf
          sw -12(r14),r3
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-40
          % processing: t2 := 2
          addi r2,r0,2
          sw -20(r14),r2
          % processing: b := t2
          lw r3,-20(r14)
          sw -8(r14),r3
          % processing: put(b)
          lw r1,-8(r14)
          % put value on stack
          addi r14,r14,-40
          sw -8(r14),r1
          % link buffer to stack
          addi r1,r0, buf
          sw -12(r14),r1
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-40
          % processing: t3 := 3
          addi r3,r0,3
          sw -24(r14),r3
          % processing: c := t3
          lw r1,-24(r14)
          sw -12(r14),r1
          % processing: put(c)
          lw r2,-12(r14)
          % put value on stack
          addi r14,r14,-40
          sw -8(r14),r2
          % link buffer to stack
          addi r2,r0, buf
          sw -12(r14),r2
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-40
          % processing: t4 := b * c
          lw r2,-8(r14)
          lw r3,-12(r14)
          mul r4,r2,r3
          sw -28(r14),r4
          % processing: t5 := a + t4
          lw r3,-4(r14)
          lw r2,-28(r14)
          add r1,r3,r2
          sw -32(r14),r1
          % processing: a := t5
          lw r2,-32(r14)
          sw -4(r14),r2
          % processing: t6 := 6
          addi r3,r0,6
          sw -36(r14),r3
          % processing: t7 := a + t6
          lw r2,-4(r14)
          lw r1,-36(r14)
          add r4,r2,r1
          sw -40(r14),r4
          % processing: put(t7)
          lw r4,-40(r14)
          % put value on stack
          addi r14,r14,-40
          sw -8(r14),r4
          % link buffer to stack
          addi r4,r0, buf
          sw -12(r14),r4
          % convert int to string for output
          jl r15, intstr
          sw -8(r14),r13
          % output to console
          jl r15, putstr
          subi r14,r14,-40
          hlt

          % buffer space used for console output
buf       res 20

