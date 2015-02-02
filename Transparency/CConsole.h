#ifndef _CConsole_HG_
#define _CConsole_HG_

#include <Windows.h>
#include <string>

class CConsole
{
public:
	~CConsole();
	static CConsole* getConsole(void);
	DWORD WriteToConsole( std::wstring text );
	DWORD WriteLineToConsole( std::wstring text );
private:
	CConsole();
	HANDLE m_hConsoleOutput;
	static CConsole* m_pConsole;
};

#endif