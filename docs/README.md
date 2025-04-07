# BARZ SPECIFICATIONS

Files:

1. Assigment Statement

   `assignment2.COMP442-6421.paquet.2025.4.pdf`

2. Grammar (same as stated in the assignment statement)

   `COMP442.grammar.BNF.grm`

3. Executable jar file of the grammar transformation tool developed by Dr. Paquet

   `grammartool.jar`

   Usage: 

   ```bash
   java -jar grammartool.jar <inputfilename.grm> <outputfilename.grm> [-opt] [-rept0] [-leftrec] [-verbose] [-first] [-follow] [-atocc] [-ucalgary]
   
   first argument (mandatory): inputfilename.grm
   second argument (mandatory): outputfilename.grm
   
   options:
   -opt: remove all optionality EBNF constructs, e.g.: <A1> ::= 'a1' [[ 'a2' ]] 'a3'
   -rept0: remove all zero-or-more repetition EBNF constructs, e.g.: <A1> ::= 'a1' {{ 'a2' }} 'a3'
   -leftrec: remove all left-recursive constructs, e.g.: <A1> ::= <A1> 'a1'
   -first: output FIRST set to .grm.first file
   -follow: output FOLLOW set to .grm.follow file
   -atocc: output the grammar to .grm.atocc file
   -ucalgary: output the grammar to .grm.ucalgary file
4. Example Programs

   ```bash
   bubblesort.src
   polynomial.src
5. Partial solution for the transformed grammar (only for expressions)
   ```bash
   COMP442.grammar.expressions.atocc.grm

## Some Changes to the Grammer
- `'==' ::= 'eq'`
- `'>' ::= 'gt'`
- `'<' ::= 'lt'`
- `':' ::= 'colon'`
- `'=>' ::= 'arrow'`
- `':=' ::= 'assign'`


## UCalgary Ambiguity
Below are the UCalgary Tool's vital statistics of the grammar after using the `grammartool.jar`:

The grammar is not LL(1) because:

- ARRAYSIZE has a first set conflict
- EXPR has a first set conflict.
- FACTOR has a first set conflict.
- IDNEST has a first set conflict.
- REPTFUNCTIONCALL0 is nullable with clashing first and follow sets.
- REPTVARIABLE0 is nullable with clashing first and follow sets.
- STATEMENT has a first set conflict. 

### Changes
- ARRAYSIZE has a first set conflict
```
[Factorization]
ARRAYSIZE -> lsqbr intnum rsqbr  . 
ARRAYSIZE -> lsqbr rsqbr  . 
=>
ARRAYSIZE -> lsqbr ARRAYSIZE2  .

ARRAYSIZE2 -> intlit rsqbr .
ARRAYSIZE2 -> rsqbr .
```

- ARRAYSIZE has a first set conflict
```
[Elimination]
EXPR -> ARITHEXPR  . 
EXPR -> RELEXPR  . 
=>
EXPR -> ARITHEXPR EXPR2 .

EXPR2 -> RELOP ARITHEXPR .
EXPR2 ->  .
```

- FACTOR has a first set conflict.
```
[Elimination]
FACTOR -> VARIABLE  . 
FACTOR -> FUNCTIONCALL  . 
FACTOR -> intlit  . 
FACTOR -> floatlit  . 
FACTOR -> lpar ARITHEXPR rpar  . 
FACTOR -> not FACTOR  . 
FACTOR -> SIGN FACTOR  . 
=>
FACTOR -> IDORSELF FACTOR2 REPTVARIABLEORFUNCTIONCALL .  
FACTOR -> intlit .  
FACTOR -> floatlit .  
FACTOR -> lpar ARITHEXPR rpar .  
FACTOR -> not FACTOR .  
FACTOR -> SIGN FACTOR .  

FACTOR2 -> lpar APARAMS rpar .  
FACTOR2 -> REPTIDNEST1 .   

REPTVARIABLEORFUNCTIONCALL -> IDNEST REPTVARIABLEORFUNCTIONCALL .  
REPTVARIABLEORFUNCTIONCALL ->  .  

```

- IDNEST has a first set conflict.
```
[Factorization]
IDNEST -> IDORSELF REPTIDNEST1 dot  . 
IDNEST -> IDORSELF lpar APARAMS rpar dot  . 

IDORSELF -> id  . 
IDORSELF -> self  . 
=>
IDNEST -> dot id IDNEST2  .
IDNEST2 -> lpar APARAMS rpar  .
IDNEST2 -> REPTIDNEST1  .
```

- STATEMENT has a first set conflict. 
```[Elimination and Factorization]
STATEMENT -> ASSIGNSTAT semi  .
STATEMENT -> FUNCTIONCALL semi  .  
STATEMENT -> if lpar RELEXPR rpar then STATBLOCK else STATBLOCK semi  . 
STATEMENT -> while lpar RELEXPR rpar STATBLOCK semi  . 
STATEMENT -> read lpar VARIABLE rpar semi  . 
STATEMENT -> write lpar EXPR rpar semi  . 
STATEMENT -> return lpar EXPR rpar semi  . 
=>
STATEMENT -> IDORSELF SELECTORLIST0 STATEMENTSUFFIX0 .
STATEMENT -> if lpar RELEXPR rpar then STATBLOCK else STATBLOCK semi  . 
STATEMENT -> while lpar RELEXPR rpar STATBLOCK semi  . 
STATEMENT -> read lpar VARIABLE rpar semi  . 
STATEMENT -> write lpar EXPR rpar semi  . 
STATEMENT -> return lpar EXPR rpar semi  . 

SELECTORLIST0 -> TYPESELECTOR SELECTORLIST0 . 
SELECTORLIST0 ->  .

TYPESELECTOR -> lsqbr ARITHEXPR rsqbr  .
TYPESELECTOR -> dot id  .

STATEMENTSUFFIX0 -> lpar APARAMS rpar semi  .
STATEMENTSUFFIX0 -> assign EXPR semi  .

[Removed] ASSIGNSTAT -> VARIABLE ASSIGNOP EXPR  . 
[Removed] ASSIGNOP -> assign  . 
[Removed] FUNCTIONCALL -> REPTFUNCTIONCALL0 id lpar APARAMS rpar  . 
[Removed] REPTFUNCTIONCALL0 -> IDNEST REPTFUNCTIONCALL0  . 
          REPTFUNCTIONCALL0 ->  . 


```
*The grammar is now an LL(1) grammar.*

`NOTE: In the partial solution, the 'self' keyword is completely eliminated. However, I kept it in my grammar for adhering to the original grammar. In the original grammar calling 'self' on 'self' keyword is allowed, however I changed this as it does not have a meaning without eliminating the self keyword.` 

# More Specs
Files:

1. assigment statement

   assignment3.COMP442-6421.paquet.2025.4.pdf
  
2. example outast output

   examples/example1.source                 source file (warning: not is the same language as we have now)
   examples/example1.ast.outast             text-readable output  
   examples/example1.dot.outast             graphviz/dot-readable output

   examples/example2.source                 source file (warning: not is the same language as we have now)
   examples/example2.ast.outast             text-readable output  
   examples/example2.dot.outast             graphviz/dot-readable output

   examples/example3.source                 source file (warning: not is the same language as we have now)
   examples/example3.ast.outast             text-readable output  
   examples/example3.dot.outast             graphviz/dot-readable output

3. example source files

   source files/bubblesort.src
   source files/polynomial.src

4. LL(1) grammar

   grammar.LL1.grm


# More Specs
Files:

1. assigment statement

   assignment4.COMP442-6421.paquet.2025.4.pdf
   
3. example files

   bubblesort.src                  semantically valid program
   bubblesort.outsymboltables      symbol table output for bubblesort.src
   polynomial.src                  semantically valid program
   polynomial.outsymboltables      symbol table output for polynomial.src
   polynomialsemanticerrors.src    semantically invalid program

NOTE:
1. ASTNodes should have a new data member of SymbolTable
2. Multiple phases needed, maybe one for symbol table and another for semantic checking
3. Semantic actions have to be grouped into traversal passes/phases, AND still be mapped to
specific nodes. 

# code gen specs
Files:

1. assigment statement

   assignment5.COMP442-6421.paquet.2025.4.pdf

2. example programs: sample programs that demonstrate how your test cases should be documented. 
   Note that these are not to be considered as complete in terms of test case coverage.  

   example-bubblesort.src 
   example-polynomial.src
   example-simplemain.src

3. moon virtual processor: implementation code, documentation and sample code

   moon/
   
NOTE: 
1. Use a phase to determine the memory allocation requirements (updates the symbol table)
- Allocate memory for primitive types and then allocate for objects
2. Use another phase for code generation
- Do global/free functions before member functions
- Implement the green ones first
- Look into multi-dimensional arrays
- Check semantic errors when out of bound indexes are used
- Use temporary results memory allocation using a unique label generator
- Use register pool allocation strategy 
- Go back into the lexical specifications for making sure that everything is implemented.