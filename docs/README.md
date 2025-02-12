# BARZ SPECIFICATIONS

Files:

1. Assigment Statement

   `assignment2.COMP442-6421.paquet.2025.4.pdf`

2. grammar (same as stated in the assignment statement)

   COMP442.grammar.BNF.grm

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
5. partial solution for the transformed grammar (only for expressions)
   ```bash
   COMP442.grammar.expressions.atocc.grm
