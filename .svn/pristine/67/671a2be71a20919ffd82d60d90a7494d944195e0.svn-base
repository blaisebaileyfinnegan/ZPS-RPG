#include "vfuncs.h"
#include "rpgdefs.h"

int VFuncs::OnTakeDamage( CBasePlayer *pThisPtr, const CTakeDamageInfo &info )
{
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[OnTakeDamageOffset]; 

	union {CBaseEntity *(VfuncEmptyClass::*mfpnew)(const CTakeDamageInfo &info);
#ifndef __linux__
	void *addr;	} u; 	u.addr = func;
#else // GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 
	struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

	return (int) (reinterpret_cast<VfuncEmptyClass*>(this_ptr)->*u.mfpnew)(info);
}

void VFuncs::Teleport( CBaseAnimating *pThisPtr, const Vector *newPosition, const QAngle *newAngles, const Vector *newVelocity )
{
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[TeleportOffset]; 

	union {void (VfuncEmptyClass::*mfpnew)(const Vector *, const QAngle *, const Vector *);
#ifndef __linux__
	void *addr;	} u; 	u.addr = func;
#else // GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 
	struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

	(void) (reinterpret_cast<VfuncEmptyClass*>(this_ptr)->*u.mfpnew)(newPosition, newAngles, newVelocity);
}

Vector VFuncs::EyePosition( CBaseEntity *pThisPtr )
{
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[EyePositionOffset]; 

	union {Vector (VfuncEmptyClass::*mfpnew)( void );
#ifndef __linux__
	void *addr;	} u; 	u.addr = func;
#else // GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 
	struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

	return (Vector) (reinterpret_cast<VfuncEmptyClass*>(this_ptr)->*u.mfpnew)( );
}

void VFuncs::Weapon_Drop( CBasePlayer *pThisPtr, CBaseCombatWeapon *pWeapon, const Vector *pvecTarget /*= NULL*/, const Vector *pVelocity /*= NULL */ )
{
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[WeaponDropOffset]; 

	union {void (VfuncEmptyClass::*mfpnew)(CBaseCombatWeapon *,Vector  const*,Vector  const*);
#ifndef __linux__
	void *addr;	} u; 	u.addr = func;
#else // GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 
	struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

	(void) (reinterpret_cast<VfuncEmptyClass*>(this_ptr)->*u.mfpnew)(pWeapon, pvecTarget, pVelocity);
}

void VFuncs::SetModel( CBaseEntity *pThisPtr, const char *modelname )
{
	void **this_ptr = *(void ***)&pThisPtr;
	void **vtable = *(void ***)pThisPtr;
	void *func = vtable[SetModelOffset]; 

	union {void (VfuncEmptyClass::*mfpnew)(const char *modelname);
#ifndef __linux__
	void *addr;	} u; 	u.addr = func;
#else // GCC's member function pointers all contain a this pointer adjustor. You'd probably set it to 0 
	struct {void *addr; intptr_t adjustor;} s; } u; u.s.addr = func; u.s.adjustor = 0;
#endif

	(void) (reinterpret_cast<VfuncEmptyClass*>(this_ptr)->*u.mfpnew)(modelname);
}