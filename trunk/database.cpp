#include "database.h"
#include "extension.h"

const char *szCreateDatabase[TOTAL_SQLITE_TABLES] =
{
	"CREATE TABLE IF NOT EXISTS `rpg_player` ( `player_id` INTEGER PRIMARY KEY AUTOINCREMENT, `player_steamid` varchar(25) NOT NULL default ”, `player_ip` varchar(20) NOT NULL default ”, `player_name` varchar(35) NOT NULL default ”, `time` timestamp(14) NOT NULL DEFAULT CURRENT_TIMESTAMP );",
	"CREATE TABLE IF NOT EXISTS `rpg_player_class` ( `player_id` int(8) NOT NULL default '0', `class_team` tinyint(4) NOT NULL default '0', `class_id` tinyint(4) NOT NULL default '0', `class_xp` int(8) default NULL, PRIMARY KEY (`player_id`,'class_team',`class_id`) );",
	"CREATE TABLE IF NOT EXISTS `rpg_player_skill` ( `player_id` int(8) NOT NULL default '0', `skill_id` tinyint(4) NOT NULL default '0', `skill_level` tinyint(4) NOT NULL default '0', PRIMARY KEY (`player_id`,`skill_id`) );"
};

const char *szTableNames[TOTAL_SQLITE_TABLES] = 
{
	"rpg_player",
	"rpg_player_class",
	"rpg_player_skill"
};

IDBDriver *driver = NULL;
IDatabase *db = NULL;

Handle_t SQL_Connect()
{
	char *error = NULL;

	if (!dbi->Connect(CONF_NAME, &driver, &db, true, error, 255))
	{
		return BAD_HANDLE;
	}

	Handle_t hndl = dbi->CreateHandle(DBHandle_Database, db, myself->GetIdentity());
	if (!hndl)
	{
		db->Close();
		return BAD_HANDLE;
	}

	smutils->LogMessage(myself, "Connection successful!");
	return hndl;
}

void SQL_CreateTables( void )
{
	char *error = NULL;
	for( int i = 0; i < TOTAL_SQLITE_TABLES; i++ )
	{
		IPreparedQuery *query = db->PrepareQuery(szCreateDatabase[i], error, sizeof(szCreateDatabase[i]));
		query->Execute();
	}
}

void SQL_CreateAccount( CRPGPlayer *pPlayer )
{
	if (!pPlayer)
		return;

	SQL_CreatePlayer( pPlayer );
}

void SQL_CreatePlayer( CRPGPlayer *pPlayer )
{
	char query[256];
	char *error = NULL;

	sprintf(query, "insert into rpg_player values(NULL, '%s', 'placeholder', '%s', julianday('now'));", 
		pPlayer->GetAuthId(), 
		pPlayer->GetPlayerInfo()->GetName());

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();

	int index = FindSQLIndexByAuth(pPlayer->GetAuthId());
	pPlayer->SetSQLIndex( index );
}

void SQL_CreatePlayerClass( CRPGPlayer *pPlayer, int classnum )
{
	char query[256];
	char error[256];

	int index = pPlayer->GetSQLIndex();
	int team = pPlayer->GetCachedTeam();

	if (index == -1)
		return;

	sprintf(query, "insert into rpg_player_class values(%d, %d, %d, 1000);", 
		index, 
		team,
		classnum);

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();
	
	SQL_CreatePlayerSkills(pPlayer, team, classnum);
}

void SQL_CreatePlayerSkill( CRPGPlayer *pPlayer, int skill )
{
	char query[256];
	char error[256];

	int index = pPlayer->GetSQLIndex();

	if(index == -1)
		return;

	sprintf(query, "insert into rpg_player_skill values(%d, %d, 0);", 
		index, 
		skill);

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();
}

SQLIndex FindSQLIndexByAuth( const char *authid )
{
	char query[256];
	char *error = NULL;

	sprintf(query, "select player_id from rpg_player where player_steamid = '%s';", authid);

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();

	IResultSet *result = qr->GetResultSet();
	if(result)
	{
		unsigned int rows = result->GetRowCount();
		if(rows == 0)
		{
			smutils->LogMessage(myself, "0 accounts of %s found!", authid);
			return -1;
		}

		if(rows > 1)
		{
			smutils->LogMessage(myself, "Warning! %d accounts found!", rows);
		}

		int index;
		IResultRow *row = result->FetchRow();

		DBResult result = row->GetInt(0, &index);
		if(result != 3)
		{
			smutils->LogMessage(myself, "Warning! result found to be %d", result);
			return -1;
		}
		smutils->LogMessage(myself, "Found %d as a player_id", index);

		if (!index)
			return -1;

		return index;
	}
	return -1;
}

bool SQL_ClassExists( CRPGPlayer *pPlayer, int classnum )
{
	char query[512];
	char *error = NULL;

	sprintf(query, "select * from rpg_player_class where player_id = %d and class_id = %d and class_team = %d;", pPlayer->GetSQLIndex(), classnum, pPlayer->GetCachedTeam());

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();

	IResultSet *result = qr->GetResultSet();

	if (pPlayer->GetSQLIndex() == 1)
	{
		smutils->LogMessage(myself, "Row count for playerid %d, class %d, team %d is %d", pPlayer->GetSQLIndex(), classnum, pPlayer->GetPlayerInfo()->GetTeamIndex(), result->GetRowCount());
	}

	if(result->GetRowCount() == 0)
		return false;

	return true;
}

bool SQL_SkillExists( CRPGPlayer *pPlayer, int skill )
{
	char query[512];
	char *error = NULL;

	sprintf(query, "select * from rpg_player_skill where player_id = %d and skill_id = %d;", pPlayer->GetSQLIndex(), skill);

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();

	IResultSet *result = qr->GetResultSet();

	if(result->GetRowCount() == 0)
		return false;

	return true;
}

int SQL_GetClassExperience( CRPGPlayer *pPlayer, int classnum )
{
	char query[512];
	char *error = NULL;

	int team = pPlayer->GetCachedTeam();

	sprintf(query, "select class_xp from rpg_player_class where player_id = %d and class_id = %d and class_team = %d;", pPlayer->GetSQLIndex(), classnum, team);

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();

	IResultSet *result = qr->GetResultSet();
	if(result)
	{
		int rows = result->GetRowCount();
		if (rows == 1)
		{
			IResultRow *row = result->FetchRow();
			int exp;
			DBResult dbresult = row->GetInt(0, &exp);
			//smutils->LogMessage(myself, "Experience is %d, wat = %d", exp, dbresult);
			return exp;
		}
	}
	return 0;
}

void SQL_StoreClass( CRPGPlayer *pPlayer, int classnum, int team )
{
	char query[512];
	char *error = NULL;

	int experience = pPlayer->GetExperience();
	
	sprintf(query, "update rpg_player_class set class_xp = %d where player_id = %d and class_team = %d and class_id = %d;", experience, pPlayer->GetSQLIndex(), team, classnum);
	//smutils->LogMessage(myself, query);

	if (!db->DoSimpleQuery(query))
	{
		smutils->LogMessage(myself, "Warning! %d XP Storage failed for player SQL index %d.", experience, pPlayer->GetSQLIndex());
	}

	SQL_StoreSkills( pPlayer );
}

void SQL_GetPlayerSkills( CRPGPlayer *pPlayer, int classnum )
{
	char query[256];
	char error[256];

	int index = pPlayer->GetSQLIndex();
	int team = pPlayer->GetCachedTeam();

	smutils->LogMessage(myself, "Team = %d", team);

	if (index == -1)
		return;

	sprintf(query, "select skill_id, skill_level from rpg_player_skill where player_id = %d;", index);

	IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
	qr->Execute();

	IResultSet *results = qr->GetResultSet();
	if (results)
	{
		int rownum = results->GetRowCount();
		smutils->LogMessage(myself, "Number of skills: %d", rownum);

		if (rownum == 0)
		{
			SQL_CreatePlayerSkills(pPlayer, team, classnum);
			SQL_GetPlayerSkills(pPlayer, classnum);
			return;
		}

		int i = 0;
		RPGSkill skill;
		IResultRow *row;

		while(row = results->FetchRow())
		{
			if (row->GetInt(0, &skill.iIndex) == DBVal_Data)
			{
				if (team == TEAM_SURVIVORS)
				{
					if (classnum == CLASS_BRUTE)
					{
						if ((skill.iIndex >= BRUTE_SKILL_HEALTH) && (skill.iIndex <= BRUTE_SKILL_CHARGE))
						{
							if (row->GetInt(1, &skill.iLevel) < DBVal_Data)
								skill.iLevel = 0;

							skill.iUsage = 0;
							pPlayer->skills[i] = skill;
							i++;
						}
					}
				}
				else if(team == TEAM_UNDEAD)
				{
					if(classnum == CLASS_BERSERKER)
					{
						if ((skill.iIndex >= BERSERKER_SKILL_ATTACKSPEED) && (skill.iIndex <= BERSERKER_SKILL_CARRIER))
						{
							if (row->GetInt(1, &skill.iLevel) < DBVal_Data)
								skill.iLevel = 0;

							skill.iUsage = 0;
							pPlayer->skills[i] = skill;
							i++;
						}
					}

				}
			}

			if (i == MAX_SKILLS) // we're done
				break;

		}
	}
}

void SQL_StoreSkills( CRPGPlayer *pPlayer )
{
	char query[512];
	char error[255];

	int index = pPlayer->GetSQLIndex();

	if (index == -1)
		return;

	for (int i = 0; i < MAX_SKILLS; i++)
	{
		sprintf(query, "update rpg_player_skill set skill_level = %d where player_id = %d and skill_id = %d;", 
			pPlayer->skills[i].iLevel,
			index,
			pPlayer->skills[i].iIndex);

		IPreparedQuery *qr = db->PrepareQuery(query, error, 255);
		qr->Execute();
	}
}

void SQL_CreatePlayerSkills( CRPGPlayer *pPlayer, int team, int classnum )
{

	if (team == TEAM_SURVIVORS)
	{
		switch (classnum)
		{
		case CLASS_BRUTE:
			for (int i = BRUTE_SKILL_HEALTH; i <= BRUTE_SKILL_CHARGE; i++)
			{
				SQL_CreatePlayerSkill(pPlayer, i);
			}
			break;
		}
	}
	else if (team == TEAM_UNDEAD)
	{
		switch (classnum)
		{
		case CLASS_BERSERKER:
			for (int i = BERSERKER_SKILL_ATTACKSPEED; i <= BERSERKER_SKILL_CARRIER; i++)
			{
				SQL_CreatePlayerSkill(pPlayer, i);
			}
			break;
		}
	}
}