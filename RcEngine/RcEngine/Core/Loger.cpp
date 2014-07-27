#include <Core/Loger.h>

namespace RcEngine {

bool gLoggingEnabled = TRUE;
uint32_t gLogLevel = 10;

typedef std::list< ILogListener* > LogListenerList;
LogListenerList gListeners;

uint32_t g_dwWarningCount = 0;
uint32_t g_dwErrorCount = 0;

typedef std::list<const char*> StringList;
StringList      gWarningsList;
StringList      gErrorsList;

char gStrBuf[500];
void BroadcastMessage( uint32_t uMessageType, const char* strMsg )
{
	LogListenerList::iterator iter = gListeners.begin();
	LogListenerList::iterator end = gListeners.end();

	while( iter != end )
	{
		switch( uMessageType )
		{
		case 1:
			(*iter)->LogWarning( strMsg );
			break;
		case 2:
			(*iter)->LogError( strMsg );
			break;
		default:
			(*iter)->LogMessage( strMsg );
			break;
		}
		++iter;
	}
}

void RecordMessage( StringList& DestStringList, const char* strMessage )
{
	size_t dwLength = (size_t)strlen( strMessage );
	char* strCopy = new char[ dwLength + 1 ];

	strcpy_s( strCopy, dwLength + 1, strMessage );
	DestStringList.push_back( strCopy );
}


void EngineLogger::EnableLogging( bool bEnable )
{
	gLoggingEnabled = bEnable;
}

void EngineLogger::SetLogLevel( uint32_t uLevel )
{
	gLogLevel = uLevel;
}

uint32_t EngineLogger::GetLogLevel()
{
	return gLogLevel;
}

void EngineLogger::AddListener( ILogListener* pListener )
{
	 gListeners.push_back( pListener );
}

void EngineLogger::ClearListeners()
{
	for (auto pListener : gListeners)
		delete pListener;

	gListeners.clear();
}

void EngineLogger::GenerateLogReport( bool bEchoWarningsAndErrors /*= TRUE */ )
{
	LogMsg( 0, "%d warning(s), %d error(s).", g_dwWarningCount, g_dwErrorCount );
	if( !bEchoWarningsAndErrors )
		return;

	StringList::iterator iter = gWarningsList.begin();
	StringList::iterator end = gWarningsList.end();
	while( iter != end )
	{
		BroadcastMessage( 1, *iter );
		++iter;
	}

	iter = gErrorsList.begin();
	end = gErrorsList.end();
	while( iter != end )
	{
		BroadcastMessage( 2, *iter );
		++iter;
	}
}

void EngineLogger::ResetCounters()
{
	StringList::iterator iter = gWarningsList.begin();
	StringList::iterator end = gWarningsList.end();
	while( iter != end )
	{
		delete[] *iter;
		++iter;
	}
	gWarningsList.clear();

	iter = gErrorsList.begin();
	end = gErrorsList.end();
	while( iter != end )
	{
		delete[] *iter;
		++iter;
	}
	gErrorsList.clear();

	g_dwWarningCount = 0;
	g_dwErrorCount = 0;
}

void EngineLogger::LogCommand( uint32_t dwCommand, VOID* pData /*= NULL */ )
{
	LogListenerList::iterator iter = gListeners.begin();
	LogListenerList::iterator end = gListeners.end();

	while( iter != end )
	{
		(*iter)->LogCommand( dwCommand, pData );
		++iter;
	}
}

void EngineLogger::LogError( const char* strFormat, ... )
{
	if( !gLoggingEnabled )
		return;

	++g_dwErrorCount;

	strcpy_s( gStrBuf, "ERROR: " );
	va_list args;
	va_start( args, strFormat );
	vsprintf_s( gStrBuf + 7, ARRAYSIZE( gStrBuf ) - 7, strFormat, args );

	RecordMessage( gErrorsList, gStrBuf );
	BroadcastMessage( 2, gStrBuf );
}

void EngineLogger::LogWarning( const char* strFormat, ... )
{
	if( !gLoggingEnabled )
		return;

	++g_dwWarningCount;

	strcpy_s( gStrBuf, "WARNING: " );
	va_list args;
	va_start( args, strFormat );
	vsprintf_s( gStrBuf + 9, ARRAYSIZE( gStrBuf ) - 9, strFormat, args );

	RecordMessage( gWarningsList, gStrBuf );
	BroadcastMessage( 1, gStrBuf );
}

void EngineLogger::LogMsg( uint32_t uImportance, const char* strFormat, ... )
{
	if( !gLoggingEnabled || ( uImportance > gLogLevel ) )
		return;

	va_list args;
	va_start( args, strFormat );
	vsprintf_s( gStrBuf, strFormat, args );

	BroadcastMessage( 0, gStrBuf );
}

//////////////////////////////////////////////////////////////////////////
FileListener::FileListener()
	: mFileHandle(nullptr)
{

}

FileListener::~FileListener()
{
	StopLogging();
}

void FileListener::StartLogging( const char* strFileName )
{
	assert( mFileHandle == nullptr );
    fopen_s(&mFileHandle, strFileName, "w");
}

void FileListener::StopLogging()
{
	if (mFileHandle)
		fclose(mFileHandle);
	mFileHandle = nullptr;
}

//////////////////////////////////////////////////////////////////////////
void FileListener::LogMessage( const char* strMessage )
{
	if( mFileHandle == nullptr )
		return;
	fprintf_s(mFileHandle, "%s\r\n", strMessage);
}

ConsoleOutListener::ConsoleOutListener()
{
	mConsoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo( mConsoleHandle, &csbi );
	mDefaultConsoleTextAttributes = csbi.wAttributes;
	mBackgroundAttributes = mDefaultConsoleTextAttributes & 0x00F0;
}

ConsoleOutListener::~ConsoleOutListener()
{

}

void ConsoleOutListener::LogMessage( const char* strMessage )
{
	puts( strMessage );
}

void ConsoleOutListener::LogWarning( const CHAR* strMessage )
{
	SetConsoleTextAttribute( mConsoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY | mBackgroundAttributes );
	LogMessage( strMessage );
	SetConsoleTextAttribute( mConsoleHandle, mDefaultConsoleTextAttributes );
}

void ConsoleOutListener::LogError( const char* strMessage )
{
	SetConsoleTextAttribute( mConsoleHandle, FOREGROUND_RED | FOREGROUND_INTENSITY | mBackgroundAttributes );
	LogMessage( strMessage );
	SetConsoleTextAttribute( mConsoleHandle, mDefaultConsoleTextAttributes );
}

}
