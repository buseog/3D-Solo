#ifndef Engine_Functor_h__
#define Engine_Functor_h__

BEGIN(Engine)

class CReleaseSingle
{
public:
	explicit CReleaseSingle(void) {}
	virtual ~CReleaseSingle(void) {}

public:
	template <typename T>
	void operator () (T& Instance)
	{
		Engine::Safe_Release(Instance);
	}
};

class CReleasePair
{
public:
	explicit CReleasePair(void) {}
	virtual ~CReleasePair(void) {}

public:
	template <typename T>
	void operator () (T& Pair)
	{
		Engine::Safe_Release(Pair.second);
	}
};

class CTagFinder
{
private:
	const TCHAR*		m_pString;

public:
	template <typename T>
	bool operator () (const T& Pair)
	{
		if (lstrcmp(m_pString, Pair.first) == 0)
			return true;
		return false;
	}

public:
	explicit CTagFinder(const TCHAR* pString) : m_pString(pString) {}
	virtual ~CTagFinder(void) {}
};

END
#endif // Engine_Functor_h__
