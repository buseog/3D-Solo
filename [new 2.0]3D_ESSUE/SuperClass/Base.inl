
unsigned long CBase::AddRef(void)
{
	return ++m_dwRefCnt;
}

unsigned long CBase::Release(void)
{
	if (m_dwRefCnt == 0)
	{
		Free();
		delete this;
		return 0;
	}
	return m_dwRefCnt--;
}