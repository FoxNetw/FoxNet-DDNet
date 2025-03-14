/* (c) Shereef Marzouk. See "licence DDRace.txt" and the readme.txt in the root of the distribution for more information. */
#include "gamecontext.h"

#include <engine/antibot.h>

#include <engine/shared/config.h>
#include <game/server/entities/character.h>
#include <game/server/gamemodes/DDRace.h>
#include <game/server/player.h>
#include <game/server/save.h>
#include <game/server/teams.h>

#include <algorithm>

bool CheckClientId(int ClientId);

void CGameContext::ConGoLeft(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Tiles = pResult->NumArguments() == 1 ? pResult->GetInteger(0) : 1;

	if(!CheckClientId(pResult->m_ClientId))
		return;
	pSelf->MoveCharacter(pResult->m_ClientId, -1 * Tiles, 0);
}

void CGameContext::ConGoRight(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Tiles = pResult->NumArguments() == 1 ? pResult->GetInteger(0) : 1;

	if(!CheckClientId(pResult->m_ClientId))
		return;
	pSelf->MoveCharacter(pResult->m_ClientId, Tiles, 0);
}

void CGameContext::ConGoDown(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Tiles = pResult->NumArguments() == 1 ? pResult->GetInteger(0) : 1;

	if(!CheckClientId(pResult->m_ClientId))
		return;
	pSelf->MoveCharacter(pResult->m_ClientId, 0, Tiles);
}

void CGameContext::ConGoUp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Tiles = pResult->NumArguments() == 1 ? pResult->GetInteger(0) : 1;

	if(!CheckClientId(pResult->m_ClientId))
		return;
	pSelf->MoveCharacter(pResult->m_ClientId, 0, -1 * Tiles);
}

void CGameContext::ConMove(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientId(pResult->m_ClientId))
		return;
	pSelf->MoveCharacter(pResult->m_ClientId, pResult->GetInteger(0),
		pResult->GetInteger(1));
}

void CGameContext::ConMoveRaw(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientId(pResult->m_ClientId))
		return;
	pSelf->MoveCharacter(pResult->m_ClientId, pResult->GetInteger(0),
		pResult->GetInteger(1), true);
}

void CGameContext::MoveCharacter(int ClientId, int X, int Y, bool Raw)
{
	CCharacter *pChr = GetPlayerChar(ClientId);

	if(!pChr)
		return;

	pChr->Move(vec2((Raw ? 1 : 32) * X, (Raw ? 1 : 32) * Y));
	pChr->m_DDRaceState = DDRACE_CHEAT;
}

void CGameContext::ConKillPlayer(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientId(pResult->m_ClientId))
		return;
	int Victim = pResult->GetVictim();

	if(pSelf->m_apPlayers[Victim])
	{
		pSelf->m_apPlayers[Victim]->KillCharacter(WEAPON_GAME);
		char aBuf[512];
		str_format(aBuf, sizeof(aBuf), "%s was killed", pSelf->Server()->ClientName(Victim));
		pSelf->SendChat(-1, TEAM_ALL, aBuf);
	}
}

void CGameContext::ConNinja(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_NINJA, false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have The Weapon: Ninja >>", Victim);
}

void CGameContext::ConUnNinja(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_NINJA, true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost The Weapon: Ninja >>", Victim);
}

void CGameContext::ConEndlessHook(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;
	pChr->SetEndlessHook(true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have Endless Hook >>", Victim);
}

void CGameContext::ConUnEndlessHook(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;
	pChr->SetEndlessHook(false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost Endless Hook >>", Victim);
}

void CGameContext::ConSuper(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;
	if(pChr && !pChr->IsSuper())
	{
		pChr->SetSuper(true);
		pChr->UnFreeze();
		if(g_Config.m_SvCommandOutput)
			pSelf->SendBroadcast("<< You are now in Super >>", Victim);
	}
}

void CGameContext::ConUnSuper(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;
	if(pChr && pChr->IsSuper())
	{
		pChr->SetSuper(false);
		if(g_Config.m_SvCommandOutput)
			pSelf->SendBroadcast("<< You aren't in Super anymore >>", Victim);
	}
}

void CGameContext::ConToggleInvincible(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetInvincible(pResult->GetInteger(0));
	if(g_Config.m_SvCommandOutput)
	{
		if(pResult->GetInteger(0))
			pSelf->SendBroadcast("<< You are now Invincible! >>", Victim);
		else if(!pResult->GetInteger(0))
			pSelf->SendBroadcast("<< You aren't Invincible anymore! >>", Victim);
	}
}

void CGameContext::ConSolo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetSolo(true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You were put into Solo! >>", Victim);
}

void CGameContext::ConUnSolo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetSolo(false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You were put out of Solo! >>", Victim);
}

void CGameContext::ConFreeze(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->Freeze();
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You were frozen by magic! >>", Victim);
}

void CGameContext::ConUnFreeze(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->UnFreeze();
}

void CGameContext::ConDeep(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetDeepFrozen(true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have been bowling-balled! >>", Victim);
}

void CGameContext::ConUnDeep(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetDeepFrozen(false);
	pChr->UnFreeze();
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have been un-deep frozen! >>", Victim);
}

void CGameContext::ConLiveFreeze(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetLiveFrozen(true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have been live frozen! >>", Victim);
}

void CGameContext::ConUnLiveFreeze(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetLiveFrozen(false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have been un-live frozen! >>", Victim);
}

void CGameContext::ConShotgun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_SHOTGUN, false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have The Weapon: Shotgun! >>", Victim);
}

void CGameContext::ConGrenade(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_GRENADE, false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have The Weapon: Grenade! >>", Victim);
}

void CGameContext::ConLaser(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_LASER, false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have The Weapon: Laser! >>", Victim);
}

void CGameContext::ConJetpack(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetJetpack(true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have a Jetpack! >>", Victim);
}

void CGameContext::ConEndlessJump(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetEndlessJump(true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have Infinite Jumps >>", Victim);
}

void CGameContext::ConSetJumps(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	char aBuf[512];
	if(g_Config.m_SvCommandOutput)
	{
		if(pResult->GetInteger(0) > 1)
			str_format(aBuf, sizeof(aBuf), "<< You have %d Jumps >>", pResult->GetInteger(0));
		else
			str_format(aBuf, sizeof(aBuf), "<< You have %d Jump >>", pResult->GetInteger(0));
	}

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetJumps(pResult->GetInteger(0));
	pSelf->SendBroadcast(aBuf, Victim);
}

void CGameContext::ConWeapons(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_HAMMER, false);
	pChr->GiveWeapon(WEAPON_GUN, false);
	pChr->GiveWeapon(WEAPON_GRENADE, false);
	pChr->GiveWeapon(WEAPON_LASER, false);
	pChr->GiveWeapon(WEAPON_SHOTGUN, false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have All Weapons! >>", Victim);
}

void CGameContext::ConUnShotgun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_SHOTGUN, true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost The Weapon: Shotgun! >>", Victim);
}

void CGameContext::ConUnGrenade(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_GRENADE, true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost The Weapon: Grenade! >>", Victim);
}

void CGameContext::ConUnLaser(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_LASER, true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost The Weapon: Laser! >>", Victim);
}

void CGameContext::ConUnJetpack(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetJetpack(false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost Jetpack! >>", Victim);
}

void CGameContext::ConUnEndlessJump(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetEndlessJump(false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost Endless Jumps! >>", Victim);
}

void CGameContext::ConUnWeapons(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->GiveWeapon(WEAPON_GRENADE, true);
	pChr->GiveWeapon(WEAPON_LASER, true);
	pChr->GiveWeapon(WEAPON_SHOTGUN, true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You have lost all Weapons! >>", Victim);
}

void CGameContext::ConRemoveWeapon(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	const char *aBuf;
	if(pResult->GetInteger(0) == 0 && pChr->GetWeaponGot(WEAPON_HAMMER))
		aBuf = "<< You lost Weapon: Hammer! >>";
	else if(pResult->GetInteger(0) == 1 && pChr->GetWeaponGot(WEAPON_GUN))
		aBuf = "<< You lost Weapon: Gun! >>";
	else if(pResult->GetInteger(0) == 2 && pChr->GetWeaponGot(WEAPON_SHOTGUN))
		aBuf = "<< You lost Weapon: Shotgun! >>";
	else if(pResult->GetInteger(0) == 3 && pChr->GetWeaponGot(WEAPON_GRENADE))
		aBuf = "<< You lost Weapon: Grenade! >>";
	else if(pResult->GetInteger(0) == 4 && pChr->GetWeaponGot(WEAPON_LASER))
		aBuf = "<< You lost Weapon: Laser! >>";
	else if(pResult->GetInteger(0) == 5 && pChr->GetWeaponGot(WEAPON_NINJA))
		aBuf = "<< You lost Weapon: Ninja! >>";
	else if(pResult->GetInteger(0) == -1)
		aBuf = "<< You lost all Weapons! >>";
	else
		aBuf = "<< You have lost Special Weapon! >>";

	if(pResult->GetInteger(0) == -1)
	{
		pChr->GiveWeapon(WEAPON_HAMMER, true);
		pChr->GiveWeapon(WEAPON_GUN, true);
		pChr->GiveWeapon(WEAPON_GRENADE, true);
		pChr->GiveWeapon(WEAPON_LASER, true);
		pChr->GiveWeapon(WEAPON_SHOTGUN, true);
	}
	else
		pChr->GiveWeapon(pResult->GetInteger(0), true);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast(aBuf, Victim);
}

void CGameContext::ConAddWeapon(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();

	if(!CheckClientId(Victim))
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	const char *aBuf;
	if(pResult->GetInteger(0) == 0 && !pChr->GetWeaponGot(WEAPON_HAMMER))
		aBuf = "<< You have Weapon: Hammer! >>";
	else if(pResult->GetInteger(0) == 1 && !pChr->GetWeaponGot(WEAPON_GUN))
		aBuf = "<< You have Weapon: Gun! >>";
	else if(pResult->GetInteger(0) == 2 && !pChr->GetWeaponGot(WEAPON_SHOTGUN))
		aBuf = "<< You have Weapon: Shotgun! >>";
	else if(pResult->GetInteger(0) == 3 && !pChr->GetWeaponGot(WEAPON_GRENADE))
		aBuf = "<< You have Weapon: Grenade! >>";
	else if(pResult->GetInteger(0) == 4 && !pChr->GetWeaponGot(WEAPON_LASER))
		aBuf = "<< You have Weapon: Laser! >>";
	else if(pResult->GetInteger(0) == 6)
		aBuf = "<< You have Weapon: Ninja! >>";
	else if(pResult->GetInteger(0) == -1)
		aBuf = "<< You have all Weapons! >>";
	else
		aBuf = "<< You have a Special Weapon! >>";

	if(pResult->GetInteger(0) == -1)
	{
		pChr->GiveWeapon(WEAPON_HAMMER, false);
		pChr->GiveWeapon(WEAPON_GUN, false);
		pChr->GiveWeapon(WEAPON_GRENADE, false);
		pChr->GiveWeapon(WEAPON_LASER, false);
		pChr->GiveWeapon(WEAPON_SHOTGUN, false);
	}
	else
		pChr->GiveWeapon(pResult->GetInteger(0), false);

	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast(aBuf, Victim);
}

void CGameContext::ModifyWeapons(IConsole::IResult *pResult, void *pUserData,
	int Weapon, bool Remove)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	CCharacter *pChr = GetPlayerChar(pResult->m_ClientId);
	if(!pChr)
		return;

	if(clamp(Weapon, -1, NUM_OTHER_WEAPONS - 1) != Weapon)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "info",
			"invalid weapon id");
		return;
	}

	if(Weapon == -1)
	{
		pChr->GiveWeapon(WEAPON_SHOTGUN, Remove);
		pChr->GiveWeapon(WEAPON_GRENADE, Remove);
		pChr->GiveWeapon(WEAPON_LASER, Remove);
	}
	else
	{
		pChr->GiveWeapon(Weapon, Remove);
	}

	pChr->m_DDRaceState = DDRACE_CHEAT;
}

void CGameContext::Teleport(CCharacter *pChr, vec2 Pos)
{
	pChr->SetPosition(Pos);
	pChr->m_Pos = Pos;
	pChr->m_PrevPos = Pos;
	pChr->m_DDRaceState = DDRACE_CHEAT;
}

void CGameContext::ConToTeleporter(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	unsigned int TeleTo = pResult->GetInteger(0);

	if(!pSelf->Collision()->TeleOuts(TeleTo - 1).empty())
	{
		CCharacter *pChr = pSelf->GetPlayerChar(pResult->m_ClientId);
		if(pChr)
		{
			int TeleOut = pSelf->m_World.m_Core.RandomOr0(pSelf->Collision()->TeleOuts(TeleTo - 1).size());
			pSelf->Teleport(pChr, pSelf->Collision()->TeleOuts(TeleTo - 1)[TeleOut]);
		}
	}
}

void CGameContext::ConToCheckTeleporter(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	unsigned int TeleTo = pResult->GetInteger(0);

	if(!pSelf->Collision()->TeleCheckOuts(TeleTo - 1).empty())
	{
		CCharacter *pChr = pSelf->GetPlayerChar(pResult->m_ClientId);
		if(pChr)
		{
			int TeleOut = pSelf->m_World.m_Core.RandomOr0(pSelf->Collision()->TeleCheckOuts(TeleTo - 1).size());
			pSelf->Teleport(pChr, pSelf->Collision()->TeleCheckOuts(TeleTo - 1)[TeleOut]);
			pChr->m_TeleCheckpoint = TeleTo;
		}
	}
}

void CGameContext::ConTeleport(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Tele = pResult->NumArguments() == 2 ? pResult->GetInteger(0) : pResult->m_ClientId;
	int TeleTo = pResult->NumArguments() ? pResult->GetInteger(pResult->NumArguments() - 1) : pResult->m_ClientId;
	int AuthLevel = pSelf->Server()->GetAuthedState(pResult->m_ClientId);

	if(Tele != pResult->m_ClientId && AuthLevel < g_Config.m_SvTeleOthersAuthLevel)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "tele", "you aren't allowed to tele others");
		return;
	}

	CCharacter *pChr = pSelf->GetPlayerChar(Tele);
	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientId];

	if(pChr && pPlayer && pSelf->GetPlayerChar(TeleTo))
	{
		// default to view pos when character is not available
		vec2 Pos = pPlayer->m_ViewPos;
		if(pResult->NumArguments() == 0 && !pPlayer->IsPaused() && pChr->IsAlive())
		{
			vec2 Target = vec2(pChr->Core()->m_Input.m_TargetX, pChr->Core()->m_Input.m_TargetY);
			Pos = pPlayer->m_CameraInfo.ConvertTargetToWorld(pChr->GetPos(), Target);
		}
		pSelf->Teleport(pChr, Pos);
		pChr->ResetJumps();
		pChr->UnFreeze();
		pChr->SetVelocity(vec2(0, 0));
	}
}

void CGameContext::ConKill(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientId(pResult->m_ClientId))
		return;
	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientId];

	if(!pPlayer || (pPlayer->m_LastKill && pPlayer->m_LastKill + pSelf->Server()->TickSpeed() * g_Config.m_SvKillDelay > pSelf->Server()->Tick()))
		return;

	pPlayer->m_LastKill = pSelf->Server()->Tick();
	pPlayer->KillCharacter(WEAPON_SELF);
}

void CGameContext::ConForcePause(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();
	int Seconds = 0;
	if(pResult->NumArguments() > 1)
		Seconds = clamp(pResult->GetInteger(1), 0, 360);

	CPlayer *pPlayer = pSelf->m_apPlayers[Victim];
	if(!pPlayer)
		return;

	pPlayer->ForcePause(Seconds);
}

bool CGameContext::TryVoteMute(const NETADDR *pAddr, int Secs, const char *pReason)
{
	// find a matching vote mute for this ip, update expiration time if found
	for(int i = 0; i < m_NumVoteMutes; i++)
	{
		if(net_addr_comp_noport(&m_aVoteMutes[i].m_Addr, pAddr) == 0)
		{
			m_aVoteMutes[i].m_Expire = Server()->Tick() + Secs * Server()->TickSpeed();
			str_copy(m_aVoteMutes[i].m_aReason, pReason, sizeof(m_aVoteMutes[i].m_aReason));
			return true;
		}
	}

	// nothing to update create new one
	if(m_NumVoteMutes < MAX_VOTE_MUTES)
	{
		m_aVoteMutes[m_NumVoteMutes].m_Addr = *pAddr;
		m_aVoteMutes[m_NumVoteMutes].m_Expire = Server()->Tick() + Secs * Server()->TickSpeed();
		str_copy(m_aVoteMutes[m_NumVoteMutes].m_aReason, pReason, sizeof(m_aVoteMutes[m_NumVoteMutes].m_aReason));
		m_NumVoteMutes++;
		return true;
	}
	// no free slot found
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "votemute", "vote mute array is full");
	return false;
}

void CGameContext::VoteMute(const NETADDR *pAddr, int Secs, const char *pReason, const char *pDisplayName, int AuthedId)
{
	if(!TryVoteMute(pAddr, Secs, pReason))
		return;
	if(!pDisplayName)
		return;

	char aBuf[128];
	if(pReason[0])
		str_format(aBuf, sizeof(aBuf), "'%s' banned '%s' for %d seconds from voting (%s)",
			Server()->ClientName(AuthedId), pDisplayName, Secs, pReason);
	else
		str_format(aBuf, sizeof(aBuf), "'%s' banned '%s' for %d seconds from voting",
			Server()->ClientName(AuthedId), pDisplayName, Secs);
	SendChat(-1, TEAM_ALL, aBuf);
}

bool CGameContext::VoteUnmute(const NETADDR *pAddr, const char *pDisplayName, int AuthedId)
{
	for(int i = 0; i < m_NumVoteMutes; i++)
	{
		if(net_addr_comp_noport(&m_aVoteMutes[i].m_Addr, pAddr) == 0)
		{
			m_NumVoteMutes--;
			m_aVoteMutes[i] = m_aVoteMutes[m_NumVoteMutes];
			if(pDisplayName)
			{
				char aBuf[128];
				str_format(aBuf, sizeof(aBuf), "'%s' unbanned '%s' from voting.",
					Server()->ClientName(AuthedId), pDisplayName);
				Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "voteunmute", aBuf);
			}
			return true;
		}
	}
	return false;
}

bool CGameContext::TryMute(const NETADDR *pAddr, int Secs, const char *pReason, bool InitialChatDelay)
{
	// find a matching mute for this ip, update expiration time if found
	for(int i = 0; i < m_NumMutes; i++)
	{
		if(net_addr_comp_noport(&m_aMutes[i].m_Addr, pAddr) == 0)
		{
			const int NewExpire = Server()->Tick() + Secs * Server()->TickSpeed();
			if(NewExpire > m_aMutes[i].m_Expire)
			{
				m_aMutes[i].m_Expire = NewExpire;
				str_copy(m_aMutes[i].m_aReason, pReason, sizeof(m_aMutes[i].m_aReason));
				m_aMutes[i].m_InitialChatDelay = InitialChatDelay;
			}
			return true;
		}
	}

	// nothing to update create new one
	if(m_NumMutes < MAX_MUTES)
	{
		m_aMutes[m_NumMutes].m_Addr = *pAddr;
		m_aMutes[m_NumMutes].m_Expire = Server()->Tick() + Secs * Server()->TickSpeed();
		str_copy(m_aMutes[m_NumMutes].m_aReason, pReason, sizeof(m_aMutes[m_NumMutes].m_aReason));
		m_aMutes[m_NumMutes].m_InitialChatDelay = InitialChatDelay;
		m_NumMutes++;
		return true;
	}
	// no free slot found
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", "mute array is full");
	return false;
}

void CGameContext::Mute(const NETADDR *pAddr, int Secs, const char *pDisplayName, const char *pReason, bool InitialChatDelay)
{
	if(Secs <= 0)
		return;
	if(!TryMute(pAddr, Secs, pReason, InitialChatDelay))
		return;
	if(InitialChatDelay)
		return;
	if(!pDisplayName)
		return;

	char aBuf[128];
	if(pReason[0])
		str_format(aBuf, sizeof(aBuf), "'%s' has been muted for %d seconds (%s)", pDisplayName, Secs, pReason);
	else
		str_format(aBuf, sizeof(aBuf), "'%s' has been muted for %d seconds", pDisplayName, Secs);
	SendChat(-1, TEAM_ALL, aBuf);
}

void CGameContext::ConVoteMute(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	if(Victim < 0 || Victim > MAX_CLIENTS || !pSelf->m_apPlayers[Victim])
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "votemute", "Client ID not found");
		return;
	}

	const NETADDR *pAddr = pSelf->Server()->ClientAddr(Victim);

	int Seconds = clamp(pResult->GetInteger(1), 1, 86400);
	const char *pReason = pResult->NumArguments() > 2 ? pResult->GetString(2) : "";
	pSelf->VoteMute(pAddr, Seconds, pReason, pSelf->Server()->ClientName(Victim), pResult->m_ClientId);
}

void CGameContext::ConVoteUnmute(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	if(Victim < 0 || Victim > MAX_CLIENTS || !pSelf->m_apPlayers[Victim])
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "voteunmute", "Client ID not found");
		return;
	}

	const NETADDR *pAddr = pSelf->Server()->ClientAddr(Victim);

	bool Found = pSelf->VoteUnmute(pAddr, pSelf->Server()->ClientName(Victim), pResult->m_ClientId);
	if(Found)
	{
		char aBuf[128];
		str_format(aBuf, sizeof(aBuf), "'%s' unbanned '%s' from voting.",
			pSelf->Server()->ClientName(pResult->m_ClientId), pSelf->Server()->ClientName(Victim));
		pSelf->SendChat(-1, 0, aBuf);
	}
}

void CGameContext::ConVoteMutes(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	if(pSelf->m_NumVoteMutes <= 0)
	{
		// Just to make sure.
		pSelf->m_NumVoteMutes = 0;
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "votemutes",
			"There are no active vote mutes.");
		return;
	}

	char aIpBuf[NETADDR_MAXSTRSIZE];
	char aBuf[128];
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "votemutes",
		"Active vote mutes:");
	for(int i = 0; i < pSelf->m_NumVoteMutes; i++)
	{
		net_addr_str(&pSelf->m_aVoteMutes[i].m_Addr, aIpBuf, sizeof(aIpBuf), false);
		str_format(aBuf, sizeof(aBuf), "%d: \"%s\", %d seconds left (%s)", i,
			aIpBuf, (pSelf->m_aVoteMutes[i].m_Expire - pSelf->Server()->Tick()) / pSelf->Server()->TickSpeed(), pSelf->m_aVoteMutes[i].m_aReason);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "votemutes", aBuf);
	}
}

void CGameContext::ConMute(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->Console()->Print(
		IConsole::OUTPUT_LEVEL_STANDARD,
		"mutes",
		"Use either 'muteid <client_id> <seconds> <reason>' or 'muteip <ip> <seconds> <reason>'");
}

// mute through client id
void CGameContext::ConMuteId(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	if(Victim < 0 || Victim > MAX_CLIENTS || !pSelf->m_apPlayers[Victim])
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "muteid", "Client id not found.");
		return;
	}

	const NETADDR *pAddr = pSelf->Server()->ClientAddr(Victim);

	const char *pReason = pResult->NumArguments() > 2 ? pResult->GetString(2) : "";

	pSelf->Mute(pAddr, clamp(pResult->GetInteger(1), 1, 86400),
		pSelf->Server()->ClientName(Victim), pReason);
}

// mute through ip, arguments reversed to workaround parsing
void CGameContext::ConMuteIp(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	NETADDR Addr;
	if(net_addr_from_str(&Addr, pResult->GetString(0)))
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes",
			"Invalid network address to mute");
	}
	const char *pReason = pResult->NumArguments() > 2 ? pResult->GetString(2) : "";
	pSelf->Mute(&Addr, clamp(pResult->GetInteger(1), 1, 86400), nullptr, pReason);
}

// unmute by mute list index
void CGameContext::ConUnmute(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Index = pResult->GetInteger(0);

	if(Index < 0 || Index >= pSelf->m_NumMutes)
		return;

	char aIpBuf[NETADDR_MAXSTRSIZE];
	char aBuf[64];
	net_addr_str(&pSelf->m_aMutes[Index].m_Addr, aIpBuf, sizeof(aIpBuf), false);
	str_format(aBuf, sizeof(aBuf), "Unmuted %s", aIpBuf);
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);

	pSelf->m_NumMutes--;
	pSelf->m_aMutes[Index] = pSelf->m_aMutes[pSelf->m_NumMutes];
}

// unmute by player id
void CGameContext::ConUnmuteId(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	if(Victim < 0 || Victim > MAX_CLIENTS || !pSelf->m_apPlayers[Victim])
		return;

	const NETADDR *pAddr = pSelf->Server()->ClientAddr(Victim);

	for(int i = 0; i < pSelf->m_NumMutes; i++)
	{
		if(net_addr_comp_noport(&pSelf->m_aMutes[i].m_Addr, pAddr) == 0)
		{
			char aIpBuf[NETADDR_MAXSTRSIZE];
			char aBuf[64];
			net_addr_str(&pSelf->m_aMutes[i].m_Addr, aIpBuf, sizeof(aIpBuf), false);
			str_format(aBuf, sizeof(aBuf), "Unmuted %s", aIpBuf);
			pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);
			pSelf->m_NumMutes--;
			pSelf->m_aMutes[i] = pSelf->m_aMutes[pSelf->m_NumMutes];
			return;
		}
	}
}

// list mutes
void CGameContext::ConMutes(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	if(pSelf->m_NumMutes <= 0)
	{
		// Just to make sure.
		pSelf->m_NumMutes = 0;
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes",
			"There are no active mutes.");
		return;
	}

	char aIpBuf[64];
	char aBuf[128];
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes",
		"Active mutes:");
	for(int i = 0; i < pSelf->m_NumMutes; i++)
	{
		net_addr_str(&pSelf->m_aMutes[i].m_Addr, aIpBuf, sizeof(aIpBuf), false);
		str_format(aBuf, sizeof(aBuf), "%d: \"%s\", %d seconds left (%s)", i, aIpBuf,
			(pSelf->m_aMutes[i].m_Expire - pSelf->Server()->Tick()) / pSelf->Server()->TickSpeed(), pSelf->m_aMutes[i].m_aReason);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "mutes", aBuf);
	}
}

void CGameContext::ConModerate(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	if(!CheckClientId(pResult->m_ClientId))
		return;

	bool HadModerator = pSelf->PlayerModerating();

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientId];
	pPlayer->m_Moderating = !pPlayer->m_Moderating;

	if(!HadModerator && pPlayer->m_Moderating)
		pSelf->SendChat(-1, TEAM_ALL, "Server kick/spec votes will now be actively moderated.", 0);

	if(!pSelf->PlayerModerating())
		pSelf->SendChat(-1, TEAM_ALL, "Server kick/spec votes are no longer actively moderated.", 0);

	if(pPlayer->m_Moderating)
		pSelf->SendChatTarget(pResult->m_ClientId, "Active moderator mode enabled for you.");
	else
		pSelf->SendChatTarget(pResult->m_ClientId, "Active moderator mode disabled for you.");
}

void CGameContext::ConSetDDRTeam(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	auto *pController = pSelf->m_pController;

	if(g_Config.m_SvTeam == SV_TEAM_FORBIDDEN || g_Config.m_SvTeam == SV_TEAM_FORCED_SOLO)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "join",
			"Teams are disabled");
		return;
	}

	int Target = pResult->GetVictim();
	int Team = pResult->GetInteger(1);

	if(Team < TEAM_FLOCK || Team >= TEAM_SUPER)
		return;

	CCharacter *pChr = pSelf->GetPlayerChar(Target);

	if((pSelf->GetDDRaceTeam(Target) && pController->Teams().GetDDRaceState(pSelf->m_apPlayers[Target]) == DDRACE_STARTED) || (pChr && pController->Teams().IsPractice(pChr->Team())))
		pSelf->m_apPlayers[Target]->KillCharacter(WEAPON_GAME);

	pController->Teams().SetForceCharacterTeam(Target, Team);
}

void CGameContext::ConUninvite(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	auto *pController = pSelf->m_pController;

	pController->Teams().SetClientInvited(pResult->GetInteger(1), pResult->GetVictim(), false);
}

void CGameContext::ConFreezeHammer(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "'%s' got freeze hammer!",
		pSelf->Server()->ClientName(Victim));
	pSelf->SendChat(-1, TEAM_ALL, aBuf);

	pChr->m_FreezeHammer = true;
}

void CGameContext::ConUnFreezeHammer(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	char aBuf[128];
	str_format(aBuf, sizeof(aBuf), "'%s' lost freeze hammer!",
		pSelf->Server()->ClientName(Victim));
	pSelf->SendChat(-1, TEAM_ALL, aBuf);

	pChr->m_FreezeHammer = false;
}
void CGameContext::ConVoteNo(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	pSelf->ForceVote(pResult->m_ClientId, false);
}

void CGameContext::ConDrySave(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;

	CPlayer *pPlayer = pSelf->m_apPlayers[pResult->m_ClientId];

	if(!pPlayer || pSelf->Server()->GetAuthedState(pResult->m_ClientId) != AUTHED_ADMIN)
		return;

	CSaveTeam SavedTeam;
	int Team = pSelf->GetDDRaceTeam(pResult->m_ClientId);
	ESaveResult Result = SavedTeam.Save(pSelf, Team, true);
	if(CSaveTeam::HandleSaveError(Result, pResult->m_ClientId, pSelf))
		return;

	char aTimestamp[32];
	str_timestamp(aTimestamp, sizeof(aTimestamp));
	char aBuf[64];
	str_format(aBuf, sizeof(aBuf), "%s_%s_%s.save", pSelf->Server()->GetMapName(), aTimestamp, pSelf->Server()->GetAuthName(pResult->m_ClientId));
	IOHANDLE File = pSelf->Storage()->OpenFile(aBuf, IOFLAG_WRITE, IStorage::TYPE_SAVE);
	if(!File)
		return;

	int Len = str_length(SavedTeam.GetString());
	io_write(File, SavedTeam.GetString(), Len);
	io_close(File);
}

void CGameContext::ConReloadCensorlist(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->ReadCensorList();
}

void CGameContext::ConDumpAntibot(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->Antibot()->ConsoleCommand("dump");
}

void CGameContext::ConAntibot(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->Antibot()->ConsoleCommand(pResult->GetString(0));
}

void CGameContext::ConDumpLog(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int LimitSecs = MAX_LOG_SECONDS;
	if(pResult->NumArguments() > 0)
		LimitSecs = pResult->GetInteger(0);

	if(LimitSecs < 0)
		return;

	int Iterator = pSelf->m_LatestLog;
	for(int i = 0; i < MAX_LOGS; i++)
	{
		CLog *pEntry = &pSelf->m_aLogs[Iterator];
		Iterator = (Iterator + 1) % MAX_LOGS;

		if(!pEntry->m_Timestamp)
			continue;

		int Seconds = (time_get() - pEntry->m_Timestamp) / time_freq();
		if(Seconds > LimitSecs)
			continue;

		char aBuf[256];
		if(pEntry->m_FromServer)
			str_format(aBuf, sizeof(aBuf), "%s, %d seconds ago", pEntry->m_aDescription, Seconds);
		else
			str_format(aBuf, sizeof(aBuf), "%s, %d seconds ago < addr=<{%s}> name='%s' client=%d",
				pEntry->m_aDescription, Seconds, pEntry->m_aClientAddrStr, pEntry->m_aClientName, pEntry->m_ClientVersion);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "log", aBuf);
	}
}

void CGameContext::LogEvent(const char *Description, int ClientId)
{
	CLog *pNewEntry = &m_aLogs[m_LatestLog];
	m_LatestLog = (m_LatestLog + 1) % MAX_LOGS;

	pNewEntry->m_Timestamp = time_get();
	str_copy(pNewEntry->m_aDescription, Description);
	pNewEntry->m_FromServer = ClientId < 0;
	if(!pNewEntry->m_FromServer)
	{
		pNewEntry->m_ClientVersion = Server()->GetClientVersion(ClientId);
		str_copy(pNewEntry->m_aClientAddrStr, Server()->ClientAddrString(ClientId, false));
		str_copy(pNewEntry->m_aClientName, Server()->ClientName(ClientId));
	}
}

void CGameContext::ConHeadItem(IConsole::IResult *pResult, void *pUserData)
{	
	CGameContext *pSelf = (CGameContext *)pUserData;

	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();

	if(pResult->GetInteger(1) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(pChr)
		pChr->HeadItem(pResult->GetInteger(0), Victim);
}

void CGameContext::ConExplosionGun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetExplosionGun(!pChr->Core()->m_ExplosionGun);
	if(g_Config.m_SvCommandOutput)
	{
		if(pChr->Core()->m_ExplosionGun)
			pSelf->SendBroadcast("<< You Have Explosion Gun! >>", Victim);
		else
			pSelf->SendBroadcast("<< You lost Explosion Gun! >>", Victim);

	}
}

void CGameContext::ConUnExplosionGun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetExplosionGun(false);
	if(g_Config.m_SvCommandOutput)
		pSelf->SendBroadcast("<< You lost Explosion Gun! >>", Victim);
}

void CGameContext::ConTelekinesisImmunity(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetTelekinesisImmunity(!pChr->Core()->m_TelekinesisImmunity);

	if(pChr->Core()->m_TelekinesisImmunity)
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "Immune to Telekinesis");
		if(g_Config.m_SvCommandOutput)

			pSelf->SendBroadcast("<< You are Immunte to Telekinesis! >>", Victim);
	}
	else
	{
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "Not Immune to Telekinesis Anymore");

		if(g_Config.m_SvCommandOutput)
			pSelf->SendBroadcast("<< You aren't Immunte to Telekinesis anymore! >>", Victim);
	}
}

void CGameContext::ConTelekinesis(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	bool GotWeapon = pChr->GetWeaponGot(WEAPON_TELEKINESIS);

	if(GotWeapon)
		pChr->GiveWeapon(WEAPON_TELEKINESIS, true);
	else
		pChr->GiveWeapon(WEAPON_TELEKINESIS);

	if(g_Config.m_SvCommandOutput)
	{
		if(GotWeapon)
			pSelf->SendBroadcast("<< You have Telekinesis >>", Victim);
		else
			pSelf->SendBroadcast("<< You lost Telekinesis >>", Victim);
	}
}

void CGameContext::ConHeartGun(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() > 1 ? pResult->m_ClientId : pResult->GetVictim();

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	bool GotWeapon = pChr->GetWeaponGot(WEAPON_HEART_GUN);

	if(GotWeapon)
		pChr->GiveWeapon(WEAPON_HEART_GUN, true);
	else
		pChr->GiveWeapon(WEAPON_HEART_GUN);

	if(g_Config.m_SvCommandOutput)
	{
		if(GotWeapon)
			pSelf->SendBroadcast("<< You have a Heart Gun >>", Victim);
		else
			pSelf->SendBroadcast("<< You lost the Heart Gun >>", Victim);
	}
}

void CGameContext::ConPlaySoundGlobal(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->CreateSoundGlobal(pResult->GetInteger(0));
}

void CGameContext::ConListSounds(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "List of Available Sounds:");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "0  = Gun Fire");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "1  = Shot Gun Fire");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "2  = Grenade Fire");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "3  = Hammer Fir");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "4  = Hammer Hit");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "5  = Ninja Fire");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "6  = Grenade Explosion");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "7  = Ninja Hit");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "8  = Laser Fire");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "9  = Laser Bounce");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "10 = Weapon Switch");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "11 = Player Pain Short");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "12 = Player Pain Long");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "13 = Body Land");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "14 = Player Airjump");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "15 = Player Jump");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "16 = Player Die");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "17 = Player Spawn");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "18 = Player Skid");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "19 = Tee Cry");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "20 = Hook Loop");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "21 = Hook Attach Ground");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "22 = Hook Attach Player");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "23 = Hook Sound No-Attach");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "24 = Pickup Health");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "25 = Pickup Armor");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "26 = Pickup Grenade");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "27 = Pickup Shotgun");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "28 = Pickup Ninja");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "29 = Weapon Spawn");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "30 = Weapon No-Ammo");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "31 = Hit");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "32 = Chat Server");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "33 = Chat Client");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "34 = Chat Highlight");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "35 = CTF Drop");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "36 = CTF Return");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "37 = CTF grab PL");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "37 = CTF grab PL");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "38 = CTF Grab EN");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "39 = CTF Flag Capture");
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", "40 = Menu");
}

void CGameContext::ConSetPlayerClan(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();
	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->Server()->SetClientClan(Victim, pResult->GetString(1));
}

void CGameContext::ConSetPlayerName(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();
	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->Server()->SetClientName(Victim, pResult->GetString(1));
}

void CGameContext::ConSetPlayerSkin(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();
	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	str_copy(pSelf->m_apPlayers[Victim]->m_TeeInfos.m_aSkinName, pResult->GetString(1));
}

void CGameContext::ConSetPlayerCustomColor(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();
	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pSelf->m_apPlayers[Victim]->m_TeeInfos.m_UseCustomColor = pResult->GetInteger(1);
}

void CGameContext::ConSetPlayerColorBody(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();	
	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pSelf->m_apPlayers[Victim]->m_TeeInfos.m_ColorBody = pResult->GetInteger(1);
}

void CGameContext::ConSetPlayerColorFeet(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->m_ClientId;
	if(pResult->NumArguments() > 1)
		Victim = pResult->GetVictim();
	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pSelf->m_apPlayers[Victim]->m_TeeInfos.m_ColorFeet = pResult->GetInteger(1);
}

void CGameContext::ConRainbow(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() ? pResult->GetVictim() : pResult->m_ClientId;

	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;
	
	if(pChr->Core()->m_Rainbow)
		pChr->RestoreColor();
	else
		pChr->SaveColor();

	pChr->SetRainbow(!pChr->Core()->m_Rainbow);
}

void CGameContext::ConRainbowSpeed(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() ? pResult->GetVictim() : pResult->m_ClientId;

	if(pResult->GetInteger(0) == -1)
		Victim = pResult->m_ClientId;

	CPlayer *pPlayer = pSelf->m_apPlayers[Victim];

	if(!pPlayer)
		return;

	char aBuf[64];
	if(pResult->NumArguments() < 2)
	{
		str_format(aBuf, sizeof(aBuf), "Speed: %d", pPlayer->m_RainbowSpeed);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", aBuf);
	}
	else
	{
		int Speed = clamp(pResult->GetInteger(1), 1, 200);
		str_format(aBuf, sizeof(aBuf), "Rainbow speed for '%s' changed to %d", pSelf->Server()->ClientName(Victim), Speed);
		pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", aBuf);
		pPlayer->m_RainbowSpeed = Speed;
	}
}

void CGameContext::ConInvisible(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() ? pResult->GetVictim() : pResult->m_ClientId;
	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	pChr->SetInvisible(!pChr->Core()->m_Invisible);
}

void CGameContext::ConNextBanSync(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	int Victim = pResult->NumArguments() ? pResult->GetVictim() : pResult->m_ClientId;
	CCharacter *pChr = pSelf->GetPlayerChar(Victim);

	if(!pChr)
		return;

	char aBuf[256];
	str_format(aBuf, sizeof(aBuf), "Next Sync: %d Seconds", (pChr->GameServer()->m_BanSaveDelay - pChr->Server()->Tick()) / pChr->Server()->TickSpeed());
	pSelf->Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", aBuf);
}

void CGameContext::ConDisallowedWords(IConsole::IResult *pResult, void *pUserData)
{
	CGameContext *pSelf = (CGameContext *)pUserData;
	pSelf->DisallowedNeedles(pResult->GetString(0), pResult->GetInteger(1));
}

void CGameContext::DisallowedNeedles(const char *Needle, bool Remove)
{
	// Wonky wonky code :c

	char aBuf[512];
	str_copy(aBuf, "");

	AutoBanNeedles Entry(Needle);

	if(str_comp(Needle, "") != 0)
	{
		if(Remove)
		{
			auto it = std::find(m_disallowedStrings.begin(), m_disallowedStrings.end(), Entry);
			if(it != m_disallowedStrings.end())
			{
				m_disallowedStrings.erase(it);
				str_format(aBuf, sizeof(aBuf), "Removed \"%s\" from disallowed words", Needle);
				Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", aBuf);
			}
		}
		else
		{
			m_disallowedStrings.push_back(Needle);
			str_format(aBuf, sizeof(aBuf), "Added \"%s\" to disallowed words", Needle);
			Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", aBuf);
		}
	}

	str_copy(aBuf, "");
	for(int i = 0; i < m_disallowedStrings.size(); i++)
	{
		char AddStrig[512];
		str_format(AddStrig, sizeof(AddStrig), "%s, ", m_disallowedStrings.at(i));
		str_append(aBuf, AddStrig);
	}
	Console()->Print(IConsole::OUTPUT_LEVEL_STANDARD, "FoxNet", aBuf);
}