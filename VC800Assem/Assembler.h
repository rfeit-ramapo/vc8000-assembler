//
//		Assembler class.  This is a container for all the components
//		that make up the assembler.
//
#pragma once 

#include "SymTab.h"
#include "Instruction.h"
#include "FileAccess.h"
#include "Emulator.h"
#include "Translation.h"
#include "Errors.h"


class Assembler {

public:
    Assembler( int argc, char *argv[] );

    // Pass I - establish the locations of the symbols
    void PassI( );

    // Pass II - generate a translation
    void PassII( );

    // InterPass - adds a buffer of user confirmation between passes of the assembler.
    void InterPass( );

    // Display the symbols in the symbol table.
    void DisplaySymbolTable() { m_symtab.DisplaySymbolTable(); }
    
    // Run emulator on the translation.
    void RunProgramInEmulator() { 
        bool success = m_emul.runProgram(m_trans);
        if (success) cout << "Program terminated successfully.";
        else Errors::DisplayErrors();
    }

private:

    FileAccess m_facc;	    // File Access object
    SymbolTable m_symtab;   // Symbol table object
    Instruction m_inst;	    // Instruction object
    Translation m_trans;    // Translation object
    emulator m_emul;        // Emulator object
};

