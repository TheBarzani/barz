package AST;
import java.util.List;

import Visitors.Visitor;

public class ClassNode extends Node {
		
	public ClassNode(){
		super("");
	}
	
	public ClassNode(Node p_parent){
		super("", p_parent);
	}
	
	public ClassNode(Node p_id, List<Node> p_listOfClassMemberNodes){
		super("");
		this.addChild(p_id);
		for (Node child : p_listOfClassMemberNodes)
			this.addChild(child);
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}