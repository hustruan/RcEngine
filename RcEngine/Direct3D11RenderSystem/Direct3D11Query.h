#ifndef Direct3D11Query_h__
#define Direct3D11Query_h__

#include "D3D11Prerequisites.h"
#include <Graphics/GpuQuery.h>

namespace RcEngine {

class _D3D11Export D3D11TimeQuery : public TimeQuery
{
public:
	D3D11TimeQuery();
	~D3D11TimeQuery();

private:
	ID3D11Query* mQueryD3D11;
};

}

#endif // Direct3D11Query_h__
