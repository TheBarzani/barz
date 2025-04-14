% Code Section

          % Processing function: main
          % Entering function scope: main
main      entry
          addi r14,r0,topaddr
          % processing: t1 := 100
          addi r1,r0,100
          sw -528(r14),r1
          lw r1,-528(r14)
          % Processing dot access expression
          % Processing dot access expression
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r2,r14,524
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t2 := 3
          addi r4,r0,3
          sw -532(r14),r4
          lw r4,-532(r14)
          muli r5,r4,4
          add r3,r3,r5
          % Array byte offset calculated
          sub r5,r2,r3
          sw -536(r14),r5
          % Array access address stored in -536(r14)
          % Warning: Using direct offset metadata
          lw r5,-536(r14)
          subi r2,r5,0
          sw -540(r14),r2
          % Member 'b' address stored at offset -540
          lw r2,-540(r14)
          subi r5,r2,-8
          sw -544(r14),r5
          % Member 'c' address stored at offset -544
          lw r5,-544(r14)
          % Storing value into class member
          sw 0(r5),r1
          % Assignment completed
          % Processing dot access expression
          % Processing dot access expression
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r1,r14,524
          % Calculating array byte offset
          addi r5,r0,0
          % processing: t6 := 3
          addi r2,r0,3
          sw -548(r14),r2
          lw r2,-548(r14)
          muli r3,r2,4
          add r5,r5,r3
          % Array byte offset calculated
          sub r3,r1,r5
          sw -552(r14),r3
          % Array access address stored in -552(r14)
          % Warning: Using direct offset metadata
          lw r3,-552(r14)
          subi r1,r3,0
          sw -556(r14),r1
          % Member 'b' address stored at offset -556
          lw r1,-556(r14)
          subi r3,r1,-8
          sw -560(r14),r3
          % Member 'c' address stored at offset -560
          lw r5,-560(r14)
          lw r3,0(r5)
          % processing: write(t9)
          % put value on stack
          addi r14,r14,-916
          sw -8(r14),r3
          % link buffer to stack
          addi r3,r0,buf
          sw -12(r14),r3
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-916
          % processing: t10 := 89
          addi r1,r0,89
          sw -564(r14),r1
          lw r1,-564(r14)
          % Processing dot access expression
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r3,r14,524
          % Calculating array byte offset
          addi r5,r0,0
          % processing: t11 := 1
          addi r2,r0,1
          sw -568(r14),r2
          lw r2,-568(r14)
          muli r4,r2,4
          add r5,r5,r4
          % Array byte offset calculated
          sub r4,r3,r5
          sw -572(r14),r4
          % Array access address stored in -572(r14)
          % Warning: Using direct offset metadata
          lw r4,-572(r14)
          subi r3,r4,-20
          sw -576(r14),r3
          % Member 'a' address stored at offset -576
          lw r3,-576(r14)
          % Storing value into class member
          sw 0(r3),r1
          % Assignment completed
          % Processing dot access expression
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r1,r14,524
          % Calculating array byte offset
          addi r3,r0,0
          % processing: t14 := 1
          addi r4,r0,1
          sw -580(r14),r4
          lw r4,-580(r14)
          muli r5,r4,4
          add r3,r3,r5
          % Array byte offset calculated
          sub r5,r1,r3
          sw -584(r14),r5
          % Array access address stored in -584(r14)
          % Warning: Using direct offset metadata
          lw r5,-584(r14)
          subi r1,r5,-20
          sw -588(r14),r1
          % Member 'a' address stored at offset -588
          % processing: t17 := 1
          addi r1,r0,1
          sw -592(r14),r1
          % processing: t18 := t16 + t17
          lw r1,-588(r14)
          lw r1,0(r1)
          lw r5,-592(r14)
          add r3,r1,r5
          sw -596(r14),r3
          % processing: write(t18)
          lw r3,-596(r14)
          % put value on stack
          addi r14,r14,-916
          sw -8(r14),r3
          % link buffer to stack
          addi r3,r0,buf
          sw -12(r14),r3
          % convert int to string for output
          jl r15,intstr
          sw -8(r14),r13
          % output to console
          jl r15,putstr
          jl r15,putnl
          subi r14,r14,-916
          % processing: t19 := 25
          addi r5,r0,25
          sw -840(r14),r5
          lw r5,-840(r14)
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r3,r14,836
          % Calculating array byte offset
          addi r1,r0,0
          % processing: t20 := 1
          addi r4,r0,1
          sw -844(r14),r4
          lw r4,-844(r14)
          muli r2,r4,80
          add r1,r1,r2
          % processing: t21 := 2
          addi r2,r0,2
          sw -848(r14),r2
          lw r2,-848(r14)
          muli r4,r2,20
          add r1,r1,r4
          % processing: t22 := 3
          addi r4,r0,3
          sw -852(r14),r4
          lw r4,-852(r14)
          muli r2,r4,4
          add r1,r1,r2
          % Array byte offset calculated
          sub r2,r3,r1
          sw -856(r14),r2
          % Array access address stored in -856(r14)
          % Storing value into array element
          add r2,r0,r0
          lw r2,-856(r14)
          sw 0(r2),r5
          % Assignment completed
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r5,r14,836
          % Calculating array byte offset
          addi r2,r0,0
          % processing: t24 := 1
          addi r3,r0,1
          sw -860(r14),r3
          lw r3,-860(r14)
          muli r1,r3,80
          add r2,r2,r1
          % processing: t25 := 2
          addi r1,r0,2
          sw -864(r14),r1
          lw r1,-864(r14)
          muli r3,r1,20
          add r2,r2,r3
          % processing: t26 := 3
          addi r3,r0,3
          sw -868(r14),r3
          lw r3,-868(r14)
          muli r1,r3,4
          add r2,r2,r1
          % Array byte offset calculated
          sub r1,r5,r2
          sw -872(r14),r1
          % Array access address stored in -872(r14)
          % Warning: Using direct offset metadata
          lw r2,-872(r14)
          lw r1,0(r2)
          % processing: write()
          % put value on stack
          addi r14,r14,-916
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
          subi r14,r14,-916
          % Processing array access
          % Static array - calculating address from frame pointer
          subi r5,r14,836
          % Calculating array byte offset
          addi r1,r0,0
          % processing: t28 := 1
          addi r2,r0,1
          sw -880(r14),r2
          lw r2,-880(r14)
          muli r3,r2,80
          add r1,r1,r3
          % processing: t29 := 2
          addi r3,r0,2
          sw -884(r14),r3
          lw r3,-884(r14)
          muli r2,r3,20
          add r1,r1,r2
          % processing: t30 := 3
          addi r2,r0,3
          sw -888(r14),r2
          lw r2,-888(r14)
          muli r3,r2,4
          add r1,r1,r3
          % Array byte offset calculated
          sub r3,r5,r1
          sw -892(r14),r3
          % Array access address stored in -892(r14)
          % processing: t32 := 2
          addi r3,r0,2
          sw -896(r14),r3
          % Warning: Using direct offset metadata
          lw r3,-892(r14)
          lw r3,0(r3)
          lw r5,-896(r14)
          % processing: t33 :=  * t32
          mul r1,r3,r5
          sw -900(r14),r1
          % processing: t34 := 5
          addi r1,r0,5
          sw -904(r14),r1
          lw r1,-900(r14)
          lw r5,-904(r14)
          % processing: t35 := t33 / t34
          div r3,r1,r5
          sw -908(r14),r3
          % processing: t36 := 1
          addi r3,r0,1
          sw -912(r14),r3
          % processing: t37 := t35 + t36
          lw r3,-908(r14)
          lw r5,-912(r14)
          add r1,r3,r5
          sw -916(r14),r1
          lw r1,-916(r14)
          % processing:: y := t37
          sw -876(r14),r1
          % Assignment completed
          % processing: write(y)
          lw r1,-876(r14)
          % put value on stack
          addi r14,r14,-916
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
          subi r14,r14,-916
          % End of program
          hlt

% Data Section
          align
% Buffer space for console I/O
buf       res 20

