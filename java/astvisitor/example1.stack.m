          % processing function definition: 
func1     sw -4(r14),r15
          % processing: t1 := b1 * c1
          lw r2,0(r14)
          lw r3,0(r14)
          mul r4,r2,r3
          sw -220(r14),r4
          % processing: t2 := a1 + t1
          lw r3,0(r14)
          lw r2,-220(r14)
          add r1,r3,r2
          sw -224(r14),r1
          % processing: a1 := t2
          lw r2,-224(r14)
          sw 0(r14),r2
          lw r15,-4(r14)
          jr r15
          % processing function definition: 
func2     sw -4(r14),r15
          % processing: t3 := b2 * c2
          lw r2,0(r14)
          lw r1,0(r14)
          mul r4,r2,r1
          sw -960(r14),r4
          % processing: t4 := a2 + t3
          lw r1,0(r14)
          lw r2,-960(r14)
          add r3,r1,r2
          sw -964(r14),r3
          % processing: x2 := t4
          lw r2,-964(r14)
          sw 0(r14),r2
          % processing: t5 := z3 * y3
          lw r2,0(r14)
          lw r3,0(r14)
          mul r4,r2,r3
          sw -964(r14),r4
          % processing: t6 := x3 + t5
          lw r3,0(r14)
          lw r2,-964(r14)
          add r1,r3,r2
          sw -964(r14),r1
          % processing: a3 := t6
          lw r2,-964(r14)
          sw 0(r14),r2
          lw r15,-4(r14)
          jr r15
          entry
          addi r14,r0,topaddr
          % processing: t7 := b4 * c4
          lw r2,0(r14)
          lw r1,0(r14)
          mul r4,r2,r1
          sw -836(r14),r4
          % processing: t8 := a4 + t7
          lw r1,0(r14)
          lw r2,-836(r14)
          add r3,r1,r2
          sw -840(r14),r3
          % processing: a4 := t8
          lw r2,-840(r14)
          sw 0(r14),r2
          % processing: t9 := b5 * c5
          lw r2,0(r14)
          lw r3,0(r14)
          mul r4,r2,r3
          sw -844(r14),r4
          % processing: t10 := a5 + t9
          lw r3,0(r14)
          lw r2,-844(r14)
          add r1,r3,r2
          sw -848(r14),r1
          % processing: x5 := t10
          lw r2,-848(r14)
          sw 0(r14),r2
          % processing: t11 := z6 * y6
          lw r2,0(r14)
          lw r1,0(r14)
          mul r4,r2,r1
          sw -852(r14),r4
          % processing: t12 := x6 + t11
          lw r1,0(r14)
          lw r2,-852(r14)
          add r3,r1,r2
          sw -856(r14),r3
          % processing: a6 := t12
          lw r2,-856(r14)
          sw 0(r14),r2
          hlt

          % buffer space used for console output
buf       res 20

