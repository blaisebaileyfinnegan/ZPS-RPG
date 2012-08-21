#ifndef DATABASEDEFS_H
#define DATABASEDEFS_H

#include "extension.h"
#include "IDBDriver.h"

#define TOTAL_SQLITE_TABLES	3
#define CONF_NAME "storage-local"

typedef int SQLIndex;

SQLIndex FindSQLIndexByAuth( const char *authid );

Handle_t SQL_Connect( void );
void SQL_CreateTables( void );
void SQL_CreateAccount( CRPGPlayer *pPlayer );

void SQL_CreatePlayerClass( CRPGPlayer *pPlayer, int classnum );
void SQL_CreatePlayerSkill( CRPGPlayer *pPlayer, int skill );
void SQL_CreatePlayer( CRPGPlayer *pPlayer );

void SQL_CreatePlayerSkills(CRPGPlayer *pPlayer, int team, int classnum);

void SQL_GetPlayerSkills( CRPGPlayer *pPlayer, int classnum );

bool SQL_ClassExists( CRPGPlayer *pPlayer, int classnum );
bool SQL_SkillExists( CRPGPlayer *pPlayer, int skill );

void SQL_StoreSkills( CRPGPlayer *pPlayer );

void SQL_StoreClass(CRPGPlayer *pPlayer, int classnum, int team);
int SQL_GetClassExperience(CRPGPlayer *pPlayer, int classnum);

extern IDBDriver *driver;
extern IDatabase *db;


#endif
