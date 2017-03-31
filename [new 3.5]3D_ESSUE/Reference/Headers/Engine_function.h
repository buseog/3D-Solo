#ifndef Engine_function_h__
#define Engine_function_h__

BEGIN(Engine)

template <typename T>
void Safe_Delete(T& Pointer)
{
	if (Pointer)
	{
		delete Pointer;
		Pointer = NULL;
	}
}

template <typename T>
void Safe_DeleteArray(T& Pointer)
{
	if (Pointer)
	{
		delete [] Pointer;
		Pointer = NULL;
	}
}

template <typename T>
unsigned long Safe_Release(T& Pointer)
{
	unsigned long dwRefCnt = 0;

	if (Pointer)
	{
		dwRefCnt = Pointer->Release();

		if (dwRefCnt == 0)
			Pointer = NULL;
	}
	return dwRefCnt;
}

template<typename T>
float GetRandomFloat(T low, T high)
{
	if (low >= high)
		return low;

	float fRandom = (float)(rand() % 10000) * 0.0001f;

	return (fRandom * (high - low) + low);
}

template<typename T>
void GetRandomVector( T* out, T* min, T* max) 
{
	out->x = GetRandomFloat(min->x, max->x);
	out->y = GetRandomFloat(min->y, max->y);
	out->z = GetRandomFloat(min->z, max->z);
}

END
#endif // Engine_function_h__
