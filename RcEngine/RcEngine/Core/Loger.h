#ifndef Loger_h__
#define Loger_h__

#include <Core/Prerequisites.h>

#if defined(RcWindows) 
	#include <Windows.h>
#endif 

namespace RcEngine {

class _ApiExport ILogListener
{
public:
	virtual void LogMessage( const char* strMessage ) = 0;
	virtual void LogWarning( const char* strMessage )			 { LogMessage( strMessage ); }
	virtual void LogError( const char* strMessage )				 { LogMessage( strMessage ); }
	virtual void LogCommand( uint32_t dwCommand, void* pData )   { }
};

class _ApiExport FileListener : public ILogListener
{
public:
	FileListener();
	~FileListener();

	void StartLogging( const char* strFileName );
	void StopLogging();
	virtual void LogMessage( const char* strMessage );

protected:
	FILE* mFileHandle;
};

class _ApiExport ConsoleOutListener : public ILogListener
{
public:
	ConsoleOutListener();
	~ConsoleOutListener();

	virtual void LogMessage( const char* strMessage );
	virtual void LogWarning( const char* strMessage );
	virtual void LogError( const char* strMessage );

protected:

#ifdef RcWindows
	HANDLE  mConsoleHandle;
	WORD    mDefaultConsoleTextAttributes;
	WORD    mBackgroundAttributes;
#endif

};

class _ApiExport EngineLogger
{
public:
	enum LogCommands
	{
		ELC_CLEAR = 0,
		ELC_STARTEXPORT,
		ELC_ENDEXPORT
	};

	static void EnableLogging( bool bEnable );
	static void SetLogLevel( uint32_t uLevel );
	static uint32_t GetLogLevel();
	static void AddListener( ILogListener* pListener );
	static void ClearListeners();

	static void GenerateLogReport( bool bEchoWarningsAndErrors = true );
	static void ResetCounters();

	static void LogCommand( uint32_t dwCommand, void* pData = NULL );
	static void LogError( const char* strFormat, ... );
	static void LogWarning( const char* strFormat, ... );
	static void LogMsg( uint32_t uImportance, const char* strFormat, ... );
};


}
#endif // Loger_h__
