package AST;
import Visitors.Visitor;

public class TypeNode extends Node {
	
	public TypeNode(String p_data){
		super(p_data);
	}
	
	public TypeNode(String p_data, Node p_parent){
		super(p_data, p_parent);
	}
	 
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}
