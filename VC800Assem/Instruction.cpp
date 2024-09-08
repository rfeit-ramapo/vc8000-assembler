#include "stdafx.h"

#include "Instruction.h"
#include "SymTab.h"
#include "TransStmt.h"
#include "Errors.h"

/**/
/*
NAME

        Instruction::LocationNextInstruction - computes the location of the next instruction.

SYNOPSIS

        int Instruction::LocationNextInstruction(int a_loc) const;
            a_loc   --> the location of the current instruction.

DESCRIPTION

        This function computes the location of the next instruction based on the current instruction's OpCode and operands. If the current instruction is an "org"
        (origin) instruction, it sets the location to the value specified by its operand. If it is a "ds" (define storage) instruction, it adds the value of its
        operand to the current location to determine the next location. Comments and end statements do not consume any space, so their location remains the same
        as the current location. For all other instructions, the location of the next instruction is one word ahead of the current location.


RETURNS

       This function returns the integer location of the next instruction.

*/
/**/
int Instruction::LocationNextInstruction(int a_loc) const {
    // Assembly "org" instruction defines the location of the next instruction.
    if (m_NumOpCode == SymbolicOpCode::OC_ORG) {
        return m_Operand1NumericValue;
    }

    // Assembly "ds" (define storage) instruction specifies amount to allocate for storage.
    else if (m_NumOpCode == SymbolicOpCode::OC_DS) {
        // If the DS operation has an invalid value, just ignore it and set the next location to be one word later.
        if (m_Operand1NumericValue < 1 || m_Operand1NumericValue > 999'999)
        {
            return a_loc + 1;
        }
        // Otherwise, reserve the desired amount of storage.
        return a_loc + m_Operand1NumericValue;
    }

    // Comments and end statements do not take up any space.
    else if (m_NumOpCode == SymbolicOpCode::OC_COMM || m_NumOpCode == SymbolicOpCode::OC_END) {
        return a_loc;
    }

    // All other instructions take up one word of space.
    return a_loc + 1;
}
/* int Instruction::LocationNextInstruction(int a_loc) const */

/**/
/*
NAME

        Instruction::ParseInstruction - parses an instruction and returns its type.

SYNOPSIS

        Instruction::InstructionType Instruction::ParseInstruction(string a_line);
            a_line   --> the instruction string to be parsed, passed by value.

DESCRIPTION

        This function parses the instruction provided in the string 'a_line'. It records the original statement, deletes any comment from the line, and
        records the label, opcode, and operands. Any formatting errors are flagged by the 'isFormatError' boolean variable. If opcode or formatting
        errors are found, the instruction's error message variable is updated. The function returns the type of the instruction, which is determined
        based on its opcode and operands. The instruction type is handled within the RecordFields function.



RETURNS

       Returns the instruction's type.

*/
/**/
Instruction::InstructionType Instruction::ParseInstruction(string a_line)
{
    // Record the original statement.  This will be needed in the second pass.
    m_instruction = a_line;

    // Delete any comment from the line.
    DeleteComment( a_line );

    // Record label, opcode, and operands.  Up to you to deal with formatting errors.
    bool isFormatError = RecordFields( a_line );

    // If there was a format error, this means there were extra operands.
    if (isFormatError) Errors::RecordError("Error: extra operands.");

    // If the opcode is set as an error, report it.
    if (m_type == InstructionType::ST_Error) Errors::RecordError("Error: invalid operation.");
    
    // Return the instruction type.  This has to be handled in the code.
    return m_type;
}
/* Instruction::InstructionType Instruction::ParseInstruction(string a_line) */

/**/
/*
NAME

        Instruction::RecordFields - record the fields that make up the instruction.

SYNOPSIS

        bool Instruction::RecordFields(const string& a_line);
            a_line   --> the instruction string to be parsed.

DESCRIPTION

        This function parses the instruction provided in the string 'a_line' and records the fields that make up the instruction, including the label, opcode, and operands.
        It also determines whether the operands are numeric and their corresponding values if they are. The opcode is converted to uppercase for comparison.
        The function uses a map to map opcode labels to their numeric values as defined in the Instruction.h file. If the opcode string cannot be found in the map,
        it indicates an opcode error. Based on the opcode, the function sets the instruction type, indicating whether it is a machine language instruction,
        an assembler instruction, or an end instruction. The function returns a boolean value indicating whether there was a format error while parsing the instruction.

RETURNS

       Returns true if there was a format error while parsing the instruction, otherwise false.

*/
/**/
bool Instruction::RecordFields(const string& a_line)
{
    // Reset error flags
    m_InvalidOpCode = false;
    m_InvalidAddr = false;
    m_InvalidReg1 = false;
    m_InvalidReg2 = false;
    m_InvalidValue = false;

    // Get the fields that make up the instruction.
    bool isFormatError = ParseLineIntoFields(a_line, m_Label, m_OpCode, m_Operand1, m_Operand2);

    // If there was a comment, record the opcode and type before returning.
    if (m_OpCode.empty() && m_Label.empty()) {
        m_NumOpCode = SymbolicOpCode::OC_COMM;
        m_type = InstructionType::ST_Comment;
        return isFormatError;
    }

    // Record whether the operands are numeric and their value if they are.
    m_IsNumericOperand1 = isStrNumber(m_Operand1);
    if (m_IsNumericOperand1) m_Operand1NumericValue = stoi(m_Operand1);

    m_IsNumericOperand2 = isStrNumber(m_Operand2);
    if (m_IsNumericOperand2) m_Operand2NumericValue = stoi(m_Operand2);

    // For the sake of comparing, convert the op code to upper case.
    for (char& c : m_OpCode)
    {
        c = toupper(c);
    }

    // Declare a map of the opcode label to the numeric values as defined in Instruction.h.
    map<string, SymbolicOpCode> opCodeMap = {
        {"ADD", SymbolicOpCode::OC_ADD},
        {"SUB", SymbolicOpCode::OC_SUB},
        {"MULT", SymbolicOpCode::OC_MULT},
        {"DIV", SymbolicOpCode::OC_DIV},
        {"LOAD", SymbolicOpCode::OC_LOAD},
        {"STORE", SymbolicOpCode::OC_STORE},
        {"ADDR", SymbolicOpCode::OC_ADDR},
        {"SUBR", SymbolicOpCode::OC_SUBR},
        {"MULTR", SymbolicOpCode::OC_MULTR},
        {"DIVR", SymbolicOpCode::OC_DIVR},
        {"READ", SymbolicOpCode::OC_READ},
        {"WRITE", SymbolicOpCode::OC_WRITE},
        {"B", SymbolicOpCode::OC_B},
        {"BM", SymbolicOpCode::OC_BM},
        {"BZ", SymbolicOpCode::OC_BZ},
        {"BP", SymbolicOpCode::OC_BP},
        {"HALT", SymbolicOpCode::OC_HALT},
        {"ORG", SymbolicOpCode::OC_ORG},
        {"DC", SymbolicOpCode::OC_DC},
        {"DS", SymbolicOpCode::OC_DS},
        {"END", SymbolicOpCode::OC_END}
    };

    // If the opcode string cannot be found, this is an opcode error.
    if (opCodeMap.find(m_OpCode) == opCodeMap.end()) {
        m_NumOpCode = SymbolicOpCode::OC_ERR;
        m_type = InstructionType::ST_Error;
        m_InvalidOpCode = true;
        return isFormatError;
    }

    // Set m_NumOpCode based on the mapping.
    m_NumOpCode = opCodeMap[m_OpCode];

    // If the opcode designates a machine language instruction:
    if (m_NumOpCode <= SymbolicOpCode::OC_HALT) m_type = InstructionType::ST_MachineLanguage;

    // Otherwise, it is either an end instruction or other assembly instruction.
    else if (m_NumOpCode == SymbolicOpCode::OC_END) m_type = InstructionType::ST_End;
    else m_type = InstructionType::ST_AssemblerInstr;

    // Return whether there was a format error.
    return isFormatError;
}
/* bool Instruction::RecordFields(const string& a_line) */

/**/
/*
NAME

        Instruction::ParseLineIntoFields - parse the instruction into fields for label, opcode, and operands.

SYNOPSIS

        bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2);
            a_line         --> the input line to be parsed.
            a_label        --> the label of the instruction.
            a_OpCode       --> the opcode of the instruction.
            a_Operand1     --> the first operand of the instruction.
            a_Operand2     --> the second operand of the instruction.
            a_ErrorMsg     --> the error messages for this instruction.

DESCRIPTION

        This function parses the input instruction line into its individual fields, including the label, opcode, and operands. It initializes the provided 
        string references to empty strings and then extracts the label, opcode, operand1, and operand2 from the input line. The function handles cases where 
        the input line has no labels and removes any commas. It returns true if there is extra data after parsing, indicating a format error.

RETURNS

       Returns true if there is extra data after parsing, indicating a format error. Otherwise, returns false.

*/
/**/
bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode,
    string& a_Operand1, string& a_Operand2)
{
    // Initialize the statement elements to empty strings.
    a_label = a_OpCode = a_Operand1 = a_Operand2 = "";

    // If this is an empty string, return indicating that it is OK.
    if (a_line.empty()) return false;

    // Get rid of any commas from the line.
    replace(a_line.begin(), a_line.end(), ',', ' ');

    // Get the elements of the line.  That is the label, op code, operand1, and operand2.
    string endStr;
    istringstream ins(a_line);
    if (a_line[0] == ' ' || a_line[0] == '\t')
    {
        a_label = "";
        ins >> a_OpCode >> a_Operand1 >> a_Operand2 >> endStr;
    }
    else
    {
        ins >> a_label >> a_OpCode >> a_Operand1 >> a_Operand2 >> endStr;
    }
    // If there is extra data, return true.
    return ! endStr.empty();
}
/* bool Instruction::ParseLineIntoFields(string a_line, string& a_label, string& a_OpCode, string& a_Operand1, string& a_Operand2); */

/**/
/*
NAME

        Instruction::isStrNumber - checks if a string represents a number.

SYNOPSIS

        bool Instruction::isStrNumber(const string& a_str);
            a_str          --> the string to be checked.

DESCRIPTION

        This function checks whether the provided string represents a numeric value. It first checks if the string is empty and returns false if so. 
        Then, it examines the string characters, allowing for a leading '-' or '+' sign. If a sign is present, it ensures that there are more 
        characters following it. Finally, the function verifies that all remaining characters are digits.

RETURNS

       Returns true if the provided string represents a numeric value, and false otherwise.

*/
/**/
bool Instruction::isStrNumber(const string& a_str)
{
    if (a_str.empty()) return false;

    // If there is a - or a plus, make sure there are more characters.
    int ichar = 0;
    if (a_str[0] == '-' || a_str[0] == '+')
    {
        ichar++;
        if (a_str.length() < 2) return false;
    }
    // Make sure that the remaining characters are all digits
    for (; ichar < a_str.length(); ichar++)
    {
        if (!isdigit(a_str[ichar])) return false;
    }
    return true;
}
/* bool Instruction::isStrNumber(const string& a_str) */

/**/
/*
NAME

        Instruction::Translate - translates an instruction into a machine code statement.

SYNOPSIS

        TransStmt Instruction::Translate(int a_loc, SymbolTable& a_st);
            a_loc            --> the location of the instruction.
            a_st             --> the symbol table used for looking up symbols.

DESCRIPTION

        This function translates the given instruction into a machine code statement using the specified location and symbol table. 
        It handles different types of instructions, such as those with registers, addresses, constants, or special cases.
        Various error checks are performed during translation to ensure the validity of the instruction operands and opcode. If 
        errors are encountered, appropriate error messages and codes are recorded.

RETURNS

       Returns a TransStmt object representing the translated machine code statement.

*/
/**/
TransStmt Instruction::Translate(int a_loc, SymbolTable& a_st)
{
    // Initialize registers, address, and constant value to -1.
    int reg1 = -1;
    int reg2 = -1;
    int addr = -1;
    int val = -1;

    // Handle register and address assignment based on opcode.
    switch (m_NumOpCode) {

        // Cases with a register and an address.
        case (Instruction::SymbolicOpCode::OC_ADD):
        case (Instruction::SymbolicOpCode::OC_SUB):
        case (Instruction::SymbolicOpCode::OC_MULT):
        case (Instruction::SymbolicOpCode::OC_DIV):
        case (Instruction::SymbolicOpCode::OC_LOAD):
        case (Instruction::SymbolicOpCode::OC_STORE):
        case (Instruction::SymbolicOpCode::OC_BM):
        case (Instruction::SymbolicOpCode::OC_BZ):
        case (Instruction::SymbolicOpCode::OC_BP):
        // Cases with an address and an ignored register.
        case (Instruction::SymbolicOpCode::OC_READ):
        case (Instruction::SymbolicOpCode::OC_WRITE):
        case (Instruction::SymbolicOpCode::OC_B):
            // Record any errors with the operands and set Register 1.
            RecordErrRegisterAddress();
            reg1 = m_Operand1NumericValue;
            // Look up the symbol and indicate if it is missing.
            if (!m_InvalidAddr && !a_st.LookupSymbol(m_Operand2, addr)) {
                Errors::RecordError("Error: label not found.");
                m_InvalidAddr = true;
            }
            // Also indicate if the symbol is multiply defined.
            if (addr == a_st.multiplyDefinedSymbol) {
                Errors::RecordError("Error: multiply defined symbol.");
                m_InvalidAddr = true;
            }
            break;

        // Cases with two registers.
        case (Instruction::SymbolicOpCode::OC_ADDR):
        case (Instruction::SymbolicOpCode::OC_SUBR):
        case (Instruction::SymbolicOpCode::OC_MULTR):
        case (Instruction::SymbolicOpCode::OC_DIVR):
            // Record any errors with the operands and set register values.
            RecordErrRegisterRegister();
            reg1 = m_Operand1NumericValue;
            reg2 = m_Operand2NumericValue;
            break;

        // Halt case: register and address both ignored.
        case (Instruction::SymbolicOpCode::OC_HALT):
            // Record errors if there are unnecessary operands.
            RecordErrHalt();
            // Set values to zero for translation.
            reg1 = 0;
            addr = 0;
            break;

        // Define constant case: Set the memory address to a particular value.
        case (Instruction::SymbolicOpCode::OC_DC):
            // Record errors with the operands and set the value.
            RecordErrDC();
            val = m_Operand1NumericValue;
            break;

        // Define storage case: Record any errors with the operands.
        case (Instruction::SymbolicOpCode::OC_DS):
            RecordErrDS();
            break;
    }

    // Create the translated statement and set its error values.
    TransStmt translated = TransStmt(m_NumOpCode, a_loc, m_instruction, reg1, reg2, addr, val);
    translated.SetErrorCodes(m_InvalidOpCode, m_InvalidReg1, m_InvalidReg2, m_InvalidAddr, m_InvalidValue);
    return translated;
}
/* TransStmt Instruction::Translate(int a_loc, SymbolTable& a_st); */

/**/
/*
NAME

        Instruction::RecordErrRegisterAddress - records errors for operations that use a register and address.

SYNOPSIS

        void Instruction::RecordErrRegisterAddress();

DESCRIPTION

        This function checks the validity of the register and address operands in the instruction. It verifies that Operand 2 
        exists, follows label conventions, and has an appropriate length. Additionally, it checks Operand 1 to ensure it is 
        numeric and within the range of 0 to 9. If any errors are encountered during validation, appropriate error messages 
        are set, and corresponding error flags are updated to indicate the presence of errors.

RETURNS

       This function does not return any value.

*/
/**/
void Instruction::RecordErrRegisterAddress() {
    // Check Operand 2: should exist and follow label conventions.
    if (m_Operand2.empty()) {
        Errors::RecordError("Error: missing operands.");
        m_InvalidAddr = true;
    }
    else if (isdigit(m_Operand2[0])) {
        Errors::RecordError("Error: Operand 2 is a label and cannot begin with a digit.");
        m_InvalidAddr = true;
    }
    if (m_Operand2.length() > 10) {
        Errors::RecordError("Error: Operand 2 is too long. Labels are a maximum of 10 characters.");
        m_InvalidAddr = true;
    }

    // Check Operand 1: should be numeric and between 0 and 9. 
    if (m_Operand1.empty()) {
        m_InvalidReg1 = true;
        return;         // Note: If operand 1 is missing, already logged error with Operand 2 check.
    }
    if (!m_IsNumericOperand1 || m_Operand1NumericValue > 9 || m_Operand1NumericValue < 0) {
        Errors::RecordError("Error: Operand 1 must be a register number between 0 and 9.");
        m_InvalidReg1 = true;
    }
}
/* void Instruction::RecordErrRegisterAddress() */

/**/
/*
NAME

        Instruction::RecordErrRegisterRegister - records errors for operations that use two registers.

SYNOPSIS

        void Instruction::RecordErrRegisterRegister();

DESCRIPTION

        This function checks the validity of the register operands in the instruction. It verifies that both operands exist,
        are numeric, and are within the range of 0 to 9. If any errors are encountered during validation, appropriate error 
        messages are set, and corresponding error flags are updated to indicate the presence of errors.

RETURNS

       This function does not return any value.

*/
/**/
void Instruction::RecordErrRegisterRegister() {
    // Check Operand 2: should exist and be numeric between 0 and 9.
    if (m_Operand2.empty()) {
        Errors::RecordError("Error: missing operands.");
        m_InvalidReg2 = true;
    }
    else if (!m_IsNumericOperand2 || m_Operand2NumericValue > 9 || m_Operand2NumericValue < 0) {
        Errors::RecordError("Error: Operand 2 must be a register number between 0 and 9.");
        m_InvalidReg2 = true;
    }

    // Check Operand 1: should be numeric and between 0 and 9.
    if (m_Operand1.empty()) {
        m_InvalidReg1 = true;
        return;
    }
    if (!m_IsNumericOperand1 || m_Operand1NumericValue > 9 || m_Operand1NumericValue < 0) {
        m_InvalidReg1 = true;
        Errors::RecordError("Error: Operand 1 must be a register number between 0 and 9.");
    }
}
/* void Instruction::RecordErrRegisterRegister() */

/**/
/*
NAME

        Instruction::RecordErrHalt - records errors for the halt operation.

SYNOPSIS

        void Instruction::RecordErrHalt();

DESCRIPTION

        This function checks the validity of a halt operation by ensuring that there are no operands. If any 
        errors are encountered during validation, appropriate error messages are set, and corresponding error 
        flags are updated to indicate the presence of errors.

RETURNS

       This function does not return any value.

*/
/**/
void Instruction::RecordErrHalt() {
    // Operands 1 and 2 should both be empty.
    if (!m_Operand1.empty() || !m_Operand2.empty()) Errors::RecordError("Error: extra operands.");
}
/* void Instruction::RecordErrHalt() */

/**/
/*
NAME

        Instruction::RecordErrDC - records errors for define constant operations.

SYNOPSIS

        void Instruction::RecordErrDC();

DESCRIPTION

        This function checks the validity of the operands in the define constant instruction. It verifies that only one operand
        exists, it is numeric, and is within the allowed range of values. If any errors are encountered during validation, 
        appropriate error messages are set, and corresponding error flags are updated to indicate the presence of errors.

RETURNS

       This function does not return any value.

*/
/**/
void Instruction::RecordErrDC() {
    // Operand 2 should be empty.
    if (!m_Operand2.empty()) Errors::RecordError("Error: extra operands.");

    // Constants must be numeric and between -999,999,999 and 999,999,999
    if (!m_IsNumericOperand1 || abs(m_Operand1NumericValue) > 999'999'999) {
        m_InvalidValue = true;
        Errors::RecordError("Error: Operand 1 must be a value between -999,999,999 and 999,999,999.");
    }
}
/* void Instruction::RecordErrDC() */

/**/
/*
NAME

        Instruction::RecordErrDS - records errors for define storage operations.

SYNOPSIS

        void Instruction::RecordErrDS();

DESCRIPTION

        This function checks the validity of the operands in the define storage instruction. It verifies that only one operand
        exists, it is numeric, and is within the allowed range of values. If any errors are encountered during validation,
        appropriate error messages are set, and corresponding error flags are updated to indicate the presence of errors.

RETURNS

       This function does not return any value.

*/
/**/
void Instruction::RecordErrDS() {
    // Operand 2 should be empty.
    if (!m_Operand2.empty()) Errors::RecordError("Error: extra operands.");

    // Space reserved must be numeric and between 1 and 999,999
    if (!m_IsNumericOperand1 || m_Operand1NumericValue < 1 || m_Operand1NumericValue > 999'999) {
        m_InvalidValue = true;
        Errors::RecordError("Error: Operand 1 must be a value between 1 and 999,999.");
    }
}
/* void Instruction::RecordErrDS() */
