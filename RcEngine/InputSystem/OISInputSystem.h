#include "OISInputPrerequisites.h"
#include "Input/InputSystem.h"

namespace RcEngine {

class _InputExport OISInputSystem : public InputSystem
{
public:
	OISInputSystem();
	~OISInputSystem();

	virtual const String& Name() const ;
	OIS::InputManager* GetInputManager() const	{ return mInputManager; }

private:
	OIS::InputManager* mInputManager;
};

}