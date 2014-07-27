#ifndef D3D11VertexDeclaration_h__
#define D3D11VertexDeclaration_h__
#pragma once

#include "D3D11Prerequisites.h"
#include <Graphics/VertexDeclaration.h>

namespace RcEngine {

class _D3D11Export D3D11VertexDeclaration : public VertexDeclaration
{
public:
	D3D11VertexDeclaration(const VertexElement* element, uint32_t count);
	~D3D11VertexDeclaration(void);

	void CreateInputLayout(const RenderOperation& operation, const Shader& vertexShader);

public:

	/**
	 * Hack: InputLayoutD3D11 will be created if it used in draw call
	 *       and InputLayoutD3D11 is still NULL.
	 */
	ID3D11InputLayout* InputLayoutD3D11;
};

}




#endif // D3D11VertexDeclaration_h__