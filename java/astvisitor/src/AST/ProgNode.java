package AST;
import Visitors.Visitor;

public class ProgNode extends Node {
	
	public ProgNode(){
		super("");
	}
	
	public ProgNode(Node p_parent){
		super("", p_parent);
	}
	
	public ProgNode(Node p_classlist, Node p_funcdeflist, Node p_programstatblock){
		super(""); 
		this.addChild(p_classlist);
		this.addChild(p_funcdeflist);
		this.addChild(p_programstatblock);		
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
	
}