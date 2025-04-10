package AST;
import java.util.List;
import Visitors.Visitor;

public class ProgramBlockNode extends Node {
		
	public ProgramBlockNode(){
		super("");
	}
	
	public ProgramBlockNode(Node p_parent){
		super("", p_parent);
	}
	
	public ProgramBlockNode(List<Node> p_listOfStatOrVarDeclNodes){
		super("");
		for (Node child : p_listOfStatOrVarDeclNodes)
			this.addChild(child);
	}
	
	public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}