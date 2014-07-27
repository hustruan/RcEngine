#ifndef IModule_h__
#define IModule_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport IModule
{
protected:
	IModule(void);

public:
	virtual ~IModule(void);

	virtual const String& GetName() const = 0;

	// Initialise module
	virtual void Initialise() = 0;
			
	// Shutdown module
	virtual void Shutdown() = 0;
};

} // Namespace RcEngine

#endif // IModule_h__