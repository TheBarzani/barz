package AST;
import Visitors.Visitor;

public class FuncCallNode extends Node {
		
	public FuncCallNode(){
		super("");
	}
	
	public FuncCallNode(Node p_parent){
		super("", p_parent);
	}
	
	public FuncCallNode(Node p_id, Node p_paramList){
		super(""); 
		this.addChild(p_id);
		this.addChild(p_paramList);		
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}