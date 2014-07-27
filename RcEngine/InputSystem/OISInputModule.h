#include "OISInputPrerequisites.h"
#include "OISInputSystem.h"
#include "Input/InputModule.h"

namespace RcEngine {

class _InputExport OISInputModule : public InputModule
{
public:
	OISInputModule();
	~OISInputModule();

	virtual const String& GetName() const;
	virtual void Initialise();
	virtual void Shutdown();

	virtual InputSystem* GetInputSystem();

private:
	OISInputSystem* mInputSystem;
};

}
