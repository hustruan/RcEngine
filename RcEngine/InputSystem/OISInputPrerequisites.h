#ifndef OISInputPrerequisites_h__
#define OISInputPrerequisites_h__

#ifndef BUILD_STATIC
#	ifdef OISINPUTSYSTEMDLL_EXPORTS
#		define _InputExport __declspec(dllexport)
#	else
#    	define _InputExport __declspec(dllimport) 
#	endif
#else
#	define _InputExport
#endif	

#include <ois/includes/OISMouse.h>
#include <ois/includes/OISKeyboard.h>
#include <ois/includes/OISInputManager.h>

#endif // OISInputPrerequisites_h__
