//
// Class to hold information about translated lines of a VC8000 program.
//
#pragma once

#include "Instruction.h"

class TransStmt {

public:

	// Constructor for a translated statement.
	TransStmt
	(Instruction::SymbolicOpCode a_oc, int a_loc, string a_origStmt, int a_reg1, int a_reg2, int a_addr, int a_val)
		: m_Loc(a_loc), m_OrigStmt(a_origStmt), m_Contents(a_oc, a_reg1, a_reg2, a_addr, a_val)
	{
		// If the instruction is a comment or end statement, make sure only the original statement is printed.
		if ((a_oc == Instruction::SymbolicOpCode::OC_COMM) || (a_oc == Instruction::SymbolicOpCode::OC_END))
			m_pOrig = true;
	}

	// Get the location of this instruction.
	inline int GetLocation() const { return m_Loc; };

	// Add an error message to this translated statement.
	inline void SetErrorMsg(string a_error) {
		m_ErrorMsg = a_error;
	}

	// Set the error codes for each portion of the machine language instruction.
	inline void SetErrorCodes(bool a_opcode, bool a_reg1, bool a_reg2, bool a_addr, bool a_val) {
		m_Contents.SetErrorCodes(a_opcode, a_reg1, a_reg2, a_addr, a_val);
	}

	// Return the contents as long long (numeric format).
	long long GetNumContents() const;

	// Display this translated line, along with any errors.
	void DisplayStatement();

	// Nested "contents" class for the actual machine code contents.
	class Contents {
	
	public:

		// Constructor for contents.
		Contents(Instruction::SymbolicOpCode oc, int reg1, int reg2, int addr, int val)
			: m_OpCode(oc), m_Reg1(reg1), m_Reg2(reg2), m_Addr(addr), m_Val(val) {}

		// Set the error codes for each portion of the machine language instruction.
		inline void SetErrorCodes(bool a_opcode, bool a_reg1, bool a_reg2, bool a_addr, bool a_val) {
			m_InvalidOpCode = a_opcode;
			m_InvalidReg1 = a_reg1;
			m_InvalidReg2 = a_reg2;
			m_InvalidAddr = a_addr;
			m_InvalidValue = a_val;
		}

		// Return the contents as one string (a sequence of decimal digits).
		string GetContents() const;

	private:
		Instruction::SymbolicOpCode m_OpCode;		// The OpCode of the instruction.
		int m_Reg1 = 0;								// The first register.
		int m_Reg2 = -1;							// The second register.
		int m_Addr = -1;							// The address of the instruction.
		int m_Val = -1;								// The value stored at this memory location.

		// Flags for invalid values.
		bool m_InvalidOpCode = false;       // == true if the opcode is invalid.
		bool m_InvalidReg1 = false;         // == true if the first register value is invalid.
		bool m_InvalidReg2 = false;         // == true if the second register value is invalid.
		bool m_InvalidAddr = false;         // == true if the address label is invalid.
		bool m_InvalidValue = false;        // == true if the constant value is invalid.
	};

private:
	int m_Loc = 0;			// The location of the instruction.
	Contents m_Contents;	// The machine code contents of this statement.
	string m_OrigStmt;		// The original, untranslated assembly code.
	bool m_pOrig = false;	// Boolean that tracks whether to print only the original statement (for "End" or comments).
	string m_ErrorMsg;		// String that contains error messages associated with this statement.
};
