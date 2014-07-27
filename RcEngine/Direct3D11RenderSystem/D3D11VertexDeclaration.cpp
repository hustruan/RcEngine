#include "D3D11VertexDeclaration.h"
#include "D3D11Device.h"
#include "D3D11GraphicCommon.h"
#include "D3D11Shader.h"
#include <Core/Exception.h>

#define MAX_ATTRIBUTES 8

namespace RcEngine {

D3D11VertexDeclaration::D3D11VertexDeclaration( const VertexElement* element, uint32_t count )
	:VertexDeclaration(element, count),
	  InputLayoutD3D11(nullptr)
{
}

D3D11VertexDeclaration::~D3D11VertexDeclaration(void)
{
	SAFE_RELEASE(InputLayoutD3D11);
}

void D3D11VertexDeclaration::CreateInputLayout( const RenderOperation& operation, const Shader& vertexShader )
{
	static D3D11_INPUT_ELEMENT_DESC layoutD3D11[MAX_ATTRIBUTES]; 

	const D3D11VertexShader* vertexShaderD3D11 = static_cast_checked<const  D3D11VertexShader*>(&vertexShader);

	/**
	 * May partial match
	 */
	//assert(mVertexElemets.size() == vertexShaderD3D11->InputSignatures.size());
	assert(mVertexElemets.size() >= vertexShaderD3D11->InputSignatures.size());
	for (size_t i = 0; i < vertexShaderD3D11->InputSignatures.size(); ++i) //for (size_t i = 0; i < mVertexElemets.size(); ++i)
	{
		const VertexElement& element = mVertexElemets[i];
		layoutD3D11[i].SemanticName = vertexShaderD3D11->InputSignatures[i].Semantic.c_str();
		layoutD3D11[i].SemanticIndex = element.UsageIndex;
		layoutD3D11[i].Format = D3D11Mapping::Mapping(element.Type);
		layoutD3D11[i].InputSlot = element.InputSlot;
		layoutD3D11[i].AlignedByteOffset = element.Offset;
		layoutD3D11[i].InputSlotClass = element.InstanceStepRate > 0 ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
		layoutD3D11[i].InstanceDataStepRate = element.InstanceStepRate;
	}

	// Todo: cache InputLayoutD3D11
	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	HRESULT hr = deviceD3D11->CreateInputLayout( layoutD3D11, vertexShaderD3D11->InputSignatures.size(), &vertexShaderD3D11->ShaderCode[0], vertexShaderD3D11->ShaderCode.size(), &InputLayoutD3D11 );

	if (FAILED(hr))
	{
		ENGINE_EXCEPT(Exception::ERR_INVALID_PARAMS, "Error: Vertex Input Signature Not Match!", "D3D11VertexDeclaration::CreateInputLayout");
	}
}

}

