#include <Core/Environment.h>

namespace RcEngine{

Environment::Environment()
	: mApplication(nullptr),
	  mRenderFactory(nullptr), 
	  mRenderDevice(nullptr)
{

}

Environment::~Environment()
{

}


} // Namespace RcEngine