//
// Class to hold the program translation.
//
#pragma once

#include "TransStmt.h"

class Translation {

public:

	// Display all translated lines.
	inline void DisplayTranslation() const {
		// Print header
		cout << left << setw(11) << "Location" << setw(15) << "Contents" << "Original Statement" << endl;

		// Display each line of the translation.
		for (TransStmt stmt : m_Stmts) {
			stmt.DisplayStatement();
		}
	};

	// Add a new translated statement to the vector.
	inline void AddStatement(TransStmt a_stmt) {
		m_Stmts.push_back(a_stmt);
	}

	// Get the vector of translated statements.
	inline const vector<TransStmt>& GetStatements() const {
		return m_Stmts;
	}

	// Add an error message to the latest translated statement.
	inline void AddError(string a_error) {
		TransStmt &lastStatement = m_Stmts.back();
		lastStatement.SetErrorMsg(a_error);
	}

private:
	// Vector to hold all of the translated statements, in order.
	vector<TransStmt> m_Stmts;
};