#ifndef OpenGLQuery_h__
#define OpenGLQuery_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/GraphicsResource.h>

namespace RcEngine {

class _OpenGLExport OpenGLQuery : public GpuQuery
{
public:
	OpenGLQuery();
	~OpenGLQuery();

private:
	GLuint mQuery[2];
};



}

#endif // OpenGLQuery_h__
