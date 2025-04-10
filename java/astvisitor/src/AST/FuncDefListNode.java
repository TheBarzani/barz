package AST;
import java.util.List;

import Visitors.Visitor;

public class FuncDefListNode extends Node {
	
	public FuncDefListNode(){
		super("");
	}
	
	public FuncDefListNode(Node p_parent){
		super("", p_parent);
	}
	
	public FuncDefListNode(List<Node> p_listOfFuncDefNodes){
		super("");
		for (Node child : p_listOfFuncDefNodes)
			this.addChild(child);
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}