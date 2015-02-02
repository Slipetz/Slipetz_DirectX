#include "CConsole.h"
#include <sstream>

// Found this code on the internet... stackoverflow, I think
//AllocConsole();
//HANDLE myConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
//DWORD cCharsWritten;
////char* str = L"enter game command";
//std::wstring textToWrite = L"Hey there\n";
//WriteConsole(myConsoleHandle, textToWrite.c_str(), textToWrite.size(), &cCharsWritten, NULL);
//WriteConsole(myConsoleHandle, textToWrite.c_str(), textToWrite.size(), &cCharsWritten, NULL);


CConsole::~CConsole()
{
	// Closes the console if no one else is using it...
	FreeConsole();
	m_pConsole = 0;
}

//static 
// This is the instance of the console
CConsole* CConsole::m_pConsole = 0;

//static 
CConsole* CConsole::getConsole(void)
{	// Do we have a console yet?
	if ( m_pConsole == 0 )
	{
		m_pConsole = new CConsole();
	}
	return m_pConsole;
}

CConsole::CConsole()
{
	AllocConsole();
	this->m_hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
}

DWORD CConsole::WriteToConsole( std::wstring text )
{
	DWORD cCharsWritten = 0;
	WriteConsole(this->m_hConsoleOutput, text.c_str(), text.size(), &cCharsWritten, NULL);
	return cCharsWritten;
}

DWORD CConsole::WriteLineToConsole( std::wstring text )
{
	std::wstringstream ss;
	ss << text << std::endl;

	DWORD cCharsWritten = 0;
	WriteConsole(this->m_hConsoleOutput, ss.str().c_str(), ss.str().size(), &cCharsWritten, NULL);
	return cCharsWritten;
}
