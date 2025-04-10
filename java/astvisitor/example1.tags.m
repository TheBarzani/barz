           % processing function definition: 
func1                sw func1link(r0),r15
           % processing: t1 := b1 * c1
           lw r2,b1(r0)
           lw r3,c1(r0)
           mul r1,r2,r3
           sw t1(r0),r1
           % processing: t2 := a1 + t1
           lw r3,a1(r0)
           lw r2,t1(r0)
           add r1,r3,r2
           sw t2(r0),r1
           % processing: a1 := t2
           lw r1,t2(r0)
           sw a1(r0),r1
           lw r15,func1link(r0)
           jr r15
           % processing function definition: 
func2                sw func2link(r0),r15
           % processing: t3 := b2 * c2
           lw r2,b2(r0)
           lw r3,c2(r0)
           mul r1,r2,r3
           sw t3(r0),r1
           % processing: t4 := a2 + t3
           lw r3,a2(r0)
           lw r2,t3(r0)
           add r1,r3,r2
           sw t4(r0),r1
           % processing: x2 := t4
           lw r1,t4(r0)
           sw x2(r0),r1
           % processing: t5 := z3 * y3
           lw r2,z3(r0)
           lw r3,y3(r0)
           mul r1,r2,r3
           sw t5(r0),r1
           % processing: t6 := x3 + t5
           lw r3,x3(r0)
           lw r2,t5(r0)
           add r1,r3,r2
           sw t6(r0),r1
           % processing: a3 := t6
           lw r1,t6(r0)
           sw a3(r0),r1
           lw r15,func2link(r0)
           jr r15
           entry
           addi r14,r0,topaddr
           % processing: t7 := b4 * c4
           lw r2,b4(r0)
           lw r3,c4(r0)
           mul r1,r2,r3
           sw t7(r0),r1
           % processing: t8 := a4 + t7
           lw r3,a4(r0)
           lw r2,t7(r0)
           add r1,r3,r2
           sw t8(r0),r1
           % processing: a4 := t8
           lw r1,t8(r0)
           sw a4(r0),r1
           % processing: t9 := b5 * c5
           lw r2,b5(r0)
           lw r3,c5(r0)
           mul r1,r2,r3
           sw t9(r0),r1
           % processing: t10 := a5 + t9
           lw r3,a5(r0)
           lw r2,t9(r0)
           add r1,r3,r2
           sw t10(r0),r1
           % processing: x5 := t10
           lw r1,t10(r0)
           sw x5(r0),r1
           % processing: t11 := z6 * y6
           lw r2,z6(r0)
           lw r3,y6(r0)
           mul r1,r2,r3
           sw t11(r0),r1
           % processing: t12 := x6 + t11
           lw r3,x6(r0)
           lw r2,t11(r0)
           add r1,r3,r2
           sw t12(r0),r1
           % processing: a6 := t12
           lw r1,t12(r0)
           sw a6(r0),r1
           hlt

float1     res 4
           % space for variable int1
int1       res 4
           % space for variable int532
int532     res 4
float10    res 4
float100   res 4
float2     res 4
           % space for variable int2
int2       res 4
func1link  res 4
func1returnres 4
           % space for variable int235
int235     res 4
float4     res 4
float7     res 4
           % space for b1 * c1
t1         res 4
           % space for a1 + t1
t2         res 4
func2link  res 4
func2returnres 4
float102   res 4
float6     res 4
           % space for variable int421
int421     res 4
float11    res 4
           % space for b2 * c2
t3         res 4
           % space for a2 + t3
t4         res 4
           % space for z3 * y3
t5         res 4
           % space for x3 + t5
t6         res 4
float101   res 4
float4     res 4
           % space for variable int4
int4       res 4
float3     res 4
           % space for variable int3
int3       res 4
           % space for b4 * c4
t7         res 4
           % space for a4 + t7
t8         res 4
           % space for b5 * c5
t9         res 4
           % space for a5 + t9
t10        res 4
           % space for z6 * y6
t11        res 4
           % space for x6 + t11
t12        res 4
           % buffer space used for console output
buf        res 20

