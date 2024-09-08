#include "stdafx.h"
#include "Errors.h"
#include "Emulator.h"

/**/
/*
NAME

        emulator::insertMemory - records instructions and data into simulated memory.

SYNOPSIS

        bool emulator::insertMemory(int a_location, long long a_contents);
            a_location          --> the memory location to insert the contents at.
            a_contents          --> the machine code to insert at this location.

DESCRIPTION

        This function inserts machine code or constant values into the memory location specified. If the location is oustide of available memory,
        an error is recorded.

RETURNS

       Returns true if the contents were successfully inserted, and false otherwise.

*/
/**/
bool emulator::insertMemory(int a_location, long long a_contents) { 
    // Check if a_location is within bounds of memory.
    if (a_location >= emulator::MEMSZ) {
        // If not, record the error and return false to indicate failure.
        Errors::RecordError("Error: location out of bounds.");
        return false;
    }

    // Set the memory location to the contents and indicate success.
    emulator::m_memory[a_location] = a_contents;
    return true;
}
/* bool emulator::insertMemory(int a_location, long long a_contents) */

/**/
/*
NAME

        emulator::runProgram - emulates the program based on a given translation.

SYNOPSIS

        bool emulator::runProgram(Translation &a_trans);
            a_trans          --> the translated program to emulate.

DESCRIPTION

        This function emulates the process of loading the translated program into memory and
        stepping through each instruction until termination. Any errors are recorded and the
        program emulation is terminated immediately.

RETURNS

       Returns true if the program terminated without errors, and false if there was an issue.

*/
/**/
bool emulator::runProgram(Translation &a_trans) { 
    // Initialize the error recording anew.
    Errors::InitErrorReporting();

    // Go through each translated statement and insert contents.
    for (const TransStmt& stmt : a_trans.GetStatements()) {
        long long contents = stmt.GetNumContents();
        if (contents == 0) continue;

        int stmt_loc = stmt.GetLocation();
        bool success = insertMemory(stmt_loc, contents);

        if (!success) {
            Errors::RecordError("Program terminated due to error allocating memory.");
            return false;
        }
    }

    int loc = 100;
    for (; ; ) {

        // The entire contents of the instruction at the current location.
        long long code = m_memory[loc];

        // If there is no instruction here, missing halt statement.
        if (code == 0) {
            Errors::RecordError("Error: missing halt statement. Terminating program.");
            return false;
        }
        // If the code is negative, there was an error in the instruction.
        else if (code < 0) {
            Errors::RecordError("Error: bad instruction reached. Terminating program.");
            return false;
        }

        // Extract the opcode.
        Instruction::SymbolicOpCode opcode = (Instruction::SymbolicOpCode)(code / 10000000);
        if (opcode > Instruction::SymbolicOpCode::OC_HALT || opcode < Instruction::SymbolicOpCode::OC_ADD) {
            Errors::RecordError("Error: bad instruction reached. Terminating program.");
            return false;
        }

        // If HALT is reached, program terminates successfully.
        if (opcode == Instruction::SymbolicOpCode::OC_HALT) return true;

        // Execute the instruction.
        bool success = ExecuteInstruction(opcode, code, loc);

        // If something failed, return false to indicate termination.
        if (!success) return success;
    }
}
/* bool emulator::runProgram(Translation &a_trans) */

/**/
/*
NAME

        emulator::ExecuteInstruction - executes the correct instruction based on opcode and instruction contents.

SYNOPSIS

        bool emulator::ExecuteInstruction(Instruction::SymbolicOpCode a_opcode, long long a_code, int& a_loc);
            a_opcode        --> the opcode of the instruction.
            a_code          --> the full instruction contents.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function switches its functionality based on the extracted opcode. It first extracts required register
        and address information from the instruction contents, then applies the function for that particular opcode.
        If any errors are encountered during execution, it returns false.

RETURNS

       Returns true if the instruction completed without errors, and false if there was an issue.

*/
/**/
bool emulator::ExecuteInstruction(Instruction::SymbolicOpCode a_opcode, long long a_code, int& a_loc) {
    int reg1;
    int reg2;
    int addr;

    // Complete the instruction based on the op code received.
    switch (a_opcode) {

        // Cases with a register and address to extract:
    case (Instruction::SymbolicOpCode::OC_ADD):     // Add
        ExtractRegAddr(a_code, reg1, addr);
        Add(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_SUB):     // Subtract
        ExtractRegAddr(a_code, reg1, addr);
        Subtract(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_MULT):    // Multiply
        ExtractRegAddr(a_code, reg1, addr);
        Multiply(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_DIV):     // Divide
        ExtractRegAddr(a_code, reg1, addr);
        if (!Divide(reg1, addr, a_loc)) return false;
    case (Instruction::SymbolicOpCode::OC_LOAD):    // Load
        ExtractRegAddr(a_code, reg1, addr);
        Load(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_STORE):   // Store
        ExtractRegAddr(a_code, reg1, addr);
        Store(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_BM):      // Branch Minus
        ExtractRegAddr(a_code, reg1, addr);
        BranchMinus(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_BZ):      // Branch Zero
        ExtractRegAddr(a_code, reg1, addr);
        BranchZero(reg1, addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_BP):      // Branch Positive
        ExtractRegAddr(a_code, reg1, addr);
        BranchPositive(reg1, addr, a_loc);
        break;

        // Cases with an address and an ignored register:
    case (Instruction::SymbolicOpCode::OC_READ):    // Read
        ExtractRegAddr(a_code, reg1, addr);
        if (!Read(addr, a_loc)) return false;
        break;
    case (Instruction::SymbolicOpCode::OC_WRITE):   // Write
        ExtractRegAddr(a_code, reg1, addr);
        Write(addr, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_B):       // Branch
        ExtractRegAddr(a_code, reg1, addr);
        Branch(addr, a_loc);
        break;

        // Cases with two registers.
    case (Instruction::SymbolicOpCode::OC_ADDR):    // Add Reg
        ExtractRegs(a_code, reg1, reg2);
        AddReg(reg1, reg2, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_SUBR):    // Sub Reg
        ExtractRegs(a_code, reg1, reg2);
        SubReg(reg1, reg2, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_MULTR):   // Mult Reg
        ExtractRegs(a_code, reg1, reg2);
        MultReg(reg1, reg2, a_loc);
        break;
    case (Instruction::SymbolicOpCode::OC_DIVR):    // Div Reg
        ExtractRegs(a_code, reg1, reg2);
        if (!DivReg(reg1, reg2, a_loc)) return false;
    }

    // If the code reached this point, the instruction was executed successfully.
    return true;
}
/* bool emulator::ExecuteInstruction(Instruction::SymbolicOpCode a_opcode, long long a_code, int& a_loc) */

/**/
/*
NAME

        emulator::Add - add a register value and address contents together; store result in the register.

SYNOPSIS

        void emulator::Add(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to add and store the result in.
            a_addr          --> the address whose contents to add.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of the address and register, adds them, and stores the result in the
        register. If the result is larger than the word size for the VC8000, it overflows. The location is
        updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Add(int a_reg, int a_addr, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg_content = m_registers[a_reg];
    long long addr_content = m_memory[a_addr];

    // Add the contents and overflow if necessary.
    long long sum = reg_content + addr_content;
    if (sum < -999'999'999) sum % -1'000'000'000;
    else if (sum > 999'999'999) sum % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg] = sum;
    a_loc += 1;
}
/* void emulator::Add(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Subtract - subtract address contents from a register value; store result in the register.

SYNOPSIS

        void emulator::Subtract(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to subtract from and store the result in.
            a_addr          --> the address whose contents to subtract.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of the address and register, subtracts (c(reg) - c(addr)), and stores 
        the result in the register. If the result is larger than the word size for the VC8000, it overflows. The 
        location is updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Subtract(int a_reg, int a_addr, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg_content = m_registers[a_reg];
    long long addr_content = m_memory[a_addr];

    // Subtract the contents and overflow if necessary.
    long long sub = reg_content - addr_content;
    if (sub < -999'999'999) sub % -1'000'000'000;
    else if (sub > 999'999'999) sub % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg] = sub;
    a_loc += 1;
}
/* void emulator::Subtract(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Multiply - multiply register value and address contents; store result in the register.

SYNOPSIS

        void emulator::Multiply(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to multiply and store the result in.
            a_addr          --> the address whose contents to multiply.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of the address and register, multiplies them, and stores the result in the
        register. If the result is larger than the word size for the VC8000, it overflows. The location is
        updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Multiply(int a_reg, int a_addr, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg_content = m_registers[a_reg];
    long long addr_content = m_memory[a_addr];

    // Multiply the contents and overflow if necessary.
    long long res = reg_content * addr_content;
    if (res < -999'999'999) res % -1'000'000'000;
    else if (res > 999'999'999) res % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg] = res;
    a_loc += 1;
}
/* void emulator::Multiply(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Divide - divide register value by address contents; store result in the register.

SYNOPSIS

        bool emulator::Divide(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to use as the dividend, and to store the result in.
            a_addr          --> the address whose contents to use as the divisor.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of the address and register, divides (c(reg) / c(addr)), and stores
        the result in the register. If the result is larger than the word size for the VC8000, it overflows. If
        the program is attempting to divide by zero, an error is recorded and the function exits early. The
        location is updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function returns true if the division was executed successfully, and false if there was an error.

*/
/**/
bool emulator::Divide(int a_reg, int a_addr, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg_content = m_registers[a_reg];
    long long addr_content = m_memory[a_addr];

    // Return false to indicate error if trying to divide by zero.
    if (addr_content == 0) {
        Errors::RecordError("Error: division by zero. Terminating program.");
        return false;
    };

    // Subtract the contents and overflow if necessary.
    long long res = reg_content / addr_content;
    if (res < -999'999'999) res % -1'000'000'000;
    else if (res > 999'999'999) res % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg] = res;
    a_loc += 1;
    return true;
}
/* bool emulator::Divide(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Load - load the address contents into the specified register.

SYNOPSIS

        void emulator::Load(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to store the loaded value in.
            a_addr          --> the address whose contents to store in the register.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of the address and stores them in the provided register. 
        The location is updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Load(int a_reg, int a_addr, int& a_loc) {
    
    // Get the content of the memory address.
    long long addr_content = m_memory[a_addr];

    // Set register content and next instruction location.
    m_registers[a_reg] = addr_content;
    a_loc += 1;
}
/* void emulator::Load(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Store - store the register value in the specified address.

SYNOPSIS

        void emulator::Store(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to get the value from.
            a_addr          --> the address whose contents to update.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of the register and stores them in the provided memory address.
        The location is updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Store(int a_reg, int a_addr, int& a_loc) {

    // Get the content of the register.
    long long reg_content = m_registers[a_reg];

    // Set the address content and next instruction location.
    m_memory[a_addr] = reg_content;
    a_loc += 1;
}
/* void emulator::Store(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::AddReg - add two registers together and store the result in the first register.

SYNOPSIS

        void emulator::AddReg(int a_reg1, int a_reg2, int& a_loc);
            a_reg1          --> the register to add and store the result in.
            a_reg2          --> the second register, whose value is added to the first.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of two registers, adds them, and stores the result in the first
        register. If the result is larger than the word size for the VC8000, it overflows. The location is
        updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::AddReg(int a_reg1, int a_reg2, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg1_content = m_registers[a_reg1];
    long long reg2_content = m_registers[a_reg2];

    // Add the contents and overflow if necessary.
    long long sum = reg1_content + reg2_content;
    if (sum < -999'999'999) sum % -1'000'000'000;
    else if (sum > 999'999'999) sum % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg1] = sum;
    a_loc += 1;
}
/* void emulator::Add(int a_reg1, int a_reg2, int& a_loc); */

/**/
/*
NAME

        emulator::SubReg - subtract the second register from the first; store result in the first register.

SYNOPSIS

        void emulator::SubReg(int a_reg1, int a_reg2, int& a_loc);
            a_reg1          --> the register to subtract from and store the result in.
            a_reg2          --> the second register, whose value is subtracted from the first.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of two registers, subtracts them (c(reg1) - c(reg2)), and stores the 
        result in the first register. If the result is larger than the word size for the VC8000, it overflows. The 
        location is updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::SubReg(int a_reg1, int a_reg2, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg1_content = m_registers[a_reg1];
    long long reg2_content = m_registers[a_reg2];

    // Add the contents and overflow if necessary.
    long long sub = reg1_content - reg2_content;
    if (sub < -999'999'999) sub % -1'000'000'000;
    else if (sub > 999'999'999) sub % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg1] = sub;
    a_loc += 1;
}
/* void emulator::SubReg(int a_reg1, int a_reg2, int& a_loc); */

/**/
/*
NAME

        emulator::MultReg - multiply two registers and store the result in the first register.

SYNOPSIS

        void emulator::MultReg(int a_reg1, int a_reg2, int& a_loc);
            a_reg1          --> the register to multiply and store the result in.
            a_reg2          --> the second register, whose value is multiplied by the first.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of two registers, multiplies them, and stores the result in the first
        register. If the result is larger than the word size for the VC8000, it overflows. The location is
        updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::MultReg(int a_reg1, int a_reg2, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg1_content = m_registers[a_reg1];
    long long reg2_content = m_registers[a_reg2];

    // Add the contents and overflow if necessary.
    long long res = reg1_content * reg2_content;
    if (res < -999'999'999) res % -1'000'000'000;
    else if (res > 999'999'999) res % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg1] = res;
    a_loc += 1;
}
/* void emulator::MultReg(int a_reg1, int a_reg2, int& a_loc); */

/**/
/*
NAME

        emulator::DivReg - divide the first register by the second and store the result in the first register.

SYNOPSIS

        void emulator::DivReg(int a_reg1, int a_reg2, int& a_loc);
            a_reg1          --> the register to use as the dividend, and to store the result in.
            a_reg2          --> the second register, whose contents are used as the divisor.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function gets the contents of two registers, divides them (c(reg1) / c(reg2)), and stores the result 
        in the first register. If the result is larger than the word size for the VC8000, it overflows. If the
        program is attempting to divide by zero, an error is recorded and the function exits early. The location is
        updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function returns true if the division was executed successfully, and false if there was an error.

*/
/**/
bool emulator::DivReg(int a_reg1, int a_reg2, int& a_loc) {

    // Get the content of the register and memory address.
    long long reg1_content = m_registers[a_reg1];
    long long reg2_content = m_registers[a_reg2];

    // Return false to indicate error if trying to divide by zero.
    if (reg2_content == 0) {
        Errors::RecordError("Error: division by zero. Terminating program.");
        return false;
    }

    // Add the contents and overflow if necessary.
    long long res = reg1_content / reg2_content;
    if (res < -999'999'999) res % -1'000'000'000;
    else if (res > 999'999'999) res % 1'000'000'000;

    // Set the register contents and next instruction location.
    m_registers[a_reg1] = res;
    a_loc += 1;
    return true;
}
/* void emulator::DivReg(int a_reg1, int a_reg2, int& a_loc); */

/**/
/*
NAME

        emulator::Read - read in a line and store the number found in the provided address.

SYNOPSIS

        bool emulator::Read(int a_addr, int& a_loc);
            a_addr          --> the address to store the read value in.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function prints "? " and awaits a line of input. After receiving a line, it parses it into
        a number and stores the value at the provided memory location. If the input was invalid, an error is recorded 
        and the function exits early. The location is updated to the adjacent address in preparation for executing 
        the next instruction.

RETURNS

       This function returns true if the read was executed successfully, and false if there was an error.

*/
/**/
bool emulator::Read(int a_addr, int& a_loc) {

    // Print ? to indicate waiting for line input to read.
    cout << "? ";

    // Take and process input into a long long.
    string input;
    cin >> input;
    // Error if not a number, or if out of bounds.
    if (!isStrNumber(input)) {
        Errors::RecordError("Error: input was not an integer between -999,999,999 and 999,999,999. Terminating program.");
        return false;
    }
    long long val = stoll(input);
    if (val > 999'999'999 || val < -999'999'999) {
        Errors::RecordError("Error: input was not an integer between -999,999,999 and 999,999,999. Terminating program.");
        return false;
    }

    // Store the value and set next instruction location.
    m_memory[a_addr] = val;
    a_loc += 1;
    return true;
}
/* bool emulator::Read(int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Write - display the contents of the specified address.

SYNOPSIS

        void emulator::Write(int a_addr, int& a_loc);
            a_addr          --> the address to get the value from.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function retrieves the contents of a specified memory location and prints them to the console. The location is 
        then updated to the adjacent address in preparation for executing the next instruction.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Write(int a_addr, int& a_loc) {
    
    // Get the contents of the address.
    long long addr_content = m_memory[a_addr];

    // Display the contents.
    cout << to_string(addr_content) << endl;

    // Set next instruction location.
    a_loc += 1;
}
/* void emulator::Write(int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::Branch - go to the specified address for the next instruction.

SYNOPSIS

        void emulator::Branch(int a_addr, int& a_loc);
            a_addr          --> the address to jump to.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function updates the location to equal the address given, allowing the program to jump
        to a specific location in memory.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::Branch(int a_addr, int& a_loc) {

    // Set the address value to be the next instruction.
    a_loc = a_addr;

}
/* void emulator::Branch(int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::BranchMinus - go to the specified address if the register value is less than zero.

SYNOPSIS

        void emulator::BranchMinus(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to compare against zero.
            a_addr          --> the address to jump to.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function updates the location to equal the address given if the contents of the register are
        less than zero. If the contents are greater than or equal to zero, the location is updated to be the
        adjacent memory location instead.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::BranchMinus(int a_reg, int a_addr, int& a_loc) {

    // Get the contents of the register.
    long long reg_contents = m_registers[a_reg];

    // If it's less than 0, go to the address. Otherwise, go to next instruction.
    if (reg_contents < 0) a_loc = a_addr;
    else a_loc += 1;
}
/* void emulator::Branch(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::BranchZero - go to the specified address if the register value is equal to zero.

SYNOPSIS

        void emulator::BranchZero(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to compare against zero.
            a_addr          --> the address to jump to.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function updates the location to equal the address given if the contents of the register are
        equal to zero. If the contents are not equal to zero, the location is updated to be the adjacent 
        memory location instead.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::BranchZero(int a_reg, int a_addr, int& a_loc) {

    // Get the contents of the register.
    long long reg_contents = m_registers[a_reg];

    // If it's equal to 0, go to the address. Otherwise, go to next instruction.
    if (reg_contents == 0) a_loc = a_addr;
    else a_loc += 1;
}
/* void emulator::BranchZero(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::BranchPositive - go to the specified address if the register value is greater than zero.

SYNOPSIS

        void emulator::BranchPositive(int a_reg, int a_addr, int& a_loc);
            a_reg           --> the register to compare against zero.
            a_addr          --> the address to jump to.
            a_loc           --> the address of the location to update.

DESCRIPTION

        This function updates the location to equal the address given if the contents of the register are
        greater than zero. If the contents are less than or equal to zero, the location is updated to be the
        adjacent memory location instead.

RETURNS

       This function does not return any value.

*/
/**/
void emulator::BranchPositive(int a_reg, int a_addr, int& a_loc) {

    // Get the contents of the register.
    long long reg_contents = m_registers[a_reg];

    // If it's more than 0, go to the address. Otherwise, go to next instruction.
    if (reg_contents > 0) a_loc = a_addr;
    else a_loc += 1;
}
/* void emulator::BranchPositive(int a_reg, int a_addr, int& a_loc); */

/**/
/*
NAME

        emulator::isStrNumber - checks if a string represents a number.

SYNOPSIS

        bool emulator::isStrNumber(const string& a_str);
            a_str          --> the string to be checked.

DESCRIPTION

        This function checks whether the provided string represents a numeric value. It first checks if the string is empty and returns false if so.
        Then, it examines the string characters, allowing for a leading '-' or '+' sign. If a sign is present, it ensures that there are more
        characters following it. Finally, the function verifies that all remaining characters are digits.

RETURNS

       Returns true if the provided string represents a numeric value, and false otherwise.

*/
/**/
bool emulator::isStrNumber(const string& a_str)
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
/* bool emulator::isStrNumber(const string& a_str) */