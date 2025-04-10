package AST;
import Visitors.Visitor;

public class AddOpNode extends Node {
	
	public AddOpNode(String p_data){
		super(p_data);
	}
	
	public AddOpNode(String p_data, Node p_parent){
		super(p_data, p_parent);
	}
	
	public AddOpNode(String p_data, Node p_leftChild, Node p_rightChild){
		super(p_data); 
		this.addChild(p_leftChild);
		this.addChild(p_rightChild);
	}

	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}