#include "OISInputPrerequisites.h"
#include "Input/InputDevice.h"

namespace RcEngine {

class _InputExport OISInputMouse : public Mouse
{
public:
	OISInputMouse(InputSystem* inputSystem, uint32_t width, uint32_t height);
	~OISInputMouse();

	void SetWindowSize(uint32_t width, uint32_t height);
	const String& GetName() const;
	void Update();

private:
	OIS::Mouse* mMouse;
};

}