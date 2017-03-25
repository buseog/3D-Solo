#ifndef function_h__
#define function_h__

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

#endif // function_h__
