#include "Direct3D11Query.h"
#include "D3D11Device.h"

namespace RcEngine {

D3D11TimeQuery::D3D11TimeQuery()
	: mQueryD3D11(nullptr)
{	
	D3D11_QUERY_DESC desc;
	desc.Query = D3D11_QUERY_TIMESTAMP;
	desc.MiscFlags = 0;

	ID3D11Device* deviceD3D11 = gD3D11Device->DeviceD3D11;
	D3D11_VERRY(deviceD3D11->CreateQuery(&desc, &mBeginQueryD3D11));
	D3D11_VERRY(deviceD3D11->CreateQuery(&desc, &mEndQueryD3D11));
}

D3D11Query::~D3D11Query()
{
	SAFE_RELEASE(mBeginQueryD3D11);
	SAFE_RELEASE(mEndQueryD3D11);
}

void D3D11Query::BeginQuery()
{
	ID3D11DeviceContext* contextD3D11 = gD3D11Device->DeviceContextD3D11;
	contextD3D11->End(mBeginQueryD3D11);
}

void D3D11Query::EndQuery()
{
	ID3D11DeviceContext* contextD3D11 = gD3D11Device->DeviceContextD3D11;
	contextD3D11->End(mEndQueryD3D11);
}




}