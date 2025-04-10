package SymbolTable;

import java.util.Vector;

public class FuncEntry extends SymTabEntry {
	
	public Vector<VarEntry> m_params   = new Vector<VarEntry>();
	
	public FuncEntry(String p_type, String p_name, Vector<VarEntry> p_params, SymTab p_table){
		super(new String("func"), p_type, p_name, p_table);
		m_params = p_params;
	}

	public String toString(){
		return 	String.format("%-12s" , "| " + m_kind) +
				String.format("%-12s" , "| " + m_name) + 
				String.format("%-28s"  , "| " + m_type) + 
				"|" + 
				m_subtable;
	}	
}
