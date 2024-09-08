//
//  Implementation of file access class.
//
#include "stdafx.h"
#include "FileAccess.h"

/**/
/*
NAME

        FileAccess::FileAccess - constructs a FileAccess object.

SYNOPSIS

        FileAccess::FileAccess( int argc, char *argv[] )
            argc      --> the number of arguments passed into the main function.
            argv      --> an array of arguments that were passed into the main function.

DESCRIPTION

        This function constructs the FileAccess object using the argument passed in to main()
        as the filename. If there is an issue parsing the arguments or opening the file, the program
        is terminated.

*/
/**/
FileAccess::FileAccess( int argc, char *argv[] )
{
    // Check that there is exactly one run time parameter.
    if( argc != 2 ) {
        cerr << "Usage: Assem <FileName>" << endl;
        exit( 1 );
    }
    // Open the file.  One might question if this is the best place to open the file.
    // One might also question whether we need a file access class.
    m_sfile.open( argv[1], ios::in );

    // If the open failed, report the error and terminate.
    if( ! m_sfile ) {
        cerr << "Source file could not be opened, assembler terminated."
            << endl;
        exit( 1 ); 
    }
}
/* FileAccess::FileAccess( int argc, char *argv[] ) */

/**/
/*
NAME

        FileAccess::GetNextLine - retrieves the next line from the file.

SYNOPSIS

        bool FileAccess::GetNextLine( string &a_line )
            a_line      --> a reference to a string where the next line from the file will be stored.

DESCRIPTION

        This function retrieves the next line from the file. It first checks if there is no more data in the file. If the end of the file (eof) is reached,
        it returns false to indicate that there are no more lines to read. Otherwise, it reads the next line from the file and stores it in the string parameter
        passed by reference (a_line). Upon successful retrieval of the line, the function returns true.


RETURNS

       This function returns true if the next line was successfully retrieved from the file, and false if there was no more data in the file.

*/
/**/
bool FileAccess::GetNextLine( string &a_line )
{
    // If there is no more data, return false.
    if( m_sfile.eof() ) {
    
        return false;
    }
    getline( m_sfile, a_line );
    
    // Return indicating success.
    return true;
}
/* bool FileAccess::GetNextLine( string &a_line ) */

/**/
/*
NAME

        FileAccess::rewind - rewinds the file to the beginning.

SYNOPSIS

        void FileAccess::rewind( )

DESCRIPTION

        This function rewinds the file to the beginning. It clears all file flags and sets the file pointer to the beginning of the file (offset 0). 
        By doing so, it allows subsequent reading operations to start from the beginning of the file.

RETURNS

        This function does not return any value.

*/
/**/
void FileAccess::rewind( )
{
    // Clean all file flags and go back to the beginning of the file.
    m_sfile.clear();
    m_sfile.seekg( 0, ios::beg );
}
/* void FileAccess::rewind() */