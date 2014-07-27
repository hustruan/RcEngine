#ifndef OpenGLVertexDeclaration_h__
#define OpenGLVertexDeclaration_h__

#include "OpenGLPrerequisites.h"
#include <Graphics/VertexDeclaration.h>

namespace RcEngine {

class _OpenGLExport OpenGLVertexDeclaration : public VertexDeclaration
{
public:
	OpenGLVertexDeclaration(const VertexElement* element, uint32_t count);
	~OpenGLVertexDeclaration();
	
	inline GLuint GetVertexArrayOGL() const { return mVertexArrayOGL; }
	
	void CreateVertexArrayOGL(const RenderOperation& operation, const Shader& vertexShader);

private:
	GLuint mVertexArrayOGL;
};

}

#endif // OpenGLVertexDeclaration_h__
