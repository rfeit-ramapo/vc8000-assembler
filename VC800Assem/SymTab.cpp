//
//		Implementation of the symbol table class.  This is the format I want for commenting functions.
//
#include "stdafx.h"
#include "SymTab.h"

/**/
/*
NAME

    AddSymbol - adds a new symbol to the symbol table.

SYNOPSIS

    void AddSymbol( const string &a_symbol, int a_loc );
    	a_symbol	-> The name of the symbol to be added to the symbol table.
    	a_loc		-> the location to be associated with the symbol.

DESCRIPTION

    This function will place the symbol "a_symbol" and its location "a_loc"
    in the symbol table.
*/
/**/
void 
SymbolTable::AddSymbol( const string &a_symbol, int a_loc )
{
    // If the symbol is already in the symbol table, record it as multiply defined.
    map<string, int>::iterator st = m_symbolTable.find( a_symbol );
    if( st != m_symbolTable.end() ) {

        st->second = multiplyDefinedSymbol;
        return;
    }
    // Record a the location in the symbol table.
    m_symbolTable[a_symbol] = a_loc;
}
/* void SymbolTable::AddSymbol( const string &a_symbol, int a_loc ) */

/**/
/*
NAME

    SymbolTable::DisplaySymbolTable() - displays the symbol table.

SYNOPSIS

    void AddSymbol( const string &a_symbol, int a_loc );
        a_symbol	-> The name of the symbol to be added to the symbol table.
        a_loc		-> the location to be associated with the symbol.

DESCRIPTION

    This function will display all symbols and locations recorded in the symbol table.

RETURNS

    This function does not return any value.

*/
/**/
void
SymbolTable::DisplaySymbolTable()
{
    // Print header
    cout << setw(10) << "Symbol #" << setw(15) << "Symbol" << setw(15) << "Location" << endl;

    // Iterate through the map and print each entry
    int index = 0;
    for (const auto& pair : m_symbolTable) {
        cout << setw(10) << index++
            << setw(15) << pair.first
            << setw(15) << pair.second << endl;
    }

}
/* void SymbolTable::DisplaySymbolTable() */

/**/
/*
NAME

    SymbolTable::LookupSymbol - searches for a symbol within the symbol table.

SYNOPSIS

    bool SymbolTable::LookupSymbol( const string& a_symbol, int& a_loc );
        a_symbol	-> the name of the symbol to look up.
        a_loc		-> the location to record the symbol value to.

DESCRIPTION

    This function will place look up the symbol and assign the stored location to a_loc.

RETURN
    
    This function returns true if the symbol was found, and false if it was not.

*/
/**/
bool SymbolTable::LookupSymbol(const string& a_symbol, int& a_loc)
{
    auto it = m_symbolTable.find(a_symbol); // Check if the symbol exists in the map
    if (it != m_symbolTable.end()) { // Symbol found
        a_loc = it->second; // Assign the location to a_loc
        return true; // Return true to indicate success
    }
    return false;

}
/* bool SymbolTable::LookupSymbol(const string& a_symbol, int& a_loc) */