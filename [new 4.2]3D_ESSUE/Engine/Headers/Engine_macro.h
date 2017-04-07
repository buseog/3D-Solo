#ifndef Engine_macro_h__
#define Engine_macro_h__

namespace Engine
{
	#define BEGIN(NAMESPACE)	namespace NAMESPACE {
	#define END					}

	#define USING(NAMESPACE)	using namespace NAMESPACE;

	#define MSG_BOX(STRING)		MessageBox(NULL, TEXT(STRING), TEXT("ERROR"), MB_OK)

	#ifdef ENGINE_EXPORTS
	#define ENGINE_DLL _declspec(dllexport)
	#else
	#define ENGINE_DLL _declspec(dllimport)
	#endif

	#define NO_COPY(CLASSNAME)									\
		private:												\
		CLASSNAME(const CLASSNAME&);							\
		CLASSNAME& operator = (const CLASSNAME&);				

	#define DECLARE_SINGLETON(CLASSNAME)						\
		NO_COPY(CLASSNAME)										\
		private:												\
		static CLASSNAME*	m_pInstance;						\
		public:													\
		static CLASSNAME*	GetInstance( void );				\
		static unsigned long DestroyInstance( void );					

	#define IMPLEMENT_SINGLETON(CLASSNAME)						\
		CLASSNAME*	CLASSNAME::m_pInstance = NULL;				\
		CLASSNAME*	CLASSNAME::GetInstance( void )	{			\
		if(NULL == m_pInstance) {								\
		m_pInstance = new CLASSNAME;							\
		}														\
		return m_pInstance;										\
		}														\
		unsigned long CLASSNAME::DestroyInstance( void ) {		\
		unsigned long dwRefCnt = 0;								\
		if(NULL != m_pInstance)	{								\
		dwRefCnt = m_pInstance->Release();						\
		if (dwRefCnt == 0)										\
		m_pInstance = NULL;										\
		}														\
		return dwRefCnt;										\
		}
}

#endif // Engine_macro_h__
