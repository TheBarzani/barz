package Visitors.AST;
import java.io.File;
import java.io.PrintWriter;

import AST.*;
import Visitors.Visitor;

public class ASTTextPrinterVisitor  extends Visitor {
	
	public String m_outputfilename = new String(); 
	public String m_outputstring   = new String();
	
	public ASTTextPrinterVisitor() {
	}
	
	public ASTTextPrinterVisitor(String p_filename) {
		this.m_outputfilename = p_filename; 
	}
	
	public void printLine(Node p_node) {
	   	for (int i = 0; i < Node.m_nodelevel; i++ )
	   		m_outputstring += "  ";
    	
    	String toprint = String.format("%-25s" , p_node.getClass().getName()); 
    	for (int i = 0; i < Node.m_nodelevel; i++ )
    		toprint = toprint.substring(0, toprint.length() - 2);
    	toprint += String.format("%-12s" , (p_node.getData() == null || p_node.getData().isEmpty())         ? " | " : " | " + p_node.getData());    	
    	toprint += String.format("%-12s" , (p_node.getType() == null || p_node.getType().isEmpty())         ? " | " : " | " + p_node.getType());
        toprint += (String.format("%-16s" , (p_node.m_subtreeString == null || p_node.m_subtreeString.isEmpty()) ? " | " : " | " + (p_node.m_subtreeString.replaceAll("\\n+",""))));
    	
        m_outputstring += toprint + "\n";
    	
    	Node.m_nodelevel++;
//    	List<Node> children = p_node.getChildren();
//		for (int i = 0; i < children.size(); i++ ){
//			children.get(i).printSubtree();
//		}
		Node.m_nodelevel--;	
	}
	
	public void visit(ProgNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += "=====================================================================\n";
		m_outputstring += "Node type                 | data      | type      | subtreestring\n";
		m_outputstring += "=====================================================================\n";
    	this.printLine(p_node);
    	Node.m_nodelevel++;
    	for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
    	m_outputstring += "=====================================================================\n"; 
		if (!this.m_outputfilename.isEmpty()) {
			File file = new File(this.m_outputfilename);
			try (PrintWriter out = new PrintWriter(file)){ 
			    out.println(this.m_outputstring);
			}
			catch(Exception e){
				e.printStackTrace();}
		}
	};
	
	public void visit(ProgramBlockNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};

	public void visit(StatBlockNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(ClassNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(PutStatNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(FuncDefNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};

	public void visit(ParamListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
    	for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	 };

	public void visit(TypeNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(VarDeclNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	 }; 

	public void visit(DimListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(IdNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}
	
	public void visit(NumNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}

	public void visit(AddOpNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}

	public void visit(MultOpNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}
	
	public void visit(AssignStatNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}

	// Below are the visit methods for node types for which this visitor does
	// not apply. They still have to propagate acceptance of the visitor to
	// their children.
	
	public void visit(FuncDefListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(ClassListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};
	
	public void visit(Node p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	};

	public void visit(DimNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}; 

	public void visit(FuncCallNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}; 
	public void visit(ReturnStatNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
    	this.printLine(p_node);
    	Node.m_nodelevel++;
		for (Node child : p_node.getChildren())
			child.accept(this);
    	Node.m_nodelevel--;
	}; 
}
