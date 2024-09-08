//
// Class to parse and provide information about instructions.
//
#pragma once

// Forward declarations required for Instruction::Translate to avoid circular dependencies.
class TransStmt;    // Forward declaration
class SymbolTable;  // Forward declaration

// The elements of an instruction.
class Instruction {

public:

    // Codes to indicate the type of instruction we are processing.  Why is this inside the
    // class?  We should make this an enum class.  We will do this during a lecture.
    enum class InstructionType {
        ST_MachineLanguage, 	// A machine language instruction.
        ST_AssemblerInstr,      // Assembler Language instruction.
        ST_Comment,             // Comment or blank line
        ST_End,                 // end instruction.
        ST_Error                // Statement has an error.
    };

    // Enum class to store the different possible symbolic op codes.
    // Also includes codes for Op Code errors, comment lines, and assembly instructions.
    enum class SymbolicOpCode {
        OC_ERR,                 // Op code has an error (not any of the below designations).
        OC_ADD,                 // ADD:             Reg <-- c(Reg) + c(ADDR)
        OC_SUB,                 // SUBTRACT:        Reg <-- c(Reg) - c(ADDR)
        OC_MULT,                // MULTIPLY:        Reg <-- c(Reg) * c(ADDR)
        OC_DIV,                 // DIVIDE:          Reg <-- c(Reg) / c(ADDR)
        OC_LOAD,                // LOAD:            Reg <-- c(ADDR)
        OC_STORE,               // STORE:           ADDR <-- c(Reg)
        OC_ADDR,                // ADD REG:         REG1 <--c(REG1) + c(REG2)
        OC_SUBR,                // SUB REG:         REG1 <--c(REG1) - c(REG2)
        OC_MULTR,               // MULT REG:        REG1 <--c(REG1) * c(REG2)  
        OC_DIVR,                // DIV REG:         REG1 <--c(REG1) / c(REG2)
        OC_READ,                // READ:            A line is read in and the number found there is recorded in the specified memory address.  The register value is ignored.
        OC_WRITE,               // WRITE:           c(ADDR) is displayed  The register value is ignored.
        OC_B,                   // BRANCH:          go to ADDR for next instruction.  The register value is ignored.
        OC_BM,                  // BRANCH MINUS:    go to ADDR if c(Reg) < 0
        OC_BZ,                  // BRANCH ZERO:     go to ADDR if c(Reg) = 0
        OC_BP,                  // BRANCH POSITIVE: go to ADDR if c(Reg) > 0
        OC_HALT,                // HALT:            terminate execution.  The register value and address are ignored.
        OC_ORG,                 // ORIGIN:          ASSEM INSTRUCTION - The operand specifies the address at which the translation of the next instruction will be generated.
        OC_DC,                  // DEFINE STORAGE:  ASSEM INSTRUCTION - The operand specifies the number of words of storage to be set aside.
        OC_DS,                  // DEFINE CONSTANT: ASSEM INSTRUCTION - The constant is a decimal integer placed in the operand field.
        OC_END,                 // END:             ASSEM INSTRUCTION - Indicates there are no additional statements to translate.
        OC_COMM                 // No operation - this is a comment line.
    };

    // Parse the Instruction to record its fields and return its type.
    InstructionType ParseInstruction(string a_line);

    // Translate the instruction to machine language and store information about the line.
    TransStmt Translate(int a_loc, SymbolTable& a_st);

    // Compute the location of the next instruction.
    int LocationNextInstruction(int a_loc) const;

    // To access the label.
    inline string &GetLabel( ) {

        return m_Label;
    };

    // To determine if a label is blank.
    inline bool isLabel( ) {

        return ! m_Label.empty();
    };


private:

    // The elemements of a instruction
    string m_Label;        // The label.
    string m_OpCode;       // The symbolic op code.
    string m_Operand1;     // The first operand. 
    string m_Operand2;     // The second operand.

    string m_instruction;    // The original instruction.

    // Derived values.
    SymbolicOpCode m_NumOpCode = Instruction::SymbolicOpCode::OC_ERR;   // The numerical value of the op code for machine language equivalents.
    InstructionType m_type = InstructionType::ST_Error;                 // The type of instruction.

    bool m_IsNumericOperand1 = false;   // == true if the operand 1 is numeric.
    int m_Operand1NumericValue = 0;     // The value of the operand 1 if it is numeric.
    
    bool m_IsNumericOperand2 = false;   // == true if the operand 2 is numeric.
    int m_Operand2NumericValue = 0;     // The value of the operand 2 if it is numeric.

    // Flags for invalid values.
    bool m_InvalidOpCode = false;       // == true if the opcode is invalid.
    bool m_InvalidReg1 = false;         // == true if the first register value is invalid.
    bool m_InvalidReg2 = false;         // == true if the second register value is invalid.
    bool m_InvalidAddr = false;         // == true if the address label is invalid.
    bool m_InvalidValue = false;        // == true if the constant value is invalid.

    // Delete any comments from the statement.
    void DeleteComment(string &a_line)
    {
        size_t isemi1 = a_line.find(';');
        if (isemi1 != string::npos)
        {
            a_line.erase(isemi1);
        }
    }

    // Record the fields of the instructions.
    bool RecordFields( const string &a_line );

    // Get the fields that make up the statement.  This function returns false if there
    // are extra fields.
    bool ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode,
        string& a_Operand1, string& a_Operand2);

    // Check if a string contains a number. 
    bool isStrNumber(const string& a_str);


    // Functions to record errors for each instruction type.

    // Record errors for instructions that require an address and register 1.
    void RecordErrRegisterAddress();

    // Record errors for instructions that require two register values.
    void RecordErrRegisterRegister();

    // Record errors for the halt instruction (no operands).
    void RecordErrHalt();

    // Record errors for the define constant instruction (1 operand to define the value).
    void RecordErrDC();

    // Record errors for the define storage instruction (1 operand to define storage reserved).
    void RecordErrDS();

};

