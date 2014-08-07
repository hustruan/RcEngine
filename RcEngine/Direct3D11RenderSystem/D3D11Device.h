#ifndef D3D11RenderDevice_h__
#define D3D11RenderDevice_h__

#include "D3D11Prerequisites.h"
#include <Graphics/RenderDevice.h>

namespace RcEngine {

class D3D11Device;
extern D3D11Device* gD3D11Device;

class _D3D11Export D3D11Device : public RenderDevice
{
public:
	D3D11Device();
	~D3D11Device();

	void CreateRenderWindow();
	void OnWindowResize(uint32_t width, uint32_t height);
	void ToggleFullscreen(bool fs);
	void AdjustProjectionMatrix(float4x4& pOut);
	void SetSamplerState(ShaderType stage, uint32_t unit, const shared_ptr<SamplerState>& state);
	void SetBlendState(const shared_ptr<BlendState>& state, const ColorRGBA& blendFactor, uint32_t sampleMask);
	void SetRasterizerState(const shared_ptr<RasterizerState>& state);
	void SetDepthStencilState(const shared_ptr<DepthStencilState>& state, uint16_t frontStencilRef = 0, uint16_t backStencilRef = 0);
	void SetViewports(const std::vector<Viewport>& vp);
	void DispatchCompute(const EffectTechnique* technique, uint32_t threadGroupCountX, uint32_t threadGroupCountY, uint32_t threadGroupCounZ);

protected:
	void DoBindShaderPipeline(const shared_ptr<ShaderPipeline>& pipeline);
	void DoDraw(const EffectTechnique* technique, const RenderOperation& operation);

public:
	ID3D11DeviceContext* DeviceContextD3D11;
	ID3D11Device* DeviceD3D11;
	
private:
	typedef std::pair<ShaderType, uint32_t> SamplerSlot;
	std::map<SamplerSlot, shared_ptr<SamplerState>> mCurrentSamplers;
};

}



#endif // D3D11RenderDevice_h__
