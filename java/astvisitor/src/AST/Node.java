package AST;
import java.util.ArrayList;
import java.util.List;

import SymbolTable.*;
import Visitors.Visitor;

public abstract class Node {
	
	// Original data members of the class
	// that are not related to the introduction of 
	// the visitors
    private List<Node> m_children  = new ArrayList<Node>();
    private Node       m_parent    = null;
    public String      m_data      = null;
    public static int  m_nodelevel = 0;
    public int         m_nodeId    = 0;
    public static int  m_curNodeId = 0;
    
    // The following data members have been added
    // during the implementation of the visitors
    // These could be added using a decorator pattern
    // triggered by a visitor

    // introduced by type checking visitor
    public String      m_type               = null;
    
    // introduced by the construct assignment and expression string visitor
    public  String      m_subtreeString      = new String();
    
    // introduced by symbol table creation visitor
    public  SymTab      m_symtab             = null;
    public  SymTabEntry m_symtabentry        = null;
    
    // introduced by code generation visitors
    public  String      m_localRegister      = new String(); 
    public  String      m_leftChildRegister  = new String(); 
    public  String      m_rightChildRegister = new String(); 
    public  String      m_moonVarName        = new String();
    
    public Node() {
    	
    }
    
    public Node(String p_data) {
        this.setData(p_data);
        this.m_nodeId = Node.m_curNodeId;
        Node.m_curNodeId++;
    }

    public Node(String p_data, Node p_parent) {
        this.setData(p_data);
        this.setParent(p_parent);
        p_parent.addChild(this);
        this.m_nodeId = Node.m_curNodeId;
        Node.m_curNodeId++;
    }
    
    public Node(String p_data, String p_type) {
        this.setData(p_data);
        this.setType(p_type);
        this.m_nodeId = Node.m_curNodeId;
        Node.m_curNodeId++;
    }    

    public List<Node> getChildren() {
        return m_children;
    }

    public void setParent(Node p_parent) {
        this.m_parent = p_parent;
    }

    public Node getParent() {
        return m_parent;
    }

    public void addChild(Node p_child) {
        p_child.setParent(this);
        this.m_children.add(p_child);
    }

    public String getData() {
        return this.m_data;
    }

    public void setData(String p_data) {
        this.m_data = p_data;
    }

    public String getType() {
        return this.m_type;
    }

    public void setType(String p_type) {
        this.m_type = p_type;
    }

    public String getSubtreeString() {
        return this.m_subtreeString;
    }

    public void setSubtreeString(String p_data) {
        this.m_subtreeString = p_data;
    }

    public boolean isRoot() {
        return (this.m_parent == null);
    }

    public boolean isLeaf() {
        if(this.m_children.size() == 0) 
            return true;
        else 
            return false;
    }

    public void removeParent() {
        this.m_parent = null;
    }

    public void print(){
    	System.out.println("=====================================================================");
    	System.out.println("Node type                 | data      | type      | subtreestring");
    	System.out.println("=====================================================================");
    	this.printSubtree();
    	System.out.println("=====================================================================");

    }

    public void printSubtree(){
    	for (int i = 0; i < Node.m_nodelevel; i++ )
    		System.out.print("  ");
    	
    	String toprint = String.format("%-25s" , this.getClass().getName()); 
    	for (int i = 0; i < Node.m_nodelevel; i++ )
    		toprint = toprint.substring(0, toprint.length() - 2);
    	toprint += String.format("%-12s" , (this.getData() == null || this.getData().isEmpty())         ? " | " : " | " + this.getData());    	
    	toprint += String.format("%-12s" , (this.getType() == null || this.getType().isEmpty())         ? " | " : " | " + this.getType());
        toprint += (String.format("%-16s" , (this.m_subtreeString == null || this.m_subtreeString.isEmpty()) ? " | " : " | " + (this.m_subtreeString.replaceAll("\\n+",""))));
    	
    	System.out.println(toprint);
    	
    	Node.m_nodelevel++;
    	List<Node> children = this.getChildren();
		for (int i = 0; i < children.size(); i++ ){
			children.get(i).printSubtree();
		}
		Node.m_nodelevel--;
    }
   
    public void accept(Visitor p_visitor) {
		p_visitor.visit(this);
	}
}