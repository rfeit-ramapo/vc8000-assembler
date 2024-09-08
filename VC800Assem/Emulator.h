//
//		Emulator class - supports the emulation of VC8000 programs
//
#ifndef _EMULATOR_H      // UNIX way of preventing multiple inclusions.
#define _EMULATOR_H

#include "Translation.h"

class emulator {

public:

    const static int MEMSZ = 1'000'000;	// The size of the memory of the VC8000.
    const static int REGSZ = 10;        // The number of registers for the VC8000.

    emulator() {

         m_memory.resize(MEMSZ, 0);
         m_registers.resize(REGSZ, 0);
    }
    // Records instructions and data into simulated memory.
    bool insertMemory(int a_location, long long a_contents);
    
    // Runs the program recorded in memory.
    bool runProgram(Translation &a_trans);

private:

    vector<long long> m_memory;  	      // Memory for the VC8000
    vector<long long> m_registers;        // Registers for the VC8000

    // Extract a register and address from a machine language instruction.
    void ExtractRegAddr(long long a_code, int& a_reg, int& a_addr) {
        a_reg = (a_code / 1000000) % 10;
        a_addr = (a_code % 1000000);
    }

    // Extract two registers from a machine language instruction.
    void ExtractRegs(int a_code, int& a_reg1, int& a_reg2) {
        a_reg1 = (a_code / 1000000) % 10;
        a_reg2 = (a_code / 100000) % 10;
    }

    // Extract required information from machine code and execute the instruction.
    bool ExecuteInstruction(Instruction::SymbolicOpCode a_opcode, long long a_code, int& a_loc);

    // Check if a string contains a number. 
    bool isStrNumber(const string& a_str);


    // Functions for each operation.

    // Add a register value and address contents together; store result in the register.
    void Add(int a_reg, int a_addr, int& a_loc);

    // Subtract address contents from a register value; store result in the register.
    void Subtract(int a_reg, int a_addr, int& a_loc);

    // Multiply register value and address contents; store result in the register.
    void Multiply(int a_reg, int a_addr, int& a_loc);

    // Divide register value by address contents; store result in the register.
    bool Divide(int a_reg, int a_addr, int& a_loc);

    // Load the address contents into the specified register.
    void Load(int a_reg, int a_addr, int& a_loc);

    // Store the register value in the specified address.
    void Store(int a_reg, int a_addr, int& a_loc);

    // Add two registers together and store the result in the first register.
    void AddReg(int a_reg1, int a_reg2, int& a_loc);

    // Subtract the second register from the first; store result in the first register.
    void SubReg(int a_reg1, int a_reg2, int& a_loc);

    // Multiply two registers and store the result in the first register.
    void MultReg(int a_reg1, int a_reg2, int& a_loc);

    // Divide the first register by the second and store the result in the first register.
    bool DivReg(int a_reg1, int a_reg2, int& a_loc);

    // Read in a line and store the number found in the provided address.
    bool Read(int a_addr, int& a_loc);

    // Display the contents of the specified address.
    void Write(int a_addr, int& a_loc);

    // Go to the specified address for the next instruction.
    void Branch(int a_addr, int& a_loc);

    // Go to the specified address if the register value is less than zero.
    void BranchMinus(int a_reg, int a_addr, int& a_loc);

    // Go to the specified address if the register value is equal to zero.
    void BranchZero(int a_reg, int a_addr, int& a_loc);

    // Go to the specified address if the register value is greater than zero.
    void BranchPositive(int a_reg, int a_addr, int& a_loc);
};

#endif

