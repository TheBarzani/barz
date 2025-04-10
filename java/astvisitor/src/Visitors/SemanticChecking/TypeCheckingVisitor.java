package Visitors.SemanticChecking;
import java.io.File;
import java.io.PrintWriter;

import AST.*;
import Visitors.Visitor;

/**
 * Visitor to compute the type of subexpressions and assignment statements. 
 * 
 * This applies only to nodes that are part of expressions and assignment statements i.e.
 * AddOpNode, MultOpNode, and AssignStatp_node. 
 * 
 */

public class TypeCheckingVisitor extends Visitor {

	public String m_outputfilename = new String();
	public String m_errors         = new String();
    
	public TypeCheckingVisitor() {
	}
	
	public TypeCheckingVisitor(String p_filename) {
		this.m_outputfilename = p_filename; 
	}

	public void visit(ProgNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
		if (!this.m_outputfilename.isEmpty()) {
			File file = new File(this.m_outputfilename);
			try (PrintWriter out = new PrintWriter(file)){ 
			    out.println(this.m_errors);
			}
			catch(Exception e){
				e.printStackTrace();}
		}
	};
	
	public void visit(AddOpNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		String leftOperandType  = p_node.getChildren().get(0).getType();
		String rightOperandType = p_node.getChildren().get(1).getType();
		if( leftOperandType == rightOperandType )
			p_node.setType(leftOperandType);
		else{
			p_node.setType("typeerror");
			this.m_errors += "AddOpNode type error:  " 
					+ p_node.getChildren().get(0).getData()
					+ "(" + p_node.getChildren().get(0).getType() + ")"
					+  " and "
					+ p_node.getChildren().get(1).getData()
					+ "(" + p_node.getChildren().get(1).getType() + ")"
					+ "\n";
		}
	}
	
	public void visit(MultOpNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		String leftOperandType  = p_node.getChildren().get(0).getType();
		String rightOperandType = p_node.getChildren().get(1).getType();
		if( leftOperandType == rightOperandType )
			p_node.setType(leftOperandType);
		else{
			p_node.setType("typeerror");
			this.m_errors += "MultOpNode type error: " 
					+ p_node.getChildren().get(0).getData()
					+ "(" + p_node.getChildren().get(0).getType() + ")"
					+  " and "
					+ p_node.getChildren().get(1).getData()
					+ "(" + p_node.getChildren().get(1).getType() + ")"
					+ "\n";
		}
	}
	
	public void visit(AssignStatNode p_node){ 
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		String leftOperandType  = p_node.getChildren().get(0).getType();
		String rightOperandType = p_node.getChildren().get(1).getType();
		if( leftOperandType == rightOperandType )
			p_node.setType(leftOperandType);
		else{
			p_node.setType("typeerror");
			this.m_errors += "AssignStatNode type error: " 
					+ p_node.getChildren().get(0).getData()
					+ "(" + p_node.getChildren().get(0).getType() + ")"
					+  " and "
					+ p_node.getChildren().get(1).getData()
					+ "(" + p_node.getChildren().get(1).getType() + ")"
					+ "\n";
		}
	}
	
	public void visit(FuncCallNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
		p_node.m_type = p_node.getChildren().get(0).m_type;
		p_node.setData(p_node.getChildren().get(0).getData());
	}; 
	
	// Below are the visit methods for node types for which this visitor does not apply
	// They still have to propagate acceptance of the visitor to their children.
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

	public void visit(FuncDefNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
		p_node.setData(p_node.getChildren().get(1).getData());
		p_node.setType(p_node.getChildren().get(1).getType());
	};

	public void visit(IdNode p_node) {
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

	public void visit(NumNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(ProgramBlockNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren())
			child.accept(this);
	};

	public void visit(PutStatNode p_node) {
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
		for (Node child : p_node.getChildren())
			child.accept(this);
		p_node.m_type = p_node.getData();
	};

	public void visit(VarDeclNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
	 }; 

	public void visit(ParamListNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
	 }

	public void visit(DimNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
	}

	public void visit(ReturnStatNode p_node) {
		// propagate accepting the same visitor to all the children
		// this effectively achieves Depth-First AST Traversal
		for (Node child : p_node.getChildren() )
			child.accept(this);
	}; 
}
