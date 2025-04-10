           entry
           addi r14,r0,topaddr
           % processing: t1 := 1
           addi r1,r0,1
           sw t1(r0),r1
           % processing: a := t1
           lw r1,t1(r0)
           sw a(r0),r1
           % processing: put(a)
           lw r1,a(r0)
           % put value on stack
           sw -8(r14),r1
           % link buffer to stack
           addi r1,r0, buf
           sw -12(r14),r1
           % convert int to string for output
           jl r15, intstr
           sw -8(r14),r13
           % output to console
           jl r15, putstr
           % processing: t2 := 2
           addi r1,r0,2
           sw t2(r0),r1
           % processing: b := t2
           lw r1,t2(r0)
           sw b(r0),r1
           % processing: put(b)
           lw r1,b(r0)
           % put value on stack
           sw -8(r14),r1
           % link buffer to stack
           addi r1,r0, buf
           sw -12(r14),r1
           % convert int to string for output
           jl r15, intstr
           sw -8(r14),r13
           % output to console
           jl r15, putstr
           % processing: t3 := 3
           addi r1,r0,3
           sw t3(r0),r1
           % processing: c := t3
           lw r1,t3(r0)
           sw c(r0),r1
           % processing: put(c)
           lw r1,c(r0)
           % put value on stack
           sw -8(r14),r1
           % link buffer to stack
           addi r1,r0, buf
           sw -12(r14),r1
           % convert int to string for output
           jl r15, intstr
           sw -8(r14),r13
           % output to console
           jl r15, putstr
           % processing: t4 := b * c
           lw r2,b(r0)
           lw r3,c(r0)
           mul r1,r2,r3
           sw t4(r0),r1
           % processing: t5 := a + t4
           lw r3,a(r0)
           lw r2,t4(r0)
           add r1,r3,r2
           sw t5(r0),r1
           % processing: a := t5
           lw r1,t5(r0)
           sw a(r0),r1
           % processing: t6 := 6
           addi r1,r0,6
           sw t6(r0),r1
           % processing: t7 := a + t6
           lw r2,a(r0)
           lw r3,t6(r0)
           add r1,r2,r3
           sw t7(r0),r1
           % processing: put(t7)
           lw r1,t7(r0)
           % put value on stack
           sw -8(r14),r1
           % link buffer to stack
           addi r1,r0, buf
           sw -12(r14),r1
           % convert int to string for output
           jl r15, intstr
           sw -8(r14),r13
           % output to console
           jl r15, putstr
           hlt

           % space for variable a
a          res 4
           % space for variable b
b          res 4
           % space for variable c
c          res 4
           % space for constant 1
t1         res 4
           % space for constant 2
t2         res 4
           % space for constant 3
t3         res 4
           % space for b * c
t4         res 4
           % space for a + t4
t5         res 4
           % space for constant 6
t6         res 4
           % space for a + t6
t7         res 4
           % buffer space used for console output
buf        res 20

