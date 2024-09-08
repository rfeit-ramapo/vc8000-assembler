#include "stdafx.h"

#include "TransStmt.h"
#include "Instruction.h"

/**/
/*
NAME

	TransStmt::GetNumContents - returns the contents as long long (numeric format).

SYNOPSIS

	long long TransStmt::GetNumContents( );

DESCRIPTION

	This function gets the contents of this translated statement as pure machine code.
	It takes the string format of contents, which may include '?' characters to indicate
	errors, and converts it into numeric format for use in emulation.

RETURN

	This function returns the machine code for this statement, or -1 if this statement
	has an error.

*/
/**/
long long 
TransStmt::GetNumContents() const {

	// Get the contents as a string.
	string str = m_Contents.GetContents();

	// If empty, the contents should be 0.
	if (str.empty()) return 0;

	// Look for ? characters. If any exist, this indicates an error, so return -1.
	// Note: Could also return -1 if this was a defined constant, but this would not
	// be used as an instruction, so it's not a problem.
	size_t foundErr = str.find('?');
	if (foundErr != string::npos) return -1;

	// Otherwise, convert the string to long long format.
	else return stoll(str);
}
/* bool TransStmt::GetNumContents() */

/**/
/*
NAME

	TransStmt::DisplayStatement - displays the current statement, along with any errors.

SYNOPSIS

	void TransStmt::DisplayStatement( );

DESCRIPTION

	This function prints out the location, contents, and original statement for this
	translated statement. All printed contents are formatted to fit tabularly within the
	translation table. It concludes by immediately printing errors associated with the 
	statement afterwards.

RETURN

	This function does not return any value.

*/
/**/
void TransStmt::DisplayStatement() {

	// In the cases where only the original statement should be printed.
	if (m_pOrig) cout << left << setw(26) << "" << m_OrigStmt << endl;

	// Otherwise, print the location, contents, and original statement.
	else
	{
		// Get the contents of the instruction and add a blank space for alignment if necessary.
		string content_string = m_Contents.GetContents();
		if (content_string.size() > 0 && content_string[0] != '-') {
			content_string = " " + content_string;
		}
		// Print the line.
		cout << left
			<< setw(10) << m_Loc
			<< setw(16) << content_string
			<< m_OrigStmt << endl;
	}

	// Print out any error messages directly after.
	if (!m_ErrorMsg.empty()) cout << m_ErrorMsg;
};
/* void TransStmt::DisplayStatement() */

/**/
/*
NAME

	TransStmt::Contents::GetContents - return the contents as one string (a sequence of decimal digits).

SYNOPSIS

	string TransStmt::Contents::GetContents( );

DESCRIPTION

	This function returns the contents in string format, as a sequence of decimal digits.
	It handles cases where certain values are invalid by using '?' in place of the bad value.
	It also ensures negative values are printed properly with the negative sign at the start
	of the string.

RETURN

	This function returns the string representation of the contents.

*/
/**/
string TransStmt::Contents::GetContents() const
{
	ostringstream oss;		// String stream to add machine code to.

	// If the opcode or constant value is invalid, we cannot translate the instruction at all.
	if (m_InvalidOpCode || m_InvalidValue) {
		oss << "?????????";
		return oss.str();
	}

	// If the operation defines a constant, output that value.
	if (m_OpCode == Instruction::SymbolicOpCode::OC_DC) {
		// Add the negative sign to the front if the constant is negative.
		string negSign = (m_Val < 0) ? "-" : "";
		oss << negSign << setw(9) << setfill('0') << abs(m_Val);
	}
	// If the operation can be converted to machine language:
	if (m_OpCode <= Instruction::SymbolicOpCode::OC_HALT) {
		// Add the opcode to the beginning
		oss << setw(2) << setfill('0') << (int)m_OpCode;

		// Add the register 1 value, or ? if it is invalid.
		if (m_InvalidReg1) oss << "?";
		else oss << m_Reg1;

		// Add register 2 value if it exists, or ? if invalid.
		if (m_InvalidReg2) oss << "?" << "00000";
		else if (m_Reg2 >= 0) oss << m_Reg2 << "00000";

		// Add the address value if it exists, or series of ? if invalid.
		if (m_InvalidAddr) oss << "??????";
		else if (m_Addr >= 0) oss << setw(6) << setfill('0') << m_Addr;
	}
	return oss.str();

}
/* void TransStmt::Contents::GetContents() */
