package AST;
import java.util.List;

import Visitors.Visitor;

public class ClassListNode extends Node {
	
	public ClassListNode(){
		super("");
	}
	
	public ClassListNode(Node p_parent){
		super("", p_parent);
	}
	
	public ClassListNode(List<Node> p_listOfClassNodes){
		super("");
		for (Node child : p_listOfClassNodes)
			this.addChild(child);
	}

	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}