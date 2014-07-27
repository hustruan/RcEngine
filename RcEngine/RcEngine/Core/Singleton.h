#ifndef _Singleton__H
#define _Singleton__H

namespace RcEngine{

template <typename T> 
class Singleton
{
public:
	static void Initialize()
	{
		new T();
	}

	static void Finalize()
	{
		delete ms_Singleton;
	}

	static T& GetSingleton( void )
	{	
		assert( ms_Singleton ); 
		return ( *ms_Singleton ); 
	}

	static T* GetSingletonPtr( void )
	{ 
		return ms_Singleton;
	}
	
protected:	
	Singleton( void )
	{
		assert( !ms_Singleton );
		ms_Singleton = static_cast<T*>( this );
	}

	virtual ~Singleton( void )
	{  
		assert( ms_Singleton ); 
		ms_Singleton = 0; 
	}
	
	static T* ms_Singleton;

private:
	Singleton(const Singleton<T> &);
	Singleton& operator=(const Singleton<T> &);	
};

template <typename T> T* Singleton<T>::ms_Singleton = nullptr;

} // Namespace RcEngine


#endif