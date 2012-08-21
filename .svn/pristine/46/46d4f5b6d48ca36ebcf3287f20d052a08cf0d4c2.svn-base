#include "rpgplayer.h"
#include "dlls/iplayerinfo.h"
#include "database.h"
#include "vfuncs.h"
#include "shareddefs.h"

#define GAME_DLL 1
#include "takedamageinfo.h"

ExperienceToLevel ExperienceTable[] = {
	{1, 0},
	{2, 200},
	{3, 500},
	{4, 1000},
	{5, 2000},
	{6, 3500},
	{7, 5000},
	{8, 8000},
	{9, 12000},
	{MAXLEVEL, 20000},
};

const char *ZombieClasses[] =
{
	"Berserker",
};

const char *HumanClasses[] =
{
	"Brute",
};

#define BASEMENUCHOICES 4

const char *BaseMenuChoices[] =
{
	"Change Class",
	"Change Skills",
	"View Character",
	"View Help",
};

const char *SkillNames[] =
{
	"Blank",
	"Reinforced Epidermis",
	"Strengthened Arms",
	"Miracle Elbows",
	"Brute Force",
	"Lightning Arms",
	"Quickened Pace",
	"Cutaneous Leech",
	"Permanent Mutation"
};



void CRPGPlayer::Init( int clientIndex )
{	
	SetIndex( clientIndex );

	edict_t *pEdict = engine->PEntityOfEntIndex( clientIndex );
	if (!pEdict)
		return;

	SetEdict( pEdict );

	CBaseEntity *pEntity = pEdict->GetUnknown()->GetBaseEntity();
	//smutils->LogMessage(myself, "in");
	if (pEntity)
	{
		//smutils->LogMessage(myself, "in");
		SetBaseEntity(pEntity);
	}

	m_playerinfo = playerinfo->GetPlayerInfo(pEdict);

	InitOffsets();
	SetCachedTeam(-1);
	SetCachedClass(-1);
}

IPlayerInfo *CRPGPlayer::GetPlayerInfo( void )
{
	return m_playerinfo;
}

void CRPGPlayer::SetPlayerInfo( IPlayerInfo *info )
{
	m_playerinfo = info;
}

void CRPGPlayer::SetBaseEntity( CBaseEntity *pEntity )
{
	m_baseent = pEntity;
}

CBaseEntity *CRPGPlayer::GetBaseEntity( void )
{
	return m_baseent;
}

edict_t *CRPGPlayer::GetEdict( void )
{
	return m_edict;
}

void CRPGPlayer::SetEdict( edict_t *pEdict )
{
	m_edict = pEdict;
}

void CRPGPlayer::SetIndex( int clientIndex )
{
	m_index = clientIndex;
}

int CRPGPlayer::GetIndex( void )
{
	return m_index;
}

void CRPGPlayer::InitAccount()
{
	m_bSynchronized = true;
	m_bHumanClassChanged = false;
	m_bZombieClassChanged = false;
	m_bLevelChanged = false;
	m_bExperienceChanged = false;
}

void CRPGPlayer::SetZombieClass( int rpgclass )
{
	m_iZombieClass = rpgclass;
	m_bZombieClassChanged = true;

	SetCachedTeam(TEAM_UNDEAD);
	SetCachedClass(rpgclass);

	if (!SQL_ClassExists(this, rpgclass))
		SQL_CreatePlayerClass(this, rpgclass);

	LoadClassInfo(rpgclass);
}

void CRPGPlayer::SetHumanClass( int rpgclass )
{
	m_iHumanClass = rpgclass;
	m_bHumanClassChanged = true;

	SetCachedTeam(TEAM_SURVIVORS);
	SetCachedClass(rpgclass);

	if (!SQL_ClassExists(this, rpgclass))
		SQL_CreatePlayerClass(this, rpgclass);

	LoadClassInfo(rpgclass);
}

int CRPGPlayer::GetHumanClass()
{
	return m_iHumanClass;
}

int CRPGPlayer::GetZombieClass()
{
	return m_iZombieClass;
}

void CRPGPlayer::SetSkillLevel( int skill, int level )
{
	for(int i = 0; i<MAX_SKILLS; i++)
	{
		if (skills[i].iIndex == skill)
		{
			skills[i].iLevel = level;
			return;
		}
	}
}

int CRPGPlayer::GetSkillLevel( int skill )
{
	for (int i = 0; i <MAX_SKILLS; i++)
	{
		if (skills[i].iIndex == skill)
			return skills[i].iLevel;
	}
	return 0;
}

void CRPGPlayer::ResetAccount()
{
	for (int i = 0; i < MAX_SKILLS; i++)
	{
		skills[i].iLevel = 0;
	}
}

void CRPGPlayer::AddExperience( int experience )
{
	SetExperience( GetExperience() + experience );
}

void CRPGPlayer::SetExperience( int experience )
{
	m_iExperience = experience;
	m_bExperienceChanged = true;

	for (int i = (MAXLEVEL - 1); i >= 0; i--)
	{
		if ((this->GetLevel() < MAXLEVEL) && (this->GetExperience() >= ExperienceTable[i].iExperience))
		{
			this->SetLevel( ExperienceTable[i].iLevel );
			break;
		}
	}
}

int CRPGPlayer::GetExperience()
{
	return m_iExperience;
}

void CRPGPlayer::AddLevel( int level )
{
	SetLevel( GetLevel() + level );
}

void CRPGPlayer::SetLevel( int level )
{
	m_bLevelChanged = true;
	m_iLevel = level;
}

int CRPGPlayer::GetLevel()
{
	return m_iLevel;
}

bool CRPGPlayer::IsSynchronized()
{
	return m_bSynchronized;
}

void CRPGPlayer::ShowBaseMenu()
{
	IMenuStyle *style = menus->GetDefaultStyle();
	IBaseMenu *menu = style->CreateMenu(&g_RPGPlugin, myself->GetIdentity());
	menu->SetDefaultTitle(MENU_BASE_TITLE);

	for (int i = 0; i < BASEMENUCHOICES; i++)
	{
		menu->AppendItem(BaseMenuChoices[i], ItemDrawInfo(BaseMenuChoices[i]));
	}
	//menu->SetMenuOptionFlags( menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT );
	menu->Display(this->GetIndex(), MENU_TIME_FOREVER);

}

void CRPGPlayer::ShowClassMenu( int team )
{
	IMenuStyle *style = menus->GetDefaultStyle();
	IBaseMenu *menu = style->CreateMenu(&g_RPGPlugin, myself->GetIdentity());
	menu->SetDefaultTitle(MENU_CLASS_TITLE);

	switch (team)
	{
	case TEAM_SURVIVORS:
		for (int i = 0; i < NUM_HUMAN_CLASSES; i++)
		{
			menu->AppendItem(HumanClasses[i], ItemDrawInfo(HumanClasses[i]));
		}
		break;
	case TEAM_UNDEAD:
		for (int i = 0; i < NUM_ZOMBIE_CLASSES; i++)
		{
			menu->AppendItem(ZombieClasses[i], ItemDrawInfo(ZombieClasses[i]));
		}
		break;
	}

	menu->InsertItem(MENU_CHOICE_RETURN, MENU_ITEM_RETURN, ItemDrawInfo(MENU_ITEM_RETURN));
	menu->Display(this->GetIndex(), MENU_TIME_FOREVER );
}

void CRPGPlayer::ShowSkillMenu()
{
	if (GetCurrentClass() == RPG_CLASS_NONE )
	{
		ShowClassMenu( GetPlayerInfo()->GetTeamIndex());
		return;
	}

	if (GetFreeSkills() < 0)
	{
		ResetAccount();
		gamehelpers->TextMsg(GetIndex(), HUD_PRINTTALK, "[ZPS-RPG] Your skills have been reset because of an error.\n");
	}

	IMenuStyle *style = menus->GetDefaultStyle();
	IBaseMenu *menu = style->CreateMenu(&g_RPGPlugin, myself->GetIdentity());

	menu->SetDefaultTitle(MENU_SKILL_TITLE);

	char skillname[64];
	unsigned int menustyle = ITEMDRAW_DEFAULT;

	for (int i = 0; i < MAX_SKILLS; i++)
	{
		sprintf(skillname, "%s (Level %d)", SkillNames[skills[i].iIndex], skills[i].iLevel);
		menustyle = ITEMDRAW_DEFAULT;

		if ((skills[i].iLevel >= 3) || (GetFreeSkills() == 0))
			menustyle = ITEMDRAW_DISABLED;

		if( i == 3 ) // ULTIMATE
		{
			if ((skills[i].iLevel >= 1) || (GetLevel() < 6) || (GetFreeSkills() == 0))
			{
				menustyle = ITEMDRAW_DISABLED;
			}
		}

		menu->AppendItem(SkillNames[skills[i].iIndex], ItemDrawInfo(skillname, menustyle));
	}
	menu->AppendItem(MENU_ITEM_RESET, ItemDrawInfo("Reset Skills"));
	menu->InsertItem(6, MENU_ITEM_RETURN, ItemDrawInfo(MENU_ITEM_RETURN));
	menu->SetMenuOptionFlags( menu->GetMenuOptionFlags() | MENUFLAG_BUTTON_EXIT );
	menu->Display(this->GetIndex(), MENU_TIME_FOREVER);
}

void CRPGPlayer::ShowChar()
{
	IMenuStyle *style = menus->GetDefaultStyle();
	IBaseMenu *menu = style->CreateMenu(&g_RPGPlugin, myself->GetIdentity());

	IMenuPanel *panel = menu->CreatePanel();

	menu->SetDefaultTitle(MENU_CHAR_TITLE);
	panel->DrawTitle(MENU_CHAR_TITLE);

	char text[255];
	sprintf(text, "Player ID: %d", GetSQLIndex());
	panel->DrawItem(ItemDrawInfo(text));

	int classnum = GetCurrentClass();

	if (classnum > RPG_CLASS_NONE)
	{
		int team = GetCachedTeam();
		if ( team == TEAM_SURVIVORS)
		{
			sprintf(text, "Class: %s", HumanClasses[classnum]);
		}
		else if (team == TEAM_UNDEAD)
		{
			sprintf(text, "Class: %s", ZombieClasses[classnum]);
		}
		else
		{
			sprintf(text, "Class: None");
		}

		panel->DrawItem(ItemDrawInfo(text));	// item 1

		sprintf(text, "Level: %d", GetLevel());
		panel->DrawItem(ItemDrawInfo(text)); // item 2

		sprintf(text, "Experience: %d", GetExperience());
		panel->DrawItem(ItemDrawInfo(text)); // item 3

		for (int i = 0; i < MAX_SKILLS; i++)		// item 4-7
		{
			sprintf(text, "%s - (Level %d)", SkillNames[skills[i].iIndex], skills[i].iLevel);
			panel->DrawItem(ItemDrawInfo(text));
		}

	}

	panel->SendDisplay(GetIndex(), &g_RPGPlugin, MENU_TIME_FOREVER );
}

void CRPGPlayer::LoadClassInfo( int classnum )
{
	int experience = SQL_GetClassExperience(this, classnum);
	SetExperience( experience );

	SQL_GetPlayerSkills(this, classnum);

	LoadBrutePassives();
	LoadBerserkerPassives();

	for (int i = 0; i < MAX_SKILLS; i++)
	{
		//smutils->LogMessage(myself, "Skill ID: %d, Skill Level: %d", skills[i].iIndex, skills[i].iLevel);
	}

	if (GetFreeSkills() > 0)
		ShowSkillMenu();
}

int CRPGPlayer::GetCurrentClass()
{
	int team = GetCachedTeam();
	if (team == TEAM_SURVIVORS)
	{
		return GetHumanClass();
	}
	else if (team == TEAM_UNDEAD)
	{
		return GetZombieClass();
	}
	else
	{
		return RPG_CLASS_NONE;
	}
}

int CRPGPlayer::GetFreeSkills()
{
	int usedskills = 0;
	for (int i = 0; i < MAX_SKILLS; i++)
	{
		usedskills += skills[i].iLevel;
	}

	return (GetLevel() - usedskills);
}

int CRPGPlayer::GetUsedSkills()
{
	int usedskills = 0;
	for (int i = 0; i < MAX_SKILLS; i++)
	{
		usedskills += skills[i].iLevel;
	}

	return (usedskills);
}

void CRPGPlayer::SetHealth( int health )
{
	sm_sendprop_info_t health_sendprop;
	if(!gamehelpers->FindSendPropInfo("CBasePlayer", "m_iHealth", &health_sendprop))
	{
		smutils->LogError(myself, "Warning! Health offset not found");
		return;
	}

	CBaseEntity *pEntity = GetBaseEntity();
	*(int *)((char*)pEntity + health_sendprop.actual_offset) = health;
}

int CRPGPlayer::GetHealth()
{
	return GetPlayerInfo()->GetHealth();
}

RPGSkill *CRPGPlayer::GetSkill( int skill_index )
{
	for (int i = 0; i < MAX_SKILLS; i++)
	{
		if ((skills[i].iIndex == skill_index) && skills[i].iLevel > 0)
			return &skills[i];
	}
	return NULL;
}

int CRPGPlayer::GetPlayerButtons()
{
	return *(int*)((char*)GetBaseEntity() + m_iButtonsOffset);
}

void CRPGPlayer::InitOffsets()
{
	datamap_t *datamap = gamehelpers->GetDataMap(GetBaseEntity());
	typedescription_t *typedesc= gamehelpers->FindInDataMap(datamap, "m_nButtons");

	m_iButtonsOffset = typedesc->fieldOffset[TD_OFFSET_NORMAL];

	sm_sendprop_info_t sendprop;
	gamehelpers->FindSendPropInfo("CBaseCombatWeapon","m_flNextPrimaryAttack", &sendprop);
	m_iNextPrimaryAttackOffset = sendprop.actual_offset;
}

int CRPGPlayer::GetOffset( const char *classname, const char *offset )
{
	sm_sendprop_info_t sendprop; 
	if (!gamehelpers->FindSendPropInfo(classname, offset, &sendprop))
	{
		smutils->LogError(myself, "%s not found!", offset);
		return 0;
	}

	return sendprop.actual_offset;
}

void CRPGPlayer::ChargeTouch( CBaseEntity *pOther )
{
	if (!m_bUsingCharge)
		RETURN_META(MRES_IGNORED);

	edict_t *pEdict = serverents->BaseEntityToEdict( pOther );
	if (!pEdict || pEdict->IsFree())
		RETURN_META(MRES_IGNORED);

	int e_index = engine->IndexOfEdict(pEdict);
	if ((e_index >= 0) && (e_index <= playerinfo->GetGlobalVars()->maxClients))
	{
		CRPGPlayer *pPlayer = FindPlayerByIndex(e_index);
		if (!pPlayer)
			RETURN_META(MRES_IGNORED);

		if (pPlayer->GetPlayerInfo()->GetTeamIndex() == TEAM_UNDEAD)
		{
			// let's throw this fool around
			Vector EyePosition = VFuncs::EyePosition(GetBaseEntity());
			QAngle EyeAngles = GetPlayerInfo()->GetAbsAngles();

			Vector forward;
			AngleVectors(EyeAngles, &forward);

			const Vector FinalVelocity = (BRUTE_SKILL_CHARGE_FORCE * forward);
			VFuncs::Teleport((CBaseAnimating*)pPlayer->GetBaseEntity(), NULL, NULL, &FinalVelocity);
		}
	}

	RETURN_META(MRES_IGNORED);
}

EHANDLE CRPGPlayer::GetActiveWeapon()
{
	return *(EHANDLE*)((char*)GetBaseEntity() + GetOffset("CAI_BaseNPC", "m_hActiveWeapon"));
}

bool CRPGPlayer::IsOnGround()
{
	if (*(int *)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_fFlags")) & FL_ONGROUND )
	{
		return true;
	}
	return false;
}

float CRPGPlayer::GetFireRate()
{
	smutils->LogMessage(myself, "Fire rate = %f", META_RESULT_ORIG_RET(float));
	RPGSkill *skill = GetSkill( BERSERKER_SKILL_ATTACKSPEED );
	if (skill)
	{
		float orig = META_RESULT_ORIG_RET(float);
		RETURN_META_VALUE(MRES_SUPERCEDE, (orig * ( 1 - (BERSERKER_SKILL_ATTACKSPEED_BASE*skill->iLevel) ) ) );
	}
	RETURN_META_VALUE(MRES_IGNORED, META_RESULT_ORIG_RET(float));
}

void CRPGPlayer::LoadBrutePassives()
{
	RPGSkill *skill = GetSkill(BRUTE_SKILL_HEALTH);
	if (skill)
	{
		SetHealth( 100 );
		SetHealth( GetHealth() + (BRUTE_SKILL_HEALTH_BASE * skill->iLevel)); 
	}
}

void CRPGPlayer::LoadBerserkerPassives()
{
	RPGSkill *skill = GetSkill(BERSERKER_SKILL_MOVESPEED);
	if (skill)
	{
		SetMoveSpeed( 1.0 + BERSERKER_SKILL_ATTACKSPEED_BASE*skill->iLevel);
	}

	skill = GetSkill(BERSERKER_SKILL_CARRIER);
	if (skill)
	{
		VFuncs::SetModel( GetBaseEntity(), MODEL_CARRIER );
		SetMoveSpeed( GetMoveSpeed() + 0.1);
		SetHealth( 250 );
	}
}

void CRPGPlayer::SetMoveSpeed( float speed )
{
	*(float*)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_flLaggedMovementValue")) = speed;
}

float CRPGPlayer::GetMoveSpeed()
{
	return *(float*)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_flLaggedMovementValue"));
}

void CRPGPlayer::ResetBrutePassives()
{
}

void CRPGPlayer::ResetBerserkerPassives()
{
	SetMoveSpeed( 1.0 );
}

CRPGPlayer *FindPlayerByIndex( int index )
{
	for (int i = 0; i < players.Count(); i++)
	{
		if (players[i] && (players[i]->GetIndex() == index))
		{
			return players[i];
		}
	}
	return NULL;
}

CRPGPlayer * FindPlayerByBase( CBaseEntity *pEntity )
{
	for (int i = 0; i < players.Count(); i++)
	{
		if (players[i] && (players[i]->GetBaseEntity() == pEntity))
		{
			return players[i];
		}
	}
	return NULL;
}
CUtlVector<CRPGPlayer*> players;