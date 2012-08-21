#include "extension.h"
#include "rpgplayer.h"

class CAccount
{
public:
	bool IsSynchronized();

	int GetLevel();
	void SetLevel( int level );

	int GetExperience();
	void SetExperience( int );

	CRPGPlayer *GetPlayerParent()
	{
		return m_pParent;
	};

	void SetPlayerParent( CRPGPlayer *pPlayer )
	{
		m_pParent = pPlayer;
	}

private:
	CRPGPlayer *m_pParent;

	bool m_bLevelChanged;

	int m_iExperience;
	int m_iLevel;

	bool m_bSynchronized;
};