#ifndef RPGDEFS_H
#define RPGDEFS_H

//#pragma warning( disable : 4700 )

#define TEAM_SURVIVORS 1
#define TEAM_UNDEAD 2

#define MAXLEVEL 10
#define EXP_PER_KILL 10

#define NUM_CLASSES 2
#define NUM_ZOMBIE_CLASSES 1
#define NUM_HUMAN_CLASSES 1

#define RPG_CLASS_NONE -1
#define CLASS_BERSERKER 0

#define CLASS_BRUTE 0

#define MENU_CHOICE_BRUTE			0
#define MENU_CHOICE_BERSERKER	0

#define MENU_CHOICE_SKILL1		0
#define MENU_CHOICE_SKILL2		1
#define MENU_CHOICE_SKILL3		2
#define MENU_CHOICE_ULTIMATE	3
#define MENU_CHOICE_RESET		4

#define MENU_CHOICE_RETURN 8

#define MENU_CHOICE_CLASS	0
#define MENU_CHOICE_SKILLS	1
#define MENU_CHOICE_CHAR		2
#define MENU_CHOICE_HELP		3

#define MENU_BASE_TITLE "ZPS RPG"
#define MENU_CLASS_TITLE "Select a class:"
#define MENU_SKILL_TITLE "Select a skill:"
#define MENU_CHAR_TITLE "Character Information"

#define MENU_ITEM_RESET "Reset Skills"

#define MENU_ITEM_RETURN "Return to base menu"

#define HUD_PRINTNOTIFY		1
#define HUD_PRINTCONSOLE	2
#define HUD_PRINTTALK		3
#define HUD_PRINTCENTER		4

#define MAX_SKILLS	4

#define BRUTE_SKILL_HEALTH_BASE				50
#define BRUTE_SKILL_MELEEDAMAGE_BASE	0.20
#define BRUTE_SKILL_WEAPONTHROW_BASE	20
#define BRUTE_SKILL_WEAPONTHROW_FORCE	300
#define BRUTE_SKILL_CHARGE_BASE				2.0
#define BRUTE_SKILL_CHARGE_LENGTH			3.0f
#define BRUTE_SKILL_CHARGE_FORCE			500

#define BERSERKER_SKILL_ATTACKSPEED_BASE		0.1 // 10% faster each skill level
#define BERSERKER_SKILL_MOVESPEED_BASE 0.1 // 10% faster each skill level
#define BERSERKER_SKILL_LIFESTEAL_BASE 0.20 // 20% more life steal each level

#define MODEL_CARRIER "models/zombies/zombie0/zombie0.mdl"

#define BRUTE_SKILL_HEALTH						1 // done
#define BRUTE_SKILL_MELEEDAMAGE				2 // done
#define BRUTE_SKILL_WEAPONTHROW			3 // done
#define BRUTE_SKILL_CHARGE						4 // done

#define BERSERKER_SKILL_ATTACKSPEED		5 // done
#define BERSERKER_SKILL_MOVESPEED			6 // done
#define BERSERKER_SKILL_LIFESTEAL			7 // done
#define BERSERKER_SKILL_CARRIER				8 // done

class CTakeDamageInfo;
class CBaseAnimating;
class CBasePlayer;
class CBaseCombatWeapon;

struct ExperienceToLevel
{
	int iLevel;
	int iExperience;
};

struct RPGClass
{
	int iLevel;
	int iExperience;
};

struct RPGSkill
{
	int iIndex;
	int iLevel;
	int iUsage;
};

extern const char *ZombieClasses[];
extern const char *HumanClasses[];
extern const char *SkillNames[];

extern int OnTakeDamageOffset;
extern int TeleportOffset;
extern int WeaponDropOffset;
extern int PlayerDropWeaponOffset;
extern int PlayerPreThinkOffset;
extern int PlayerTouchOffset;
extern int EyePositionOffset;
extern int WeaponGetFireRateOffset;
extern int SetModelOffset;
extern int DropOffset;

#endif
