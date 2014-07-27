#ifndef EffectAnnotation_h__
#define EffectAnnotation_h__

#include <Core/Prerequisites.h>

namespace RcEngine {

class _ApiExport EffectAnnotation
{
public:
	EffectAnnotation();
	virtual~ EffectAnnotation();

	const String& GetAnnotationName() const		{ return mName; }

protected:
	String mName;
};

typedef vector<EffectAnnotation*> EffectAnnotationList;

}

#endif // EffectAnnotation_h__