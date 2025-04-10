//Driver that manually creates an AST, then calls different visitors on it

import java.util.Arrays;

import AST.*;
import Visitors.AST.ASTDotPrinterVisitor;
import Visitors.AST.ASTTextPrinterVisitor;
import Visitors.AST.ReconstructSourceProgramVisitor;
import Visitors.CodeGeneration.*;
import Visitors.SemanticChecking.*;
import Visitors.SymbolTable.*;

public class VisitorDriver {
	
	public static void main(String[] args){
		
		/*************************************************************
		 * EXAMPLE 1
		 *************************************************************/
		
		/**
		 * class class1{
		 *   float float1;
		 *   int int1;
		 *  }
		 *  		
		 * class class2{
		 *   int in532[5][3][2];
		 *   float float10[10];
		 *   float float100[100];
		 *   float float1;
		 *   int int1;
		 * }
		 * 
		 * 		
		 * int func1(int int532[5][3][2], float float100[100]){
		 *   float float100;
		 *   a=a+b*c;
		 * }
		 * 
		 * float func2(float float1, int int1){
		 *   int int532[5][3][2]; 
		 *   float float100[100];
		 *   a=a+b*c;
		 *   x=a+b*c;
		 *   
		 * program{
		 *   int in532[5][3][2];
		 *   float float10[10];
		 *   float float100[100];
		 *   float float1;
		 *   int int1;
		 *   a=a+b*c;
		 *   x=a+b*c;
		 *   a=x+z*y
		 *  }
		 * 
		 */	
		// assignment statement subtree for a=a+b*c 
		Node a1           = new IdNode("a1", "int");
		Node aa1           = new IdNode("a1", "int");		
		Node b1           = new IdNode("b1", "int");
		Node c1           = new IdNode("c1", "int");
		Node times1       = new MultOpNode("*1", b1, c1);
		Node plus1        = new AddOpNode("+", a1, times1); 
		Node stat1        = new AssignStatNode(aa1,plus1); 

		// assignment statement subtree for x=a+b*c 
		Node x2           = new IdNode("x2", "float");
		Node a2           = new IdNode("a2", "int");
		Node b2           = new IdNode("b2", "int");
		Node c2           = new IdNode("c2", "int");
		Node times2       = new MultOpNode("*", b2, c2);
		Node plus2        = new AddOpNode("+", a2, times2); 
		Node stat2        = new AssignStatNode(x2,plus2); 

		// assignment statement subtree for a=x+z*y
		Node a3          = new IdNode("a3", "int");
		Node x3          = new IdNode("x3", "int");
		Node y3          = new IdNode("y3", "int");
		Node z3          = new IdNode("z3", "float");
		Node times3      = new MultOpNode("*", z3, y3);
		Node plus3       = new AddOpNode("+", x3, times3);
		Node stat3       = new AssignStatNode(a3,plus3); 		
		
		// assignment statement subtree for a=a+b*c 
		Node aa4          = new IdNode("a4", "int");		
		Node a4           = new IdNode("a4", "int");
		Node b4           = new IdNode("b4", "int");
		Node c4           = new IdNode("c4", "int");
		Node times4       = new MultOpNode("*", b4, c4);
		Node plus4        = new AddOpNode("+", a4, times4); 
		Node stat4        = new AssignStatNode(aa4,plus4); 

		// assignment statement subtree for x=a+b*c 
		Node x5           = new IdNode("x5", "int");
		Node a5           = new IdNode("a5", "int");
		Node b5           = new IdNode("b5", "int");
		Node c5           = new IdNode("c5", "int");
		Node times5       = new MultOpNode("*", b5, c5);
		Node plus5        = new AddOpNode("+", a5, times5); 
		Node stat5        = new AssignStatNode(x5,plus5); 

		// assignment statement subtree for a=x+z*y
		Node a6          = new IdNode("a6", "int");
		Node x6          = new IdNode("x6", "int");
		Node y6          = new IdNode("y6", "int");
		Node z6          = new IdNode("z6", "int");
		Node times6      = new MultOpNode("*", z6, y6);
		Node plus6       = new AddOpNode("+", x6, times6);
		Node stat6       = new AssignStatNode(a6,plus6); 
		
		// variable declaration subtree for int a[5][3][2]
		Node type1        = new TypeNode("int");
		Node id1          = new IdNode("int532");
		Node d11          = new DimNode("5");
		Node d21          = new DimNode("3");
		Node d31          = new DimNode("2");
		Node dimlist1     = new DimListNode(Arrays.asList(d11,d21,d31));
		Node vd1          = new VarDeclNode(type1, id1, dimlist1);

		// variable declaration subtree for float var1[10]
		Node type2       = new TypeNode("float");
		Node id2         = new IdNode("float10");
		Node d12         = new DimNode("10");
		Node dimlist2    = new DimListNode(Arrays.asList(d12));
		Node vd2         = new VarDeclNode(type2, id2, dimlist2);

		// variable declaration subtree for a float f[100]
		Node type3       = new TypeNode("float");
		Node id3         = new IdNode("float100");
		Node d13         = new DimNode("100");
		Node dimlist3    = new DimListNode(Arrays.asList(d13));
		Node vd3         = new VarDeclNode(type3, id3, dimlist3);
		
		// variable declaration subtree for a float float1
		Node type4       = new TypeNode("float");
		Node id4         = new IdNode("float1");	
		Node dimlist4    = new DimListNode();	
		Node vd4         = new VarDeclNode(type4, id4, dimlist4);

		// variable declaration subtree for a int int1
		Node type5       = new TypeNode("int");
		Node id5         = new IdNode("int1");	
		Node dimlist5    = new DimListNode();	
		Node vd5         = new VarDeclNode(type5, id5, dimlist5);

		// variable declaration subtree for a float float1
		Node type6       = new TypeNode("float");
		Node id6         = new IdNode("float2");	
		Node dimlist6    = new DimListNode();	
		Node vd6         = new VarDeclNode(type6, id6, dimlist6);

		// variable declaration subtree for a int int1
		Node type7       = new TypeNode("int");
		Node id7         = new IdNode("int2");	
		Node dimlist7    = new DimListNode();	
		Node vd7         = new VarDeclNode(type7, id7, dimlist7);
		
		// variable declaration subtree for int a[5][3][2]
		Node type8        = new TypeNode("int");
		Node id8          = new IdNode("int421");
		Node d18          = new DimNode("4");
		Node d28          = new DimNode("2");
		Node d38          = new DimNode("1");
		Node dimlist8     = new DimListNode(Arrays.asList(d18,d28,d38));
		Node vd8          = new VarDeclNode(type8, id8, dimlist8);

		// variable declaration subtree for float var1[10]
		Node type9       = new TypeNode("float");
		Node id9         = new IdNode("float11");
		Node d19         = new DimNode("11");
		Node dimlist9    = new DimListNode(Arrays.asList(d19));
		Node vd9         = new VarDeclNode(type9, id9, dimlist9);

		// variable declaration subtree for a float f[100]
		Node type10       = new TypeNode("float");
		Node id10         = new IdNode("float101");
		Node d110         = new DimNode("101");
		Node dimlist10    = new DimListNode(Arrays.asList(d110));
		Node vd10         = new VarDeclNode(type10, id10, dimlist10);
		
		// variable declaration subtree for a float float1
		Node type11       = new TypeNode("float");
		Node id11         = new IdNode("float4");	
		Node dimlist11    = new DimListNode();	
		Node vd11         = new VarDeclNode(type11, id11, dimlist11);

		// variable declaration subtree for a int int1
		Node type12       = new TypeNode("int");
		Node id12         = new IdNode("int4");	
		Node dimlist12    = new DimListNode();	
		Node vd12         = new VarDeclNode(type12, id12, dimlist12);

		// variable declaration subtree for a float float1
		Node type13       = new TypeNode("float");
		Node id13         = new IdNode("float3");	
		Node dimlist13    = new DimListNode();	
		Node vd13         = new VarDeclNode(type13, id13, dimlist13);

		// variable declaration subtree for a int int1
		Node type14       = new TypeNode("int");
		Node id14         = new IdNode("int3");	
		Node dimlist14    = new DimListNode();	
		Node vd14         = new VarDeclNode(type14, id14, dimlist14);
		
		// variable declaration subtree for int a[5][3][2]
		Node type15        = new TypeNode("int");
		Node id15          = new IdNode("int235");
		Node d115          = new DimNode("2");
		Node d215          = new DimNode("3");
		Node d315          = new DimNode("5");
		Node dimlist15     = new DimListNode(Arrays.asList(d115,d215,d315));
		Node vd15          = new VarDeclNode(type15, id15, dimlist15);

		// variable declaration subtree for float var1[10]
		Node type16       = new TypeNode("float");
		Node id16         = new IdNode("float4");
		Node d116         = new DimNode("10");
		Node dimlist16    = new DimListNode(Arrays.asList(d116));
		Node vd16         = new VarDeclNode(type16, id16, dimlist16);

		// variable declaration subtree for a float f[100]
		Node type17       = new TypeNode("float");
		Node id17         = new IdNode("float102");
		Node d117         = new DimNode("102");
		Node dimlist17    = new DimListNode(Arrays.asList(d117));
		Node vd17         = new VarDeclNode(type17, id17, dimlist17);
		
		// variable declaration subtree for a float float1
		Node type18       = new TypeNode("float");
		Node id18         = new IdNode("float6");	
		Node dimlist18    = new DimListNode();	
		Node vd18         = new VarDeclNode(type18, id18, dimlist18);

		// variable declaration subtree for a float float1
		Node type19       = new TypeNode("float");
		Node id19         = new IdNode("float7");	
		Node dimlist19    = new DimListNode();	
		Node vd19         = new VarDeclNode(type19, id19, dimlist19);

		/**
		 * class class1{
		 *   float float1;
		 *   int int1;
		 *   }
		 */
		Node class1name     = new IdNode("class1");
		Node class1         = new ClassNode(class1name, Arrays.asList(vd4,vd5));

		/**
		 * class class2{
		 *   int int532[5][3][2];
		 *   float float10[10];
		 *   float float100[100];
		 *   float float1;
		 *   int int1;
		 */
		Node class2name  = new IdNode("class2");
		Node class2      = new ClassNode(class2name, Arrays.asList(vd1,vd2,vd3,vd6,vd7));
		
		// statement block containing vd2, stat1
		Node statblock1  = new StatBlockNode(Arrays.asList(vd19,stat1));
		
		// statement block containing vd1, vd3, stat1, stat2
		Node statblock2  = new StatBlockNode(Arrays.asList(vd8,vd9,stat2,stat3));
		
		/** program{
		 *   int in532[5][3][2];
		 *   float float10[10];
		 *   float float100[100];
		 *   float float1;
		 *   int int1;
		 *   a=a+b*c;
		 *   x=a+b*c;
		 *   a=x+z*y
		 *  }
		 * 
		 */	
		Node progblock1  = new ProgramBlockNode(Arrays.asList(vd10,vd11,vd12,vd13,vd14,stat4,stat5,stat6));
			
		/**
		 * int func1(int int235[2][3][5], float float4){
		 *   float float10;
		 *   a=a+b*c;
		 * }
		 */
		Node f1type      = new TypeNode("int");
		Node f1			 = new IdNode("func1");
		Node pl			 = new ParamListNode(Arrays.asList(vd15, vd16));
		Node fd1		 = new FuncDefNode(f1type, f1, pl, statblock1);
		
		/**
		 * float func2(float float1, int int1){
		 *   int int532[5][3][2]; 
		 *   float float100[100];
		 *   a=a+b*c;
		 *   x=a+b*c;
		 */
		Node f2type      = new TypeNode("float");		
		Node f2			 = new IdNode("func2");
		Node pl2		 = new ParamListNode(Arrays.asList(vd17, vd18));
		Node fd2		 = new FuncDefNode(f2type, f2, pl2, statblock2);

		Node classlist   = new ClassListNode(Arrays.asList(class1, class2));
		Node funcdeflist = new FuncDefListNode(Arrays.asList(fd1, fd2));
		Node prog        = new ProgNode(classlist, funcdeflist, progblock1);
		
		ASTTextPrinterVisitor           ASTTPVisitor = new ASTTextPrinterVisitor("example1.ast");
		ASTDotPrinterVisitor            ASTDPVisitor = new ASTDotPrinterVisitor("example1.dot");		
		ReconstructSourceProgramVisitor CSVisitor    = new ReconstructSourceProgramVisitor("example1.source"); 
		TypeCheckingVisitor             TCVisitor    = new TypeCheckingVisitor("example1.errors"); 
		SymTabCreationVisitor           STCVisitor   = new SymTabCreationVisitor(); 
		ComputeMemSizeVisitor           CMSVisitor   = new ComputeMemSizeVisitor("example1.symtab");
		TagsBasedCodeGenerationVisitor  CGVisitor    = new TagsBasedCodeGenerationVisitor("example1.tags.m");
		StackBasedCodeGenerationVisitor CGSVisitor   = new StackBasedCodeGenerationVisitor("example1.stack.m");
	
		prog.accept(CSVisitor);		
		prog.accept(TCVisitor);
		prog.accept(ASTTPVisitor);
		prog.accept(ASTDPVisitor);
		prog.accept(STCVisitor);
		prog.accept(CMSVisitor);
		prog.accept(CGVisitor);
		prog.accept(CGSVisitor);
		
		//prog.print();
	
		/*************************************************************
		 * EXAMPLE 2
		 *************************************************************/
		
		/**
		 * program that is composed of an empty class list, 
		 * an empty function definition list, 
		 * and a program statement block with three variables, an assignment statement and a put statement	
		 *
		 *    program{
		 *	    int a;
		 *      int b;
	     *	    int c;
         *	    a = 1;
         *      put(a);
         *	    b = 2;
         *      put(b);
         *	    c = 3;
         *      put(c);
         *	    a = a + b * c;
         *	    put(a + 3);
         *    } // result = 10
         * 
		 */
		
		Node typea        = new TypeNode("int");
		Node typeb        = new TypeNode("int");
		Node typec        = new TypeNode("int");
		
		Node ida          = new IdNode("a", "int");
		Node ida1         = new IdNode("a", "int");
		Node idb          = new IdNode("b", "int");
		Node idb1         = new IdNode("b", "int");
		Node idc          = new IdNode("c", "int");
		Node idc1         = new IdNode("c", "int");

		Node vda          = new VarDeclNode(typea, ida);
		Node vdb          = new VarDeclNode(typeb, idb);
		Node vdc          = new VarDeclNode(typec, idc);

		Node n1           = new NumNode("1", "int");
		Node n2           = new NumNode("2", "int");
		Node n3           = new NumNode("3", "int");
		Node n4           = new NumNode("6", "int");
		
		Node stata        = new AssignStatNode(ida,n1); 
		Node statb        = new AssignStatNode(idb,n2); 
		Node statc        = new AssignStatNode(idc,n3); 

		Node aa           = new IdNode("a", "int");
		Node ap           = new IdNode("a", "int");
		Node bp           = new IdNode("b", "int");
		Node cp           = new IdNode("c", "int");
		Node timesp       = new MultOpNode("*", bp, cp);
		Node plusp        = new AddOpNode("+", ap, timesp); 
		Node statp        = new AssignStatNode(aa,plusp);
		
		Node aput         = new IdNode("a", "int");
		Node put          = new PutStatNode(new AddOpNode("+", aput, n4));
		Node put1         = new PutStatNode(ida1);
		Node put2         = new PutStatNode(idb1);
		Node put3         = new PutStatNode(idc1);
		
		Node emptyclasslist   = new ClassListNode();
		Node emptyfuncdeflist = new FuncDefListNode();
		Node progblock2       = new ProgramBlockNode(Arrays.asList(vda,vdb,vdc,stata,put1,statb,put2,statc,put3,statp,put));
		Node progblockonly    = new ProgNode(emptyclasslist, emptyfuncdeflist, progblock2);

		ASTTextPrinterVisitor           ASTTPVisitor1 = new ASTTextPrinterVisitor("example2.ast");
		ASTDotPrinterVisitor            ASTDPVisitor1 = new ASTDotPrinterVisitor("example2.dot");
		ReconstructSourceProgramVisitor CSVisitor1    = new ReconstructSourceProgramVisitor("example2.source"); 
		TypeCheckingVisitor             TCVisitor1    = new TypeCheckingVisitor("example2.errors"); 
		SymTabCreationVisitor           STCVisitor1   = new SymTabCreationVisitor(); 
		ComputeMemSizeVisitor           CMSVisitor1   = new ComputeMemSizeVisitor("example2.symtab");
		TagsBasedCodeGenerationVisitor  CGVisitor1    = new TagsBasedCodeGenerationVisitor("example2.tags.m");
		StackBasedCodeGenerationVisitor CGSVisitor1   = new StackBasedCodeGenerationVisitor("example2.stack.m");
		
		progblockonly.accept(CSVisitor1);
		progblockonly.accept(TCVisitor1);
		progblockonly.accept(ASTTPVisitor1);
		progblockonly.accept(ASTDPVisitor1);
		progblockonly.accept(STCVisitor1);
		progblockonly.accept(CMSVisitor1);
		progblockonly.accept(CGVisitor1);
		progblockonly.accept(CGSVisitor1);
		
		/*************************************************************
		 * EXAMPLE 3
		 *************************************************************/
		
		/**
		 * program that is composed of an empty class list, 
		 * an empty function definition list, 
		 * and a program statement block with three variables, an assignment statement and a put statement	
		 *
		 *    program{
		 *	    int a;
		 *      int b;
	     *	    int c;
         *	    a = 1;
         *      put(a);
         *	    b = 2;
         *      put(b);
         *	    c = 3;
         *      put(c);
         *	    a = f(a,b,c) * b + c;
         *	    put(a + 4);
         *    } // (7*2+3)+4=21 output:123123721
         *    
         *    int f(int p1, int p2, int p3){
         *      int retval;
         *      put(p1);
         *      put(p2);
         *      put(p3);
         * 		retval = p1+p2*p3;
         * 		put(retval);
         * 		return(retval);
         *    } // 1+2*3=7
		 */
		
		Node type3a        = new TypeNode("int");
		Node type3b        = new TypeNode("int");
		Node type3c        = new TypeNode("int");
		Node type3p1       = new TypeNode("int");
		Node type3p2       = new TypeNode("int");
		Node type3p3       = new TypeNode("int");
		Node type3retval   = new TypeNode("int");
		Node type3retval1   = new TypeNode("int");
		
			
		Node id3a1         = new IdNode("a", "int");
		Node id3a2         = new IdNode("a", "int");
		Node id3a3         = new IdNode("a", "int");
		Node id3a4         = new IdNode("a", "int");
		Node id3a5         = new IdNode("a", "int");
		Node id3a6         = new IdNode("a", "int");
		
		Node id3b1         = new IdNode("b", "int");
		Node id3b2         = new IdNode("b", "int");
		Node id3b3         = new IdNode("b", "int");
		Node id3b4         = new IdNode("b", "int");
		Node id3b5         = new IdNode("b", "int");
		Node id3b6         = new IdNode("b", "int");
		
		Node id3c1         = new IdNode("c", "int");
		Node id3c2         = new IdNode("c", "int");
		Node id3c3         = new IdNode("c", "int");
		Node id3c4         = new IdNode("c", "int");
		Node id3c5         = new IdNode("c", "int");
		Node id3c6         = new IdNode("c", "int");
		
		Node id3p11         = new IdNode("p1", "int");
		Node id3p12         = new IdNode("p1", "int");		
		Node id3p13         = new IdNode("p1", "int");		

		Node id3p21         = new IdNode("p2", "int");
		Node id3p22         = new IdNode("p2", "int");	
		Node id3p23         = new IdNode("p2", "int");	

		Node id3p31         = new IdNode("p3", "int");
		Node id3p32         = new IdNode("p3", "int");
		Node id3p33         = new IdNode("p3", "int");

		Node id3retval1     = new IdNode("retval1", "int");
		Node id3retval1a     = new IdNode("retval1", "int");
		Node id3retval2     = new IdNode("retval2", "int");	
		Node id3retval2a     = new IdNode("retval2", "int");	
		Node id3retval3     = new IdNode("retval3", "int");		

		Node id3f1          = new IdNode("f", "int");		
		Node id3f2          = new IdNode("f", "int");		

		Node vd3a          = new VarDeclNode(type3a, id3a1);
		Node vd3b          = new VarDeclNode(type3b, id3b1);
		Node vd3c          = new VarDeclNode(type3c, id3c1);
		Node vd3p1         = new VarDeclNode(type3p1, id3p11);
		Node vd3p2         = new VarDeclNode(type3p2, id3p21);
		Node vd3p3         = new VarDeclNode(type3p3, id3p31);
		Node vd3retval     = new VarDeclNode(type3retval, id3retval1a);

		Node n31           = new NumNode("1", "int");
		Node n32           = new NumNode("2", "int");
		Node n33           = new NumNode("3", "int");
		Node n34           = new NumNode("4", "int");
		
		Node stat3aeq1        = new AssignStatNode(id3a2,n31); 
		Node stat3beq2        = new AssignStatNode(id3b2,n32); 
		Node stat3ceq3        = new AssignStatNode(id3c2,n33); 

		Node times3p2tp3        = new MultOpNode("*", id3p23, id3p33);
		Node plus3p1pt          = new AddOpNode("+", id3p13, times3p2tp3); 
		Node stat3reqp1pp2tp3   = new AssignStatNode(id3retval2,plus3p1pt);
		
		Node put3p1        = new PutStatNode(id3p12);
		Node put3p2        = new PutStatNode(id3p22);
		Node put3p3        = new PutStatNode(id3p32);
		Node put3retval    = new PutStatNode(id3retval3);
		
		Node return3       = new ReturnStatNode(id3retval2a);
		
		Node put3ap4       = new PutStatNode(new AddOpNode("+", id3a3, n34));
		Node put3a         = new PutStatNode(id3a4);
		Node put3b         = new PutStatNode(id3b4);
		Node put3c         = new PutStatNode(id3c4);
		
		Node statblock3f   = new StatBlockNode(Arrays.asList(vd3retval, put3p1, put3p2, put3p3, stat3reqp1pp2tp3, put3retval, return3)); 
		
		Node funcdef3      = new FuncDefNode(type3retval1, id3f2, new ParamListNode(Arrays.asList(vd3p1, vd3p2, vd3p3)), statblock3f);
		
		Node flist3        = new FuncDefListNode(Arrays.asList(funcdef3));
		
		Node times3ftb     = new MultOpNode("*", new FuncCallNode(id3f1, new ParamListNode(Arrays.asList(id3a6, id3b6, id3c6))), id3b5);
		Node plus3tpc	   = new AddOpNode("+", times3ftb, id3c5);
		Node stat3f        = new AssignStatNode(id3a5, plus3tpc);
		
		Node emptyclasslist3   = new ClassListNode();
		Node progblock3        = new ProgramBlockNode(Arrays.asList(vd3a,vd3b,vd3c,stat3aeq1,put3a,stat3beq2,put3b,stat3ceq3,put3c,stat3f,put3ap4));
		Node progblockonly3    = new ProgNode(emptyclasslist3, flist3, progblock3);

		ASTTextPrinterVisitor           ASTTPVisitor3 = new ASTTextPrinterVisitor("example3.ast");
		ASTDotPrinterVisitor            ASTDPVisitor3 = new ASTDotPrinterVisitor("example3.dot");
		ReconstructSourceProgramVisitor CSVisitor3    = new ReconstructSourceProgramVisitor("example3.source"); 
		TypeCheckingVisitor             TCVisitor3    = new TypeCheckingVisitor("example3.errors"); 
		SymTabCreationVisitor           STCVisitor3   = new SymTabCreationVisitor(); 
		ComputeMemSizeVisitor           CMSVisitor3   = new ComputeMemSizeVisitor("example3.symtab");
		TagsBasedCodeGenerationVisitor  CGVisitor3    = new TagsBasedCodeGenerationVisitor("example3.tags.m");
		StackBasedCodeGenerationVisitor CGSVisitor3   = new StackBasedCodeGenerationVisitor("example3.stack.m");
		
		progblockonly3.accept(CSVisitor3);
		progblockonly3.accept(TCVisitor3);
		progblockonly3.accept(ASTTPVisitor3);
		progblockonly3.accept(ASTDPVisitor3);
		progblockonly3.accept(STCVisitor3);
		progblockonly3.accept(CMSVisitor3);
		progblockonly3.accept(CGVisitor3);
		progblockonly3.accept(CGSVisitor3);
		
	}
}