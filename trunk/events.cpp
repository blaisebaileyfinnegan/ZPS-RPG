#include "rpgplayer.h"
#include "extension.h"
#include "database.h"
//#include "vfuncs.h"

/*
#define GAME_DLL 1

#include "cbase.h"
#include "takedamageinfo.h"
#include "mathlib.h"
*/


void CRPGPlugin::Say_handler()
{
	int index = GetCmdIndex();
	if(!index)
		return;

	CRPGPlayer *pPlayer = FindPlayerByIndex(index);
	const char *command = engine->Cmd_Argv(1);

	if (!pPlayer)
		return;

	if (!stricmp(command, "/classmenu"))
	{
		pPlayer->ShowClassMenu( pPlayer->GetPlayerInfo()->GetTeamIndex() );
		RETURN_META(MRES_SUPERCEDE);
	}
	else if (!stricmp(command, "/skillsmenu"))
	{
		pPlayer->ShowSkillMenu();
		RETURN_META(MRES_SUPERCEDE);
	}
	else if (!stricmp(command,"/char"))
	{
		char message[256];

		int classnum = pPlayer->GetCurrentClass();
		int team = pPlayer->GetPlayerInfo()->GetTeamIndex();

		if (classnum > RPG_CLASS_NONE)
		{
			const char *classname;
			switch (team)
			{
			case TEAM_SURVIVORS:
				classname = HumanClasses[classnum];
				break;
			case TEAM_UNDEAD:
				classname = ZombieClasses[classnum];
				break;
			}
			//pPlayer->AddExperience(4);

			int level = pPlayer->GetLevel();
			int experience = pPlayer->GetExperience();

			sprintf(message, "Class: %s Level: %d Experience: %d\n", classname, level, experience);
		}
		else
		{
			sprintf(message, "You have not yet selected a class. Type /classmenu to do so.\n");
		}
		gamehelpers->TextMsg(index, HUD_PRINTTALK, message);

		RETURN_META(MRES_SUPERCEDE);
	}
	else if (!stricmp(command, "/rpghelp"))
	{
		gamehelpers->TextMsg( index, HUD_PRINTTALK, "[ZPS-RPG] Available commands: /rpgmenu /rpghelp\n");
		RETURN_META(MRES_SUPERCEDE);
	}
	else if (!stricmp(command, "/rpgmenu"))
	{
		pPlayer->ShowBaseMenu();
		RETURN_META(MRES_SUPERCEDE);
	}
}
