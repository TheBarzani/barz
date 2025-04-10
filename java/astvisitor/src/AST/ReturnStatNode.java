package AST;
import Visitors.Visitor;

public class ReturnStatNode extends Node {
	
	public ReturnStatNode(){
		super("");
	}
		
	public ReturnStatNode(Node p_child){
		super(""); 
		this.addChild(p_child);
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}
