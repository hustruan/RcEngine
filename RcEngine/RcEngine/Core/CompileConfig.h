#ifndef CompileConfig_h__
#define CompileConfig_h__

namespace RcEngine
{

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#    pragma warning( disable: 4996)
#    define _CRT_SECURE_NO_DEPRECATE  1
#    define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

	// Disabling compiler warning, which may make no sence 
#if _MSC_VER >= 1200
#  pragma warning(disable : 4006) // Ignore double defined symbols warning
#  pragma warning(disable : 4786)
#  pragma warning(disable : 4251)
#  pragma warning(disable : 4100)
#  pragma warning(disable : 4661)
#endif
}


#endif // CompileConfig_h__
