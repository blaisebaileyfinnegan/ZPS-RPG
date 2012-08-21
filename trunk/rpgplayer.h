#ifndef RPG_PLAYER_H
#define RPG_PLAYER_H

#include "extension.h"
#include "vector.h"
#include "dlls/iplayerinfo.h"
#include "utlvector.h"
#include "rpgdefs.h"
#include "extension.h"
#include "predictioncopy.h"
class CRPGPlayer
{
public: // GENERAL FUNCTIONS
	CRPGPlayer()
	{
		Clear();
	}

	void Init( int clientIndex );
	void InitAccount( void );

	IPlayerInfo *GetPlayerInfo( void );
	void SetPlayerInfo( IPlayerInfo *);

	void SetBaseEntity( CBaseEntity *);
	CBaseEntity *GetBaseEntity( void );

	edict_t *GetEdict( void );

	void SetEdict( edict_t *pEdict );
	void SetIndex( int );

	int GetIndex( void );

	const char *GetAuthId( void )
	{
		if (!m_playerinfo)
			return NULL;
		return GetPlayerInfo()->GetNetworkIDString();
	};

	void Clear()
	{
		m_index = 0;
		m_edict = NULL;
		m_baseent = NULL;
		m_playerinfo = NULL;

		m_bSynchronized = false;
		m_bExperienceChanged = false;
		m_bHumanClassChanged = false;
		m_bZombieClassChanged = false;

		m_iZombieClass = RPG_CLASS_NONE;
		m_iHumanClass = RPG_CLASS_NONE;

		m_iLevel = 0;

		m_iFreeSkills = 0;
		m_iUsedSkills = 0;

		m_bWeaponRateSet = false;
	}

private:
	int m_index;
	edict_t *m_edict;
	CBaseEntity *m_baseent;
	IPlayerInfo *m_playerinfo;

public: // RPG FUNCTIONS
	bool IsSynchronized();
	void LoadClassInfo(int classnum);

	int GetLevel();
	void SetLevel( int level );
	void AddLevel( int level );

	int GetExperience();
	void SetExperience( int experience );
	void AddExperience( int experience );

	void ResetAccount();

	void SetHumanClass( int rpgclass );
	int GetHumanClass();

	int GetZombieClass();
	void SetZombieClass( int rpgclass );

	int GetSkillLevel( int skill );
	void SetSkillLevel( int skill, int level);

	int GetFreeSkills();
	int GetUsedSkills();

	void ShowBaseMenu();

	void ShowClassMenu( int team );
	void ShowSkillMenu();
	void ShowChar();

	int GetSQLIndex() const { return m_iSQLIndex; }
	void SetSQLIndex(int val) { m_iSQLIndex = val; }

	int GetCurrentClass();

	int GetCachedTeam() const { return m_iCachedTeam; }
	void SetCachedTeam(int val) { m_iCachedTeam = val; }

	int GetCachedClass() const { return m_iCachedClass; }
	void SetCachedClass(int val) { m_iCachedClass = val; }

	RPGSkill* GetSkill(int skill_index);

	void SetHealth(int health);
	int GetHealth();
	void SetMoveSpeed(float speed);
	float GetMoveSpeed();

	int GetPlayerButtons();
	EHANDLE GetActiveWeapon();
	bool IsOnGround();

	void InitOffsets();

	// DEH SKILLS
	void StartCharge();
	void EndCharge();

	void ResetBrutePassives();
	void ResetBerserkerPassives();

	void LoadBrutePassives();
	void LoadBerserkerPassives();

	int GetOffset(const char *classname, const char *offset);

	// hooks
	void ChargeTouch(CBaseEntity *pOther);
	void WeaponTouch(CBaseEntity *pOther);
	void WeaponDrop( const Vector &vecVelocity);
	float GetFireRate();

	void PlayerDropWeapon_Prehandler();

	float m_flChargeStart;
	bool m_bUsingCharge;

	/***********************************
	Let the DB know if any of this has changed:
	***********************************/
	bool m_bLevelChanged;
	bool m_bExperienceChanged;
	bool m_bHumanClassChanged;
	bool m_bZombieClassChanged;

	/***********************************
	If it has, store the corresponding data
	***********************************/
	int m_iExperience;
	int m_iLevel;
	int m_iHumanClass;
	int m_iZombieClass;

	int m_iCachedTeam;
	int m_iCachedClass;

	RPGSkill skills[MAX_SKILLS];
	RPGSkill oldskills[MAX_SKILLS];

	bool m_bWeaponRateSet;

private:
	int m_iFreeSkills;
	int m_iUsedSkills;

	int m_iSQLIndex;
	bool m_bSynchronized;
	
	int m_iButtonsOffset;
	int m_iNextPrimaryAttackOffset;
};

extern CUtlVector<CRPGPlayer*> players;
extern ExperienceToLevel ExperienceTable[];

CRPGPlayer *FindPlayerByIndex( int index );
CRPGPlayer *FindPlayerByBase( CBaseEntity *pEntity );

#endif
