#include "OpenGLVertexDeclaration.h"
#include "OpenGLGraphicCommon.h"
#include "OpenGLBuffer.h"
#include <Graphics/RenderOperation.h>

namespace RcEngine {

#define BUFFER_OFFSET(offset) ((void *)(offset))

OpenGLVertexDeclaration::OpenGLVertexDeclaration( const VertexElement* element, uint32_t count )
	: VertexDeclaration(element, count),
	  mVertexArrayOGL(0)
{

}

OpenGLVertexDeclaration::~OpenGLVertexDeclaration()
{
	if (mVertexArrayOGL)
		glDeleteVertexArrays(1, &mVertexArrayOGL);
}

void OpenGLVertexDeclaration::CreateVertexArrayOGL( const RenderOperation& operation, const Shader& vertexShader )
{
	glGenVertexArrays(1, &mVertexArrayOGL);
	glBindVertexArray(mVertexArrayOGL);

	OpenGLVertexDeclaration* vertexDeclOGL = static_cast_checked<OpenGLVertexDeclaration*>(operation.VertexDecl.get());

	GLuint vertexSlotBind = -1;
	for (GLuint attribIndex = 0; attribIndex < vertexDeclOGL->mVertexElemets.size(); ++attribIndex)
	{
		const VertexElement& attribute = vertexDeclOGL->mVertexElemets[attribIndex];

		// Bind vertex stream to slot
		if (attribute.InputSlot != vertexSlotBind)
		{
			OpenGLBuffer* bufferOGL = static_cast_checked<OpenGLBuffer*>(operation.VertexStreams[attribute.InputSlot].get());
			glBindBuffer(GL_ARRAY_BUFFER, bufferOGL->GetBufferOGL());

			vertexSlotBind = attribute.InputSlot;
		}

		GLenum type = OpenGLMapping::Mapping(attribute.Type);
		uint32_t size = VertexElementUtil::GetElementComponentCount(attribute);
		uint32_t stride =  vertexDeclOGL->GetStreamStride(attribute.InputSlot);
		uint32_t offset = attribute.Offset;
			
		bool isMatchVertexShader = true;
		
		if (isMatchVertexShader)
		{
			glEnableVertexAttribArray(attribIndex);

			if (OpenGLMapping::IsIntegerType(type))
				glVertexAttribIPointer(attribIndex, size, type, stride, BUFFER_OFFSET(offset));	
			else
				glVertexAttribPointer(attribIndex, size, type, false, stride, BUFFER_OFFSET(offset));	

			if (attribute.InstanceStepRate > 0)
				glVertexAttribDivisor(attribIndex, attribute.InstanceStepRate);
		}
		else 
			glDisableVertexAttribArray(attribIndex);
	}

	glBindVertexArray(0);
}

}