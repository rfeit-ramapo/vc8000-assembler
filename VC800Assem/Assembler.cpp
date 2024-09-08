//
//      Implementation of the Assembler class.
//
#include "stdafx.h"
#include "Assembler.h"
#include "Errors.h"

// Constructor for the assembler.  Note: we are passing argc and argv to the file access constructor.
// See main program.  
Assembler::Assembler( int argc, char *argv[] )
: m_facc( argc, argv )
{
}  

/**/
/*
NAME

        Assembler::PassI - processes Pass I of the assembler, which establishes the location of all labels.

SYNOPSIS

        void Assembler::PassI( );

DESCRIPTION

        This function reads each line from the source file and parses it to determine the type of instruction.
        If the instruction is an end statement, the function exits. Otherwise, it handles labels, records them, and
        computes the location of the next instruction based on the current location.
    
        Labels can only be associated with machine language and assembler language instructions. Comments are skipped.


RETURNS

        This function does not return any value.

*/
/**/
void Assembler::PassI( ) 
{
    int loc = 0;        // Tracks the location of the instructions to be generated.

    // Successively process each line of source code.
    for( ; ; ) {

        // Read the next line from the source file.
        string line; 
        if( ! m_facc.GetNextLine( line ) ) {

            // If there are no more lines, we are missing an end statement.
            // We will let this error be reported by Pass II.
            //cout << "missing end statemtn" << endl;
            break;
        }

        // Parse the line and get the instruction type.
        Instruction::InstructionType st =  m_inst.ParseInstruction( line );

        // If this is an end statement, there is nothing left to do in pass I.
        // Pass II will determine if the end is the last statement and report an error if it isn't.
        if( st == Instruction::InstructionType::ST_End ) break;

        // Labels can only be on machine language and assembler language
        // instructions.  So, skip comments.
        if( st == Instruction::InstructionType::ST_Comment )  
        {
        	continue;
	    }
        // Handle the case where there is an error.
        
        // If the instruction has a label, record it and its location in the
        // symbol table.
        if( m_inst.isLabel( ) ) {

            m_symtab.AddSymbol( m_inst.GetLabel( ), loc );
        }
        // Compute the location of the next instruction.
        loc = m_inst.LocationNextInstruction( loc );
    }
}
/* void Assembler::PassI( ) */

/**/
/*
NAME

        Assembler::PassII - processes Pass II of the assembler, which generates machine code from the assembly 
        instructions and handles errors.

SYNOPSIS

        void Assembler::PassII( );

DESCRIPTION

        This function processes the second pass of assembly. It reads each line from the source file, parses it to 
        determine the type of instruction, translates the instruction, and adds it to the translation. If an end statement 
        is encountered, the function checks if it's the first or multiple end statements, and reports errors accordingly. 
        If no end statement is found, an error is added. Finally, the entire translation is displayed.

RETURNS

        This function does not return any value.

*/
/**/
void Assembler::PassII() {

    int loc = 0;                // Tracks the location of the instructions to be generated.
    bool reachedEnd = false;    // Tracks whether an end statement was reached.

    // Rewind the file to the beginning.
    m_facc.rewind();
    // Reset error logging to prevent duplicate messages.
    Errors::InitErrorReporting();
    
    // Successively process each line of source code.
    for ( ; ; ) {

        // Read the next line from the source file.
        string line;
        if (!m_facc.GetNextLine(line)) {
            // If there are no more lines, we are missing an end statement.
            if (!reachedEnd) Errors::RecordError("Error: Missing end statement.");
            break;
        }

        // Parse the line and get the instruction type.
        Instruction::InstructionType st = m_inst.ParseInstruction(line);

        // Translate the instruction and add it to the translation.
        m_trans.AddStatement(m_inst.Translate(loc, m_symtab));

        if (st == Instruction::InstructionType::ST_End) {
            // If we already reached the end, this statement is redundant.
            if (reachedEnd) Errors::RecordError("Error: Multiple end statements.");
            else reachedEnd = true;
        }
        else {
            // Report error if we encounter additional statements after an end statement was reached.
            // Ignore comments or blank lines.
            if (reachedEnd && st != Instruction::InstructionType::ST_Comment) {
                Errors::RecordError("Error: Additional statement following end statement.");
            }
        }

        // Add all logged errors and reset the error handler for the next line.
        m_trans.AddError(Errors::GetErrors());
        Errors::InitErrorReporting();

        // Compute the location of the next instruction.
        loc = m_inst.LocationNextInstruction(loc);
    }

    // Display the entire translation calculated by Pass II.
    m_trans.DisplayTranslation();
}
/* void Assembler::PassII() */

/**/
/*
NAME

        Assembler::InterPass - adds a buffer of user confirmation between passes of the assembler.

SYNOPSIS

        void Assembler::InterPass( );

DESCRIPTION

        This function breaks up the passes and printing of assembler information with lines and user input to clearly
        delineate each section of code.

RETURNS

        This function does not return any value.

*/
/**/
void Assembler::InterPass() {
    // Break up the program passes with user input.
    cout << "__________________________________________________________\n" << endl;
    cout << "Press Enter to continue...\n" << endl;
    cin.get();
}
/* void Assembler::InterPass() */