//
//		File access to source file.
//
#ifndef _FILEACCESS_H  // This is the way that multiple inclusions are defended against often used in UNIX
#define _FILEACCESS_H  // We use pragmas in Visual Studio and g++.  See other include files

#include <fstream>
#include <stdlib.h>
#include <string>

class FileAccess {

public:

    // Opens the file.
    FileAccess( int argc, char *argv[] );

    // Closes the file.
    ~FileAccess()
    {
        // Not that necessary in that the file will be closed when the program terminates, but good form.
        m_sfile.close();
    }

    // Get the next line from the source file.  Returns true if there was one.
    bool GetNextLine( string &a_line );

    // Put the file pointer back to the beginning of the file.
    void rewind( );

private:

    ifstream m_sfile;		// Source file object.
};
#endif

