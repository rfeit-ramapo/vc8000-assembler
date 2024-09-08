//
// Class to manage error reporting. Note: all members are static so we can access them anywhere.
// What other choices do we have to accomplish the same thing?
//
#ifndef _ERRORS_H
#define _ERRORS_H

#include <string>
#include <vector>

class Errors {

public:
    
    // Initializes error reports by clearing existing error messages from the vector.
    static inline void InitErrorReporting() {
        m_ErrorMsgs.clear();
    }

    // Records an error message.
    static inline void RecordError(string a_emsg) {
        m_ErrorMsgs.push_back(a_emsg);
    }

    // Displays the collected error messages.
    static inline void DisplayErrors() {
        for (string e : m_ErrorMsgs) cout << e << endl;
    }

    // Returns a string containing all collected error messages.
    static inline string GetErrors() {
        string errs;
        for (string e : m_ErrorMsgs) errs += e + "\n";
        return errs;
    }

private:

    static vector<string> m_ErrorMsgs;  // This must be declared in the .cpp file.
};
#endif