#ifndef GpuQuery_h__
#define GpuQuery_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport TimeQuery
{
public:
	virtual ~TimeQuery() {}

};


class _ApiExport OcclusionQuery
{
public:
	virtual ~OcclusionQuery() {}

	virtual void BeginQuery() = 0;
	virtual void EndQuery() = 0;
};



#endif // GpuQuery_h__