#include "OISInputPrerequisites.h"
#include "Input/InputDevice.h"

namespace RcEngine {

class _InputExport OISInputKeyboard : public Keyboard
{
public:
	OISInputKeyboard(InputSystem* inputSystem);
	~OISInputKeyboard();

	virtual const String& GetName() const ;
	virtual void Update() ;

private:
	OIS::Keyboard* mKeyboard;	
};

}