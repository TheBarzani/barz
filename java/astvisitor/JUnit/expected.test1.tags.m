           % processing function definition: 
f                    sw flink(r0),r15
           % processing: put(p1)
           lw r1,p1(r0)
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
           % processing: put(p2)
           lw r1,p2(r0)
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
           % processing: put(p3)
           lw r1,p3(r0)
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
           % processing: t1 := p2 * p3
           lw r2,p2(r0)
           lw r3,p3(r0)
           mul r1,r2,r3
           sw t1(r0),r1
           % processing: t2 := p1 + t1
           lw r3,p1(r0)
           lw r2,t1(r0)
           add r1,r3,r2
           sw t2(r0),r1
           % processing: retval := t2
           lw r1,t2(r0)
           sw retval(r0),r1
           % processing: put(retval)
           lw r1,retval(r0)
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
           % processing: return(retval)
           lw r1,retval(r0)
           sw freturn(r0),r1
           lw r15,flink(r0)
           jr r15
           entry
           addi r14,r0,topaddr
           % processing: t3 := 1
           addi r1,r0,1
           sw t3(r0),r1
           % processing: a := t3
           lw r1,t3(r0)
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
           % processing: t4 := 2
           addi r1,r0,2
           sw t4(r0),r1
           % processing: b := t4
           lw r1,t4(r0)
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
           % processing: t5 := 3
           addi r1,r0,3
           sw t5(r0),r1
           % processing: c := t5
           lw r1,t5(r0)
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
           % processing: function call to f 
           lw r1,a(r0)
           sw p1(r0),r1
           lw r1,b(r0)
           sw p2(r0),r1
           lw r1,c(r0)
           sw p3(r0),r1
           jl r15,f
           lw r1,freturn(r0)
           sw t6(r0),r1
           % processing: t7 := t6 * b
           lw r2,t6(r0)
           lw r3,b(r0)
           mul r1,r2,r3
           sw t7(r0),r1
           % processing: t8 := t7 + c
           lw r3,t7(r0)
           lw r2,c(r0)
           add r1,r3,r2
           sw t8(r0),r1
           % processing: a := t8
           lw r1,t8(r0)
           sw a(r0),r1
           % processing: t9 := 4
           addi r1,r0,4
           sw t9(r0),r1
           % processing: t10 := a + t9
           lw r2,a(r0)
           lw r3,t9(r0)
           add r1,r2,r3
           sw t10(r0),r1
           % processing: put(t10)
           lw r1,t10(r0)
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

flink      res 4
freturn    res 4
           % space for variable p1
p1         res 4
           % space for variable p2
p2         res 4
           % space for variable p3
p3         res 4
           % space for variable retval
retval     res 4
           % space for p2 * p3
t1         res 4
           % space for p1 + t1
t2         res 4
           % space for variable a
a          res 4
           % space for variable b
b          res 4
           % space for variable c
c          res 4
           % space for constant 1
t3         res 4
           % space for constant 2
t4         res 4
           % space for constant 3
t5         res 4
           % space for function call expression factor
t6         res 4
           % space for t6 * b
t7         res 4
           % space for t7 + c
t8         res 4
           % space for constant 4
t9         res 4
           % space for a + t9
t10        res 4
           % buffer space used for console output
buf        res 20

