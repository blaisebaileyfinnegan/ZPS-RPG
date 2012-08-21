/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod CRPGPlugin Extension
 * Copyright (C) 2004-2007 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"
#include "igameevents.h"
#include "dlls/iplayerinfo.h"
#include "rpgplayer.h"
#include "database.h"

#define GAME_DLL 1

//#include "cbase.h"
#include "takedamageinfo.h"
#include "shareddefs.h"
#include "in_buttons.h"
#include "vfuncs.h"

SH_DECL_HOOK0_void(ConCommand, Dispatch, SH_NOATTRIB, false);
SH_DECL_HOOK0_void(IServerGameDLL, LevelShutdown, SH_NOATTRIB, false);
SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, 0, int);
SH_DECL_HOOK0(IServerGameDLL, GetGameDescription, SH_NOATTRIB, true, const char *);

SH_DECL_MANUALHOOK0(WeaponGetFireRate, 0, 0, 0, float);
SH_DECL_MANUALHOOK1_void(PlayerTouch, 0, 0, 0, CBaseEntity *);
SH_DECL_MANUALHOOK1_void(WeaponTouch, 0, 0, 0, CBaseEntity *);
SH_DECL_MANUALHOOK1_void(Drop, 0, 0, 0, const Vector &);

IGameEventManager *events = NULL;
IPlayerInfoManager *playerinfo = NULL;
ICvar *icvar = NULL;
IServerGameClients *serverclients = NULL;
IServerGameEnts *serverents = NULL;

IGameConfig *g_pZPConf = NULL;

SH_DECL_MANUALHOOK1(OnTakeDamage, 0, 0, 0, int, const CTakeDamageInfo &);
SH_DECL_MANUALHOOK0_void(PlayerDropWeapon, 0, 0, 0);
SH_DECL_MANUALHOOK0_void(PlayerPreThink, 0, 0, 0);

void CRPGPlugin::LevelShutdown()
{
	int class_num;
	for (int i = 0; i < players.Count(); i++)
	{
		if (players[i])
		{
			int team = players[i]->GetCachedTeam();
			switch (team)
			{
			case TEAM_SURVIVORS:
				class_num = players[i]->GetHumanClass();
				if (class_num > RPG_CLASS_NONE)
					SQL_StoreClass(players[i], class_num, TEAM_SURVIVORS);
				break;
			case TEAM_UNDEAD:
				class_num = players[i]->GetZombieClass();
				if (class_num > RPG_CLASS_NONE)
					SQL_StoreClass(players[i], class_num, TEAM_UNDEAD);
				break;
			}
			CBasePlayer *player = (CBasePlayer*)players[i]->GetBaseEntity();

			SH_REMOVE_MANUALHOOK_MEMFUNC(OnTakeDamage, player, this, &CRPGPlugin::OnTakeDamage_handler, false);
			SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerDropWeapon, player, players[i], &CRPGPlayer::PlayerDropWeapon_Prehandler, false);
			SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerPreThink, player, this, &CRPGPlugin::PlayerPreThink_handler, true);
			if (players[i]->m_bWeaponRateSet)
			{
				SH_REMOVE_MANUALHOOK_MEMFUNC(WeaponGetFireRate, player, players[i], &CRPGPlayer::GetFireRate, true);
			}

			players.Remove(i);
		}

	}
}


void CRPGPlugin::OnClientPutInServer(int client)
{
	int i = 0;
	CRPGPlayer *player = new CRPGPlayer;
	player->Init(client);
	players.AddToTail(player);

	const char *authid = player->GetAuthId();
	if (authid)
	{
		SQLIndex index = FindSQLIndexByAuth(authid);
		if (index == -1)
		{
			smutils->LogMessage(myself, "%s not found in the database; creating an account for the new guy.", authid);
			SQL_CreateAccount( player );
			RETURN_META(MRES_IGNORED);
		}
		player->SetSQLIndex( index );
	}

	//smutils->LogMessage(myself, "OnTakeDamage idx = %d", OnTakeDamageOffset);
	//smutils->LogMessage(myself, "WeapnoDrop idx = %d", WeaponDropOffset);

	CBasePlayer *pPlayer = (CBasePlayer*)player->GetBaseEntity();
	SH_ADD_MANUALHOOK_MEMFUNC(OnTakeDamage, pPlayer, this, &CRPGPlugin::OnTakeDamage_handler, false);
	SH_ADD_MANUALHOOK_MEMFUNC(PlayerDropWeapon, pPlayer, player, &CRPGPlayer::PlayerDropWeapon_Prehandler, false);
	SH_ADD_MANUALHOOK_MEMFUNC(PlayerPreThink, pPlayer, this, &CRPGPlugin::PlayerPreThink_handler, true);

}

void CRPGPlugin::OnClientDisconnected(int client)
{
	CRPGPlayer *pPlayer = FindPlayerByIndex( client );
	if (!pPlayer)
		return;

	if (pPlayer)
	{
		int oldclass;
		int team = pPlayer->GetCachedTeam();
		switch (team)
		{
		case TEAM_SURVIVORS:
			oldclass = pPlayer->GetHumanClass();
			if (oldclass != RPG_CLASS_NONE)
				SQL_StoreClass(pPlayer, oldclass, TEAM_SURVIVORS);
			break;
		case TEAM_UNDEAD:
			oldclass = pPlayer->GetZombieClass();
			if (oldclass != RPG_CLASS_NONE)
				SQL_StoreClass(pPlayer, oldclass, TEAM_UNDEAD);
			break;
		}
	}

	CBasePlayer *player = (CBasePlayer*)pPlayer->GetBaseEntity();

	SH_REMOVE_MANUALHOOK_MEMFUNC(OnTakeDamage, player, this, &CRPGPlugin::OnTakeDamage_handler, false);
	SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerDropWeapon, player, pPlayer, &CRPGPlayer::PlayerDropWeapon_Prehandler, false);
	SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerPreThink, player, this, &CRPGPlugin::PlayerPreThink_handler, true);


	int index = -1;
	for (int i = 0; i < players.Count(); i++)
	{
		if (players[i] && (players[i]->GetIndex() == client))
		{
			index = i;
			break;
		}
	}

	//delete players[index];
	if (index > -1)
		players.Remove(index);
}

int CRPGPlugin::OnTakeDamage_handler( const CTakeDamageInfo &info )
{
	CBasePlayer *pVictim = META_IFACEPTR(CBasePlayer);

	if (!pVictim)
		RETURN_META_VALUE( MRES_IGNORED, META_RESULT_ORIG_RET(int) );

	CRPGPlayer *pAttacker = FindPlayerByIndex(info.m_hAttacker.GetEntryIndex());

	if (!pAttacker)
		RETURN_META_VALUE( MRES_IGNORED, META_RESULT_ORIG_RET(int) );


	RPGSkill *skill = pAttacker->GetSkill(BRUTE_SKILL_MELEEDAMAGE);
	if( skill )
	{
		CTakeDamageInfo newinfo = info;
		if (newinfo.GetDamageType() & (DMG_GENERIC | DMG_CRUSH | DMG_SLASH | DMG_CLUB))
		{
			newinfo.ScaleDamage( 1.0f + (BRUTE_SKILL_MELEEDAMAGE_BASE * skill->iLevel));
			smutils->LogMessage(myself, "New Damage: %f", newinfo.GetDamage());
			RETURN_META_VALUE_MNEWPARAMS(MRES_HANDLED, META_RESULT_ORIG_RET(int), OnTakeDamage, (newinfo));
		}
	}

	skill = pAttacker->GetSkill(BERSERKER_SKILL_LIFESTEAL);
	if (skill)
	{
		int delta_health = (info.GetDamage()*(BERSERKER_SKILL_LIFESTEAL_BASE*skill->iLevel));
		pAttacker->SetHealth( pAttacker->GetHealth() + delta_health);
	}

	RETURN_META_VALUE( MRES_IGNORED, META_RESULT_ORIG_RET(int) );
}

void CRPGPlugin::PlayerPreThink_handler()
{
	CBaseEntity *pEntity = (CBaseEntity*)META_IFACEPTR(CBasePlayer);
	if (!pEntity)
		return;

	CRPGPlayer *pPlayer = FindPlayerByBase(pEntity);
	if (!pPlayer)
		return;

	if (pPlayer->GetPlayerInfo()->IsDead())
		RETURN_META(MRES_IGNORED);

	int nButtons = pPlayer->GetPlayerButtons();

	RPGSkill *skill;
	if( skill = pPlayer->GetSkill(BRUTE_SKILL_CHARGE) )
	{
		if ((skill->iUsage == 0) && (nButtons & IN_ATTACK2) && (!pPlayer->GetPlayerInfo()->IsDead()))
		{
			skill->iUsage++;
			pPlayer->StartCharge();
			return;
		}
		float curtime = playerinfo->GetGlobalVars()->curtime;
		if ((pPlayer->m_bUsingCharge) && (curtime >= (pPlayer->m_flChargeStart + BRUTE_SKILL_CHARGE_LENGTH)))
		{
			pPlayer->EndCharge();
		}
	}

}

bool CRPGPlugin::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlength, bool late)
{
	size_t maxlen = maxlength;

	return true;
}

bool CRPGPlugin::SDK_OnMetamodUnload(char *error, size_t maxlength)
{
	return true;
}

void CRPGPlugin::SDK_OnAllLoaded()
{
	playerhelpers->AddClientListener( this );

	/*
	events->AddListener(this, "player_death", true);
	events->AddListener(this, "player_team", true);
	events->AddListener(this, "game_round_restart", true);
	events->AddListener(this, "player_spawn", true);
	*/

	events->AddListener(this, true);

	ConCommandBase *pCmd = icvar->GetCommands();
	const char *name;
	while (pCmd)
	{
		if (pCmd->IsCommand())
		{
			name = pCmd->GetName();
			if (!stricmp(name, "say"))
			{
				m_pSayCmd = (ConCommand *)pCmd;
			}
		}
		pCmd = const_cast<ConCommandBase *>(pCmd->GetNext());
	}

	if (m_pSayCmd)
	{
		SH_ADD_HOOK_MEMFUNC(ConCommand, Dispatch, m_pSayCmd, this, &CRPGPlugin::Say_handler, false);
	}

	SH_ADD_HOOK_MEMFUNC(IServerGameClients, SetCommandClient, serverclients, this, &CRPGPlugin::SetCmdIndex, false);
	SH_ADD_HOOK_MEMFUNC(IServerGameDLL, GetGameDescription, gamedll, this, &CRPGPlugin::GetGameDescription, false);
	SH_ADD_HOOK_MEMFUNC(IServerGameDLL, LevelShutdown, gamedll, this, &CRPGPlugin::LevelShutdown, false);

	char error[255];
	if (!gameconfs->LoadGameConfigFile("noobcannon.zps", &g_pZPConf, error, 255 ))
	{
		smutils->LogError(myself, error);
	}

	g_pZPConf->GetOffset("OnTakeDamage", &OnTakeDamageOffset);
	g_pZPConf->GetOffset("WeaponDrop", &WeaponDropOffset);
	g_pZPConf->GetOffset("PlayerDropWeapon", &PlayerDropWeaponOffset);
	g_pZPConf->GetOffset("PlayerPreThink", &PlayerPreThinkOffset);
	g_pZPConf->GetOffset("PlayerTouch", &PlayerTouchOffset);
	g_pZPConf->GetOffset("Teleport", &TeleportOffset);
	g_pZPConf->GetOffset("EyePosition", &EyePositionOffset);
	g_pZPConf->GetOffset("WeaponGetFireRate", &WeaponGetFireRateOffset);
	g_pZPConf->GetOffset("SetModel", &SetModelOffset);
	g_pZPConf->GetOffset("Drop", &DropOffset);

	SH_MANUALHOOK_RECONFIGURE(PlayerTouch, PlayerTouchOffset, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(WeaponGetFireRate, WeaponGetFireRateOffset, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(OnTakeDamage, OnTakeDamageOffset, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(PlayerDropWeapon, PlayerDropWeaponOffset, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(PlayerPreThink, PlayerPreThinkOffset, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(Drop, DropOffset, 0, 0);
	SH_MANUALHOOK_RECONFIGURE(WeaponTouch, PlayerTouchOffset, 0, 0);


	//smutils->LogMessage(myself, "OnTakeDamage idx = %d", OnTakeDamageOffset);
	Handle_t hndl = SQL_Connect();
	SQL_CreateTables();
}

void CRPGPlugin::LevelInit( char const *pMapName, char const *pMapEntities, char const *pOldLevel, char const *pLandmarkName, bool loadGame, bool background )
{
	smutils->LogMessage(myself, "LevelInit Called");
}

const char *CRPGPlugin::GetGameDescription( void )
{
	RETURN_META_VALUE(MRES_IGNORED, META_RESULT_ORIG_RET(const char *));
}

void CRPGPlugin::SetCmdIndex( int cmd )
{
	m_CmdIndex = cmd + 1; //HL2 is offset by -1 for some reason...
}

int CRPGPlugin::GetCmdIndex()
{
	return m_CmdIndex;
}

void CRPGPlayer::StartCharge()
{
	*(float *)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_flLaggedMovementValue")) = BRUTE_SKILL_CHARGE_BASE;
	*(int *)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_fFlags")) |= FL_GODMODE;

	SH_ADD_MANUALHOOK_MEMFUNC(PlayerTouch, GetBaseEntity(), this, &CRPGPlayer::ChargeTouch, false);

	m_flChargeStart = playerinfo->GetGlobalVars()->curtime;
	m_bUsingCharge = true;
}

void CRPGPlayer::EndCharge()
{
	*(float *)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_flLaggedMovementValue")) = 1.0;
	*(int *)((char*)GetBaseEntity() + GetOffset("CBasePlayer", "m_fFlags")) &= ~FL_GODMODE;

	SH_REMOVE_MANUALHOOK_MEMFUNC(PlayerTouch, GetBaseEntity(), this, &CRPGPlayer::ChargeTouch, false);
	m_bUsingCharge = false;
}

void CRPGPlayer::WeaponDrop( const Vector &vecVelocity )
{
	smutils->LogMessage(myself, "WeaponDrop Called!");
	RPGSkill *skill = GetSkill(BRUTE_SKILL_WEAPONTHROW);
	if ( skill )
	{
		Vector newVelocity = vecVelocity;
		newVelocity *= BRUTE_SKILL_WEAPONTHROW_FORCE;

		RETURN_META_MNEWPARAMS(MRES_HANDLED, Drop, (newVelocity));
	}
}

void CRPGPlayer::WeaponTouch( CBaseEntity *pOther )
{
	if (pOther)
	{
		edict_t *pEdict = serverents->BaseEntityToEdict( pOther );
		if (pEdict && !pEdict->IsFree())
		{
			CRPGPlayer *pPlayer = FindPlayerByBase( pOther );
			if (pPlayer && (pPlayer->GetPlayerInfo()->GetTeamIndex() == TEAM_UNDEAD) )
			{
				RPGSkill *skill = GetSkill(BRUTE_SKILL_WEAPONTHROW);
				int level = skill->iLevel;

				edict_t *pMe = serverents->BaseEntityToEdict( META_IFACEPTR(CBaseEntity) );
				int me_index = engine->IndexOfEdict(pMe);

				//pPlayer->SetHealth( GetHealth() - (BRUTE_SKILL_WEAPONTHROW_BASE * level));
				VFuncs::OnTakeDamage((CBasePlayer*)pPlayer->GetBaseEntity(), CTakeDamageInfo(GetBaseEntity(), GetBaseEntity(), (BRUTE_SKILL_WEAPONTHROW_BASE * level), DMG_CRUSH));
			}
		}
	}

	if (!pOther)
		RETURN_META(MRES_IGNORED);

	if (pOther == GetBaseEntity())
		RETURN_META(MRES_IGNORED);


	SH_REMOVE_MANUALHOOK_MEMFUNC(WeaponTouch, META_IFACEPTR(CBaseEntity), this, &CRPGPlayer::WeaponTouch, false);
	RETURN_META(MRES_IGNORED);
}

void CRPGPlayer::PlayerDropWeapon_Prehandler()
{
	RPGSkill *skill = GetSkill(BRUTE_SKILL_WEAPONTHROW);
	if (skill)
	{
		smutils->LogMessage(myself, "Pre-PlayerDropWeapon Called");
		unsigned long weapon = GetActiveWeapon().GetEntryIndex();
		smutils->LogMessage(myself, "weapon = %d", weapon);
		if (!weapon)
		{
			smutils->LogMessage(myself, "!weapon");
			RETURN_META(MRES_IGNORED);
		}

		edict_t *pEdict = engine->PEntityOfEntIndex(weapon);
		if(!pEdict || pEdict->IsFree())
		{
			smutils->LogMessage(myself, "!pEdict in PlayerDropWeapon");
			RETURN_META(MRES_IGNORED);
		}

		CBaseEntity *pEntity = pEdict->GetUnknown()->GetBaseEntity();

		QAngle EyeAngles = GetPlayerInfo()->GetAbsAngles();
		Vector forward;
		AngleVectors(EyeAngles, &forward);

		const Vector FinalVelocity = (BRUTE_SKILL_WEAPONTHROW_FORCE * forward);

		//VFuncs::Weapon_Drop((CBasePlayer*)GetBaseEntity(), (CBaseCombatWeapon*)pEntity, NULL, &FinalVelocity)
		SH_ADD_MANUALHOOK_MEMFUNC(Drop, pEntity, this, &CRPGPlayer::WeaponDrop, false);
		SH_ADD_MANUALHOOK_MEMFUNC(WeaponTouch, pEntity, this, &CRPGPlayer::WeaponTouch, false);

		RETURN_META(MRES_IGNORED);
	}
	RETURN_META(MRES_IGNORED);
}

void CRPGPlugin::OnMenuSelect( IBaseMenu *menu, int client, unsigned int item )
{
	CRPGPlayer *pPlayer = FindPlayerByIndex(client);
	if (!pPlayer)
		return;

	if (!menu)
		return;

	ItemDrawInfo *info = NULL;
	const char *choice = menu->GetItemInfo(item, info);

	const char *title = menu->GetDefaultTitle();
	int team = pPlayer->GetPlayerInfo()->GetTeamIndex();
	if (!stricmp(title, MENU_CLASS_TITLE))
	{
		char text[256];
		if (item == MENU_CHOICE_RETURN)
		{
			pPlayer->ShowBaseMenu();
		}

		if( !pPlayer->m_bWeaponRateSet && pPlayer->GetSkill(BERSERKER_SKILL_ATTACKSPEED) )
		{
			pPlayer->m_bWeaponRateSet = true;

			int weapon = pPlayer->GetActiveWeapon().GetEntryIndex();
			if (weapon != -1)
			{
				CBaseEntity *pEntity = engine->PEntityOfEntIndex(weapon)->GetUnknown()->GetBaseEntity();
				if (pEntity)
					SH_ADD_MANUALHOOK_MEMFUNC(WeaponGetFireRate, pEntity, pPlayer, &CRPGPlayer::GetFireRate, true);
			}
		}


		if (team == TEAM_SURVIVORS)
		{
			int oldclass = pPlayer->GetHumanClass();
			if (oldclass != RPG_CLASS_NONE)
				SQL_StoreClass(pPlayer, oldclass, TEAM_SURVIVORS);


			switch (item)
			{
			case MENU_CHOICE_BRUTE:
				pPlayer->SetHumanClass( CLASS_BRUTE );
				//smutils->LogMessage(myself, "Player exp is %d", pPlayer->GetExperience());

				sprintf(text, "[ZPS-RPG] You have selected a level %d %s as your human class.\n", pPlayer->GetLevel(), HumanClasses[item]);
				gamehelpers->TextMsg( client, HUD_PRINTTALK, text);
				break;
			}
		}
		else if (team == TEAM_UNDEAD)
		{
			int oldclass = pPlayer->GetZombieClass();
			if (oldclass != RPG_CLASS_NONE)
				SQL_StoreClass(pPlayer, oldclass, TEAM_UNDEAD);

			switch (item)
			{
			case MENU_CHOICE_BERSERKER:
				pPlayer->SetZombieClass( CLASS_BERSERKER );
				//smutils->LogMessage(myself, "Player exp is %d", pPlayer->GetExperience());

				sprintf(text, "[ZPS-RPG] You have selected a level %d %s as your zombie class.\n", pPlayer->GetLevel(), ZombieClasses[item]);
				gamehelpers->TextMsg( client, HUD_PRINTTALK, text);
				break;
			}
		}
	}
	else if (!stricmp(title, MENU_SKILL_TITLE))
	{

		if ((item >= MENU_CHOICE_SKILL1) && (item <= MENU_CHOICE_SKILL3))
		{
			if (pPlayer->GetFreeSkills() == 0)
			{
				gamehelpers->TextMsg(client, HUD_PRINTTALK, "[ZPS-RPG] You do not have any free skill points.\n");
				return;
			}
			if (pPlayer->skills[item].iLevel >= 3)
			{
				gamehelpers->TextMsg(client, HUD_PRINTTALK, "[ZPS-RPG] You have already maxed this skill.\n");
				pPlayer->ShowSkillMenu( );
				return;
			}
			pPlayer->skills[item].iLevel++;
			if (pPlayer->GetFreeSkills() > 0)
				pPlayer->ShowSkillMenu( );
		}
		else if (item == MENU_CHOICE_ULTIMATE)
		{
			if (pPlayer->GetFreeSkills() == 0)
			{
				gamehelpers->TextMsg(client, HUD_PRINTTALK, "[ZPS-RPG] You do not have any free skill points.\n");
				return;
			}

			if (pPlayer->skills[item].iLevel  >= 1)
			{
				gamehelpers->TextMsg(client, HUD_PRINTTALK, "[ZPS-RPG] You have already maxed this skill.\n");
				pPlayer->ShowSkillMenu();
				return;
			}
			pPlayer->skills[item].iLevel++;
			SQL_StoreSkills(pPlayer);
			if (pPlayer->GetFreeSkills() > 0)
				pPlayer->ShowSkillMenu();
		}

		else if (item == MENU_CHOICE_RESET)
		{
			pPlayer->ResetAccount();
			pPlayer->ShowSkillMenu();
		}
		else if (item == MENU_CHOICE_RETURN)
		{
			pPlayer->ShowBaseMenu();
		}
	}
	else if (!stricmp(title, MENU_BASE_TITLE))
	{
		switch (item)
		{
		case MENU_CHOICE_CLASS:
			pPlayer->ShowClassMenu(pPlayer->GetPlayerInfo()->GetTeamIndex());
			break;
		case MENU_CHOICE_SKILLS:
			pPlayer->ShowSkillMenu();
			break;
		case MENU_CHOICE_CHAR:
			pPlayer->ShowChar();
			break;
		case MENU_CHOICE_HELP:
			break;
		}
	}
}

void CRPGPlugin::FireGameEvent(KeyValues *event)
{
	if (!event)
		return;

	smutils->LogMessage(myself, "Event = %s", event->GetName());
	if (!stricmp(event->GetName(), "player_death"))
	{
		int victim = event->GetInt("userid");
		int attacker = event->GetInt("attacker");
		const char *weapon = event->GetString("weapon");

		int v_index = playerhelpers->GetClientOfUserId(victim);
		int a_index = playerhelpers->GetClientOfUserId(attacker);

		int maxclients = playerhelpers->GetMaxClients();
		if ((v_index > maxclients) || 
			(a_index > maxclients))
			return;

		CRPGPlayer *pVictim = FindPlayerByIndex(v_index);
		CRPGPlayer *pAttacker = FindPlayerByIndex(a_index);

		if (!pVictim || !pAttacker)
		{
			smutils->LogError(myself, "Warning! pVictim = %d & pAttacker = %d", v_index, a_index);
			return;
		}

		int bonus = (pVictim->GetLevel()/pAttacker->GetLevel());
		pAttacker->AddExperience( EXP_PER_KILL * bonus );

		pVictim->ResetBerserkerPassives();
		pVictim->ResetBrutePassives();

		if (pVictim->m_bWeaponRateSet)
		{
			pVictim->m_bWeaponRateSet = false;
			if (pVictim->m_bWeaponRateSet)
			{
				SH_REMOVE_MANUALHOOK_MEMFUNC(WeaponGetFireRate, pVictim->GetBaseEntity(), pVictim, &CRPGPlayer::GetFireRate, true);
				pVictim->m_bWeaponRateSet = false;
			}
		}
	}
	else if (!stricmp(event->GetName(), "game_round_restart"))
	{
		CRPGPlayer *pPlayer = NULL;
		for (int i = 0; i <= playerinfo->GetGlobalVars()->maxClients; i++)
		{
			if (pPlayer = FindPlayerByIndex(i))
			{
				for (int i = 0; i < MAX_SKILLS; i++)
				{
					pPlayer->skills[i].iUsage = 0;
				}
			}
		}
	}
	else if (!stricmp(event->GetName(), "player_spawn"))
	{
		int player = event->GetInt("userid");
		int index = playerhelpers->GetClientOfUserId(player);

		CRPGPlayer *pPlayer = FindPlayerByIndex(index);

		if (!pPlayer)
			return;

		if( !pPlayer->m_bWeaponRateSet && pPlayer->GetSkill(BERSERKER_SKILL_ATTACKSPEED) )
		{
			pPlayer->m_bWeaponRateSet = true;

			int weapon = pPlayer->GetActiveWeapon().GetEntryIndex();
			if (weapon != -1)
			{
				edict_t *pEdict = engine->PEntityOfEntIndex(weapon);
				if (pEdict)
				{
					CBaseEntity *pEntity = pEdict->GetUnknown()->GetBaseEntity();
					if (pEntity)
						SH_ADD_MANUALHOOK_MEMFUNC(WeaponGetFireRate, pEntity, pPlayer, &CRPGPlayer::GetFireRate, true);
				}
			}
		}

		if (pPlayer->GetCachedClass() > RPG_CLASS_NONE)
			SQL_StoreClass(pPlayer, pPlayer->GetCachedClass(), pPlayer->GetCachedTeam());

		int team = pPlayer->GetPlayerInfo()->GetTeamIndex();
		switch (team)
		{
		case TEAM_SURVIVORS:

			if (pPlayer->GetHumanClass() == RPG_CLASS_NONE)
			{
				pPlayer->ShowClassMenu( team );
			}
			else
			{
				char text[256];
				sprintf(text, "[ZPS-RPG] You are currently a level %d %s. Type /rpghelp for more info.\n", pPlayer->GetLevel(), HumanClasses[pPlayer->GetHumanClass()]);
				gamehelpers->TextMsg(index, HUD_PRINTTALK, text);

				pPlayer->SetHumanClass( pPlayer->GetHumanClass() );
			}
			break;
		case TEAM_UNDEAD:

			if (pPlayer->GetZombieClass() == RPG_CLASS_NONE)
			{
				pPlayer->ShowClassMenu( team );
			}
			else
			{
				char text[256];
				sprintf(text, "You are currently a level %d %s.\n", pPlayer->GetLevel(), ZombieClasses[pPlayer->GetZombieClass()]);
				gamehelpers->TextMsg(index, HUD_PRINTTALK, text);

				pPlayer->SetZombieClass( pPlayer->GetZombieClass() );
			}
			break;
		}

		pPlayer->LoadBrutePassives();
		pPlayer->LoadBerserkerPassives();
	}
	else if(!stricmp(event->GetName(), "player_team"))
	{
		int player = event->GetInt("userid");
		int index = playerhelpers->GetClientOfUserId(player);

		CRPGPlayer *pPlayer = FindPlayerByIndex(index);
		if (pPlayer)
		{
			smutils->LogMessage(myself, "Class was %d, team was %d", pPlayer->GetCachedClass(), pPlayer->GetCachedTeam());
			SQL_StoreClass(pPlayer, pPlayer->GetCachedClass(), pPlayer->GetCachedTeam());

			pPlayer->ResetBerserkerPassives();
			pPlayer->ResetBrutePassives();
		}
	}
}

CRPGPlugin g_RPGPlugin;		/**< Global singleton for extension's main interface */

SMEXT_LINK(&g_RPGPlugin);
