#ifndef VFUNCS_H
#define VFUNCS_H

#include "extension.h"

class VfuncEmptyClass {};

class VFuncs
{
public:
	static void Weapon_Drop( CBasePlayer *pThisPtr, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget = NULL, const Vector *pVelocity = NULL );
	static int OnTakeDamage( CBasePlayer *pThisPtr, const CTakeDamageInfo &info);
	static void Teleport( CBaseAnimating *pThisPtr, const Vector *newPosition, const QAngle *newAngles, const Vector *newVelocity);
	static Vector EyePosition( CBaseEntity *pThisPtr );
	static void SetModel( CBaseEntity *pThisPtr, const char *modelname );
};

#endif