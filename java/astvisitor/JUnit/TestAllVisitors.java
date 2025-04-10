

import java.io.IOException;
import java.io.InputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.Paths;
import java.util.Arrays;

import org.junit.jupiter.api.BeforeAll;
import org.junit.jupiter.api.Test;

import AST.*;
import Visitors.AST.ASTTextPrinterVisitor;
import Visitors.AST.ReconstructSourceProgramVisitor;
import Visitors.CodeGeneration.StackBasedCodeGenerationVisitor;
import Visitors.CodeGeneration.TagsBasedCodeGenerationVisitor;
import Visitors.SemanticChecking.TypeCheckingVisitor;
import Visitors.SymbolTable.ComputeMemSizeVisitor;
import Visitors.SymbolTable.SymTabCreationVisitor;

class TestAllVisitors {
	
	static String m_result_filename_ast;
	static String m_expected_filename_ast;
	static String m_result_filename_source;
	static String m_expected_filename_source;
	static String m_result_filename_errors;
	static String m_expected_filename_errors;
    static String m_result_filename_symtab;
	static String m_expected_filename_symtab;
    static String m_result_filename_moon_tags;
	static String m_expected_filename_moon_tags;
    static String m_result_filename_moon_stack;
	static String m_expected_filename_moon_stack;
    
	boolean sameContent(Path file1, Path file2) throws IOException {
	    final long size = Files.size(file1);
	    if (size != Files.size(file2))
	        return false;

	    if (size < 4096)
	        return Arrays.equals(Files.readAllBytes(file1), Files.readAllBytes(file2));

	    try (InputStream is1 = Files.newInputStream(file1);
	         InputStream is2 = Files.newInputStream(file2)) {
	        // Compare byte-by-byte.
	        // Note that this can be sped up drastically by reading large chunks
	        // (e.g. 16 KBs) but care must be taken as InputStream.read(byte[])
	        // does not neccessarily read a whole array!
	        int data;
	        while ((data = is1.read()) != -1)
	            if (data != is2.read())
	                return false;
	    }

	    return true;
	}

	@BeforeAll 
	public static void init() {		
		m_result_filename_ast          = new String("JUnit/result.test1.ast");
		m_expected_filename_ast        = new String("JUnit/expected.test1.ast");
		m_result_filename_source       = new String("JUnit/result.test1.source");
		m_expected_filename_source     = new String("JUnit/expected.test1.source");
		m_result_filename_errors       = new String("JUnit/result.test1.errors");
	    m_expected_filename_errors     = new String("JUnit/expected.test1.errors");
		m_result_filename_symtab       = new String("JUnit/result.test1.symtab");
	    m_expected_filename_symtab     = new String("JUnit/expected.test1.symtab");
		m_result_filename_moon_tags    = new String("JUnit/result.test1.tags.m");
	    m_expected_filename_moon_tags  = new String("JUnit/expected.test1.tags.m");
		m_result_filename_moon_stack   = new String("JUnit/result.test1.stack.m");
	    m_expected_filename_moon_stack = new String("JUnit/expected.test1.stack.m");
	    
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
			
		Node id3a1         = new IdNode("a", "int");
		Node id3a2         = new IdNode("a", "int");
		Node id3a3         = new IdNode("a", "int");
		Node id3a4         = new IdNode("a", "int");
		Node id3a5         = new IdNode("a", "int");
		
		Node id3b1         = new IdNode("b", "int");
		Node id3b2         = new IdNode("b", "int");
		Node id3b3         = new IdNode("b", "int");
		Node id3b4         = new IdNode("b", "int");
		Node id3b5         = new IdNode("b", "int");
		
		Node id3c1         = new IdNode("c", "int");
		Node id3c2         = new IdNode("c", "int");
		Node id3c3         = new IdNode("c", "int");
		Node id3c4         = new IdNode("c", "int");
		Node id3c5         = new IdNode("c", "int");
		
		Node id3p11         = new IdNode("p1", "int");
		Node id3p12         = new IdNode("p1", "int");		
		Node id3p13         = new IdNode("p1", "int");		

		Node id3p21         = new IdNode("p2", "int");
		Node id3p22         = new IdNode("p2", "int");	
		Node id3p23         = new IdNode("p2", "int");	

		Node id3p31         = new IdNode("p3", "int");
		Node id3p32         = new IdNode("p3", "int");
		Node id3p33         = new IdNode("p3", "int");

		Node id3retval1     = new IdNode("retval", "int");
		Node id3retval2     = new IdNode("retval", "int");		
		Node id3retval3     = new IdNode("retval", "int");		

		Node id3f1          = new IdNode("f", "int");		
		Node id3f2          = new IdNode("f", "int");		

		Node vd3a          = new VarDeclNode(type3a, id3a1);
		Node vd3b          = new VarDeclNode(type3b, id3b1);
		Node vd3c          = new VarDeclNode(type3c, id3c1);
		Node vd3p1         = new VarDeclNode(type3p1, id3p11);
		Node vd3p2         = new VarDeclNode(type3p2, id3p21);
		Node vd3p3         = new VarDeclNode(type3p3, id3p31);
		Node vd3retval     = new VarDeclNode(type3retval, id3retval1);

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
		
		Node return3       = new ReturnStatNode(id3retval2);
		
		Node put3ap4       = new PutStatNode(new AddOpNode("+", id3a3, n34));
		Node put3a         = new PutStatNode(id3a3);
		Node put3b         = new PutStatNode(id3b3);
		Node put3c         = new PutStatNode(id3c3);
		
		Node statblock3f   = new StatBlockNode(Arrays.asList(vd3retval, put3p1, put3p2, put3p3, stat3reqp1pp2tp3, put3retval, return3)); 
		
		Node funcdef3      = new FuncDefNode(type3retval, id3f2, new ParamListNode(Arrays.asList(vd3p1, vd3p2, vd3p3)), statblock3f);
		
		Node flist3        = new FuncDefListNode(Arrays.asList(funcdef3));
		
		Node times3ftb     = new MultOpNode("*", new FuncCallNode(id3f1, new ParamListNode(Arrays.asList(id3a4, id3b4, id3c4))), id3b5);
		Node plus3tpc	   = new AddOpNode("+", times3ftb, id3c5);
		Node stat3f        = new AssignStatNode(id3a5, plus3tpc);
		
		Node emptyclasslist3   = new ClassListNode();
		Node progblock3        = new ProgramBlockNode(Arrays.asList(vd3a,vd3b,vd3c,stat3aeq1,put3a,stat3beq2,put3b,stat3ceq3,put3c,stat3f,put3ap4));
		Node prog              = new ProgNode(emptyclasslist3, flist3, progblock3);

		ASTTextPrinterVisitor               ASTPVisitor = new ASTTextPrinterVisitor(m_result_filename_ast);
		ReconstructSourceProgramVisitor CSVisitor   = new ReconstructSourceProgramVisitor(m_result_filename_source); 
		TypeCheckingVisitor             TCVisitor   = new TypeCheckingVisitor(m_result_filename_errors); 
		SymTabCreationVisitor           STCVisitor  = new SymTabCreationVisitor(); 
		ComputeMemSizeVisitor           CMSVisitor  = new ComputeMemSizeVisitor(m_result_filename_symtab);
		TagsBasedCodeGenerationVisitor  CGVisitor   = new TagsBasedCodeGenerationVisitor(m_result_filename_moon_tags);
		StackBasedCodeGenerationVisitor CGSVisitor  = new StackBasedCodeGenerationVisitor(m_result_filename_moon_stack);
		
		prog.accept(CSVisitor);
		prog.accept(TCVisitor);
		prog.accept(ASTPVisitor);
		prog.accept(STCVisitor);
		prog.accept(CMSVisitor);
		prog.accept(CGVisitor);
		prog.accept(CGSVisitor);
	}
	
	@Test
	void test_ast() {
		try{
			assert(sameContent(Paths.get(m_expected_filename_ast), Paths.get(m_result_filename_ast)));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Test
	void test_source() {
		try{
			assert(sameContent(Paths.get(m_expected_filename_source), Paths.get(m_result_filename_source)));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Test
	void test_errors() {
		try{
			assert(sameContent(Paths.get(m_expected_filename_errors), Paths.get(m_result_filename_errors)));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Test
	void test_symtab() {
		System.out.println("test_symtab");
		try{
			assert(sameContent(Paths.get(m_expected_filename_symtab), Paths.get(m_result_filename_symtab)));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Test
	void test_moon_code_with_tags() {
		System.out.println("test_moon_code_with_tags");
		try{
			assert(sameContent(Paths.get(m_result_filename_moon_tags), Paths.get(m_result_filename_moon_tags)));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
	
	@Test
	void test_moon_code_with_stack() {
		System.out.println("test_moon_code_with_stack");
		try{
			assert(sameContent(Paths.get(m_expected_filename_moon_stack), Paths.get(m_result_filename_moon_stack)));
		}
		catch(Exception e) {
			e.printStackTrace();
		}
	}
}
