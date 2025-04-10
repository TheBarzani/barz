package AST;
import Visitors.Visitor;

public class MultOpNode extends Node {
	
	public MultOpNode(String p_data){
		super(p_data);
	}
	
	public MultOpNode(String p_data, Node p_parent){
		super(p_data, p_parent);
	}
	
	public MultOpNode(String p_data, Node p_leftChild, Node p_rightChild){
		super(p_data); 
		this.addChild(p_leftChild);
		this.addChild(p_rightChild);
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}
