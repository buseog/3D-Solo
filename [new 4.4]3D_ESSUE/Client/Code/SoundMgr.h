#ifndef SoundMgr_h__
#define SoundMgr_h__

#include "Export_Engine.h"
#include "BigHeaders.h"
#include "Base.h"

class CSoundMgr :public CBase
{
	DECLARE_SINGLETON(CSoundMgr)

private:
	FMOD_SYSTEM*	m_pSystem;
	FMOD_CHANNEL*	m_pBgm;
	FMOD_CHANNEL*	m_pPlayer;
	FMOD_CHANNEL*	m_pPlayerEffect;
	FMOD_CHANNEL*	m_pBoss;
	FMOD_CHANNEL*	m_pBossEffect;
	FMOD_CHANNEL*	m_pTemp;
	FMOD_CHANNEL*	m_pTemp2;
	FMOD_CHANNEL*	m_pTemp3;
	FMOD_CHANNEL*	m_pTemp4;
	FMOD_CHANNEL*	m_pTemp5;

	
	
	unsigned int	m_iVersion;

	//사운드들을 보관할 map 컨테이너...
	map<TCHAR*, FMOD_SOUND*>		m_mapSound;
public:
	map<TCHAR*, FMOD_SOUND*>*	GetSoundMap(void)
	{
		return &m_mapSound;
	}
public:
	void	Initialize(void);
	void	LoadSoundFile();
	void	PlaySound(TCHAR* pSoundKey, const _uint& iIndex);
	void	PlayBGM(TCHAR*	pSoundKey);
	void	StopBGM(TCHAR*	pSoundKey);
	void	StopSoundAll(void);

private:
	virtual void Free(void);

private:
	explicit CSoundMgr(void);
	virtual ~CSoundMgr(void);
};

#endif // SoundMgr_h__