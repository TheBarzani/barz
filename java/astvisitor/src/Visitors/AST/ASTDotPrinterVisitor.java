package Visitors.AST;
import java.io.File;
import java.io.PrintWriter;

import AST.*;
import Visitors.Visitor;

public class ASTDotPrinterVisitor  extends Visitor {
	
	public String m_outputfilename = new String(); 
	public String m_outputstring   = new String();
	public int m_nodeId = 0;
	
	public ASTDotPrinterVisitor() {
	}
	
	public ASTDotPrinterVisitor(String p_filename) {
		this.m_outputfilename = p_filename; 
	}
	
	public void visit(ProgNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += "digraph AST {\n" + 
				          "node [shape=record];\n node [fontname=Sans];charset=\"UTF-8\" splines=true splines=spline rankdir =LR\n";
		m_outputstring += p_node.m_nodeId + "[label=\"Prog\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
		m_outputstring += "}";
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
		m_outputstring += p_node.m_nodeId + "[label=\"ProgramBlock\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};

	public void visit(StatBlockNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"StatBlock\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};
	
	public void visit(ClassNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"Class | " + p_node.getData() + "\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};
	
	public void visit(PutStatNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"PutStat\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};
	
	public void visit(FuncDefNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"FuncDef\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};

	public void visit(ParamListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"ParamList\"];\n";
    	for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
    	}
	 };

	public void visit(TypeNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"Type | " + p_node.getData() + "\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};
	
	public void visit(VarDeclNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"VarDecl\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	 }; 

	public void visit(DimListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"DimList\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
		if (p_node.getChildren().size() == 0) {
			m_outputstring += "none" + p_node.m_nodeId + "[shape=point];\n";
			m_outputstring += p_node.m_nodeId + "->" +  "none" + p_node.m_nodeId+ ";\n";
		}
	};
	
	public void visit(IdNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"Id | " + p_node.getData() + "\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}
	
	public void visit(NumNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"Num |" + p_node.getData() + "\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}

	public void visit(AddOpNode p_node){
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"AddOp\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}

	public void visit(MultOpNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"MultOp\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}
	
	public void visit(AssignStatNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"AssignStat\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}

	// Below are the visit methods for node types for which this visitor does
	// not apply. They still have to propagate acceptance of the visitor to
	// their children.
	
	public void visit(FuncDefListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"FuncDefList\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};
	
	public void visit(ClassListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"ClassList\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};
	
	public void visit(Node p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"Node\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	};

	public void visit(DimNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"Dim |" + p_node.getData() + "\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}; 

	public void visit(FuncCallNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"FuncCall\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}; 
	public void visit(ReturnStatNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		m_outputstring += p_node.m_nodeId + "[label=\"ReturnStat\"];\n";
		for (Node child : p_node.getChildren()) {
			m_outputstring += p_node.m_nodeId + "->" +  child.m_nodeId+ ";\n";
			child.accept(this);
		}
	}; 
}
