package AST;
import Visitors.Visitor;

public class PutStatNode extends Node {
	
	public PutStatNode(){
		super("");
	}
		
	public PutStatNode(Node p_child){
		super(""); 
		this.addChild(p_child);
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}
