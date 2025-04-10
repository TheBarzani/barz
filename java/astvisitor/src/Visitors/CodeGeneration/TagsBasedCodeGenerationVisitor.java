package Visitors.CodeGeneration;
import java.io.File;
import java.io.PrintWriter;
import java.util.Stack;

import AST.*;
import SymbolTable.SymTabEntry;
import Visitors.Visitor;

/**
 * Visitor to generate moon code for simple expressions and assignment and put 
 * statements, and function calls. Uses a label-based model. Note that the 
 * label-based model bring limitations and is inefficient. 
 *
 */

public class TagsBasedCodeGenerationVisitor extends Visitor {
	
    public Stack<String> m_registerPool   = new Stack<String>();
    public Integer       m_tempVarNum     = 0;
    public String        m_moonExecCode   = new String();               // moon code instructions part
    public String        m_moonDataCode   = new String();               // moon code data part
    public String        m_mooncodeindent = new String("           ");
    public String        m_outputfilename = new String(); 
    
    public TagsBasedCodeGenerationVisitor() {
       	// create a pool of registers as a stack of Strings
    	// assuming only r1, ..., r12 are available
    	for (Integer i = 12; i>=1; i--)
    		m_registerPool.push("r" + i.toString());
    }
    
    public TagsBasedCodeGenerationVisitor(String p_filename) {
    	this.m_outputfilename = p_filename; 
       	// create a pool of registers as a stack of Strings
    	// assuming only r1, ..., r12 are available
    	for (Integer i = 12; i>=1; i--)
    		m_registerPool.push("r" + i.toString());
    }

	public void visit(ProgNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);	
		if (!this.m_outputfilename.isEmpty()) {
			File file = new File(this.m_outputfilename);
			try (PrintWriter out = new PrintWriter(file)) {
			    out.println(this.m_moonExecCode);
			    out.println(this.m_moonDataCode);}		
			catch(Exception e){
				e.printStackTrace();}
		}
	};
	
	public void visit(VarDeclNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		// Then, do the processing of this nodes' visitor
		if (p_node.getChildren().get(0).getData() == "int")
			m_moonDataCode += m_mooncodeindent + "% space for variable " + p_node.getChildren().get(1).getData() + "\n";
			m_moonDataCode += String.format("%-10s" ,p_node.getChildren().get(1).getData()) + " res 4\n";
	}

	public void visit(NumNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		// Then, do the processing of this nodes' visitor
		// create a local variable and allocate a register to this subcomputation 
		String localRegister = this.m_registerPool.pop();
		//generate code
		m_moonDataCode += m_mooncodeindent + "% space for constant " + p_node.getData() + "\n";
		m_moonDataCode += String.format("%-10s",p_node.m_moonVarName) + " res 4\n";
		m_moonExecCode += m_mooncodeindent + "% processing: " + p_node.m_moonVarName  + " := " + p_node.getData() + "\n";
		m_moonExecCode += m_mooncodeindent + "addi " + localRegister + ",r0," + p_node.getData() + "\n"; 
		m_moonExecCode += m_mooncodeindent + "sw " + p_node.m_moonVarName + "(r0)," + localRegister + "\n";
		// deallocate the register for the current node
		this.m_registerPool.push(localRegister);
	}
	
	public void visit(AddOpNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		// Then, do the processing of this nodes' visitor
		// create a local variable and allocate a register to this subcomputation 
		String localRegister      = this.m_registerPool.pop();
		String leftChildRegister  = this.m_registerPool.pop();
		String rightChildRegister = this.m_registerPool.pop();
		// generate code
		m_moonExecCode += m_mooncodeindent + "% processing: " + p_node.m_moonVarName + " := " + p_node.getChildren().get(0).m_moonVarName + " + " + p_node.getChildren().get(1).m_moonVarName + "\n";
		m_moonExecCode += m_mooncodeindent + "lw "  + leftChildRegister +  "," + p_node.getChildren().get(0).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "lw "  + rightChildRegister + "," + p_node.getChildren().get(1).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "add " + localRegister +      "," + leftChildRegister + "," + rightChildRegister + "\n"; 
		m_moonDataCode += m_mooncodeindent + "% space for " + p_node.getChildren().get(0).m_moonVarName + " + " + p_node.getChildren().get(1).m_moonVarName + "\n";
		m_moonDataCode += String.format("%-10s",p_node.m_moonVarName) + " res 4\n";
		m_moonExecCode += m_mooncodeindent + "sw " + p_node.m_moonVarName + "(r0)," + localRegister + "\n";
		// deallocate the registers for the two children, and the current node
		this.m_registerPool.push(leftChildRegister);
		this.m_registerPool.push(rightChildRegister);
		this.m_registerPool.push(localRegister);
	}

	public void visit(MultOpNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		// Then, do the processing of this nodes' visitor		
		// create a local variable and allocate a register to this subcomputation 
		String localRegister      = this.m_registerPool.pop();
		String leftChildRegister  = this.m_registerPool.pop();
		String rightChildRegister = this.m_registerPool.pop();
		// generate code
		m_moonExecCode += m_mooncodeindent + "% processing: " + p_node.m_moonVarName + " := " + p_node.getChildren().get(0).m_moonVarName + " * " + p_node.getChildren().get(1).m_moonVarName + "\n";
		m_moonExecCode += m_mooncodeindent + "lw "  + leftChildRegister  + "," + p_node.getChildren().get(0).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "lw "  + rightChildRegister + "," + p_node.getChildren().get(1).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "mul " + localRegister      + "," + leftChildRegister + "," + rightChildRegister + "\n"; 
		m_moonDataCode += m_mooncodeindent + "% space for " + p_node.getChildren().get(0).m_moonVarName + " * " + p_node.getChildren().get(1).m_moonVarName + "\n";
		m_moonDataCode += String.format("%-10s",p_node.m_moonVarName) + " res 4\n";
		m_moonExecCode += m_mooncodeindent + "sw " + p_node.m_moonVarName + "(r0)," + localRegister + "\n";
		// deallocate the registers for the two children, and the current node
		this.m_registerPool.push(leftChildRegister);
		this.m_registerPool.push(rightChildRegister);
		this.m_registerPool.push(localRegister);
	}
	
	public void visit(AssignStatNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		// Then, do the processing of this nodes' visitor
		// allocate local register
		String localRegister = this.m_registerPool.pop();
		//generate code
		m_moonExecCode += m_mooncodeindent + "% processing: "  + p_node.getChildren().get(0).m_moonVarName + " := " + p_node.getChildren().get(1).m_moonVarName + "\n";
		m_moonExecCode += m_mooncodeindent + "lw " + localRegister + "," + p_node.getChildren().get(1).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "sw " + p_node.getChildren().get(0).m_moonVarName + "(r0)," + localRegister + "\n";
		//deallocate local register
		this.m_registerPool.push(localRegister);		
	}
	
	public void visit(ProgramBlockNode p_node) {
		// generate moon program's entry point
		m_moonExecCode += m_mooncodeindent + "entry\n";
		m_moonExecCode += m_mooncodeindent + "addi r14,r0,topaddr\n";
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
		// generate moon program's end point
		m_moonDataCode += m_mooncodeindent + "% buffer space used for console output\n";
		m_moonDataCode += String.format("%-11s", "buf") + "res 20\n";
		m_moonExecCode += m_mooncodeindent + "hlt\n";
	}
	
	public void visit(PutStatNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
		// Then, do the processing of this nodes' visitor		
		// create a local variable and allocate a register to this subcomputation 
		String localRegister      = this.m_registerPool.pop();
		//generate code
		m_moonExecCode += m_mooncodeindent + "% processing: put("  + p_node.getChildren().get(0).m_moonVarName + ")\n";
		m_moonExecCode += m_mooncodeindent + "lw " + localRegister + "," + p_node.getChildren().get(0).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "% put value on stack\n";	
		m_moonExecCode += m_mooncodeindent + "sw -8(r14)," + localRegister + "\n";
		m_moonExecCode += m_mooncodeindent + "% link buffer to stack\n";	
		m_moonExecCode += m_mooncodeindent + "addi " + localRegister + ",r0, buf\n";
		m_moonExecCode += m_mooncodeindent + "sw -12(r14)," + localRegister + "\n";
		m_moonExecCode += m_mooncodeindent + "% convert int to string for output\n";	
		m_moonExecCode += m_mooncodeindent + "jl r15, intstr\n";	
		m_moonExecCode += m_mooncodeindent + "sw -8(r14),r13\n";
		m_moonExecCode += m_mooncodeindent + "% output to console\n";	
		m_moonExecCode += m_mooncodeindent + "jl r15, putstr\n";
		//deallocate local register
		this.m_registerPool.push(localRegister);		
	};
	
	public void visit(FuncDefNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_moonExecCode += m_mooncodeindent + "% processing function definition: "  + p_node.m_moonVarName + "\n";
		//create the tag to jump onto 
		m_moonExecCode += String.format("%-10s",p_node.getData());
		// copy the jumping-back address value in a tagged cell named "fname" appended with "link"
		m_moonDataCode += String.format("%-11s", p_node.getData() + "link") + "res 4\n";
		m_moonExecCode += m_mooncodeindent + "sw " + p_node.getData() + "link(r0),r15\n";
		// tagged cell for return value
		// here assumed to be integer (limitation)
		m_moonDataCode += String.format("%-11s", p_node.getData() + "return") + "res 4\n";
		//generate the code for the function body
		for (Node child : p_node.getChildren())
			child.accept(this);
		// copy back the jumping-back address into r15
		m_moonExecCode += m_mooncodeindent + "lw r15," + p_node.getData() + "link(r0)\n";
		// jump back to the calling function
		m_moonExecCode += m_mooncodeindent + "jr r15\n";	
	};
	
	public void visit(FuncCallNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		String localregister1 = this.m_registerPool.pop();
		// pass parameters directly in the function's local variables
		// it is assumed that the parameters are the first n entries in the 
		// function's symbol table 
		// here we assume that the parameters are the size of a word, 
		// which is not true for arrays and objects. 
		// In those cases, a loop copying the values e.g. byte-by-byte is necessary
		SymTabEntry tableentryofcalledfunction = p_node.m_symtab.lookupName(p_node.getData());
		int indexofparam = 0;
		m_moonExecCode += m_mooncodeindent + "% processing: function call to "  + p_node.getChildren().get(0).m_moonVarName + " \n";
		for(Node param : p_node.getChildren().get(1).getChildren()){
			m_moonExecCode += m_mooncodeindent + "lw " + localregister1 + "," + param.m_moonVarName + "(r0)\n";
		    String nameofparam = tableentryofcalledfunction.m_subtable.m_symlist.get(indexofparam).m_name;
			m_moonExecCode += m_mooncodeindent + "sw " + nameofparam + "(r0)," + localregister1 + "\n";
			indexofparam++;
		}
		// jump to the called function's code
		// here the name of the label is assumed to be the function's name
		// a unique label generator is necessary in the general case (limitation)
		m_moonExecCode += m_mooncodeindent + "jl r15," + p_node.getData() + "\n";
		// copy the return value in a tagged memory cell
		m_moonDataCode += m_mooncodeindent + "% space for function call expression factor\n";		
		m_moonDataCode += String.format("%-11s", p_node.m_moonVarName) + "res 4\n";
		m_moonExecCode += m_mooncodeindent + "lw " + localregister1 + "," + p_node.getData() + "return(r0)\n";
		m_moonExecCode += m_mooncodeindent + "sw " + p_node.m_moonVarName + "(r0)," + localregister1 + "\n";
		this.m_registerPool.push(localregister1);	
	}; 
	
	public void visit(ReturnStatNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		String localregister1 = this.m_registerPool.pop();
		for (Node child : p_node.getChildren() )
			child.accept(this);
		// copy the result of the return value in a cell tagged with the name "function name" + "return", e.g. "f1return"
		// get the function name from the symbol table
		m_moonExecCode += m_mooncodeindent + "% processing: return("  + p_node.getChildren().get(0).m_moonVarName + ")\n";
		m_moonExecCode += m_mooncodeindent + "lw " + localregister1 + "," + p_node.getChildren().get(0).m_moonVarName + "(r0)\n";
		m_moonExecCode += m_mooncodeindent + "sw "   + p_node.m_symtab.m_name + "return(r0)," + localregister1 + "\n";
		this.m_registerPool.push(localregister1);	
	}
	
	// Below are the visit methods for node types for which this visitor does
	// not apply. They still have to propagate acceptance of the visitor to
	// their children.
	
    public void visit(ParamListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
    }
    
	public void visit(ClassListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(ClassNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(DimListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(FuncDefListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(Node p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(StatBlockNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(TypeNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
    };

	public void visit(DimNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);		
	}; 

	public void visit(IdNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
	}
}
