// made by fokkonaut
// edited by qxdFox
#include "votingmenu.h"
#include <game/server/entities/character.h>
#include <game/server/gamecontext.h>
#include <game/server/teams.h>
#include <engine/shared/config.h>
#include <game/server/gamemodes/DDRace.h>
#include <engine/server/server.h>
#include <game/server/player.h>


CGameContext *CVotingMenu::GameServer() const { return m_pGameServer; }
IServer *CVotingMenu::Server() const { return GameServer()->Server(); }

// Font: https://fsymbols.com/generators/smallcaps/
// Not a normal space: https://www.cogsci.ed.ac.uk/~richard/utf-8.cgi?input=%E2%80%8A&mode=char
static const char *PLACEHOLDER_DESC = " ";
// Acc
static const char *COLLAPSE_HEADER_WANTED_PLAYERS = "Wᴀɴᴛᴇᴅ Pʟᴀʏᴇʀs";
static const char *COLLAPSE_HEADER_ACC_INFO = "Aᴄᴄᴏᴜɴᴛ Iɴғᴏ";
static const char *COLLAPSE_HEADER_ACC_STATS = "Aᴄᴄᴏᴜɴᴛ Sᴛᴀᴛs";
static const char *COLLAPSE_HEADER_PLOT_INFO = "Pʟᴏᴛ";
// Wanted // careful when adding another "Page", it has to differ somehow
static const char *ACC_WANTED_PLAYERS_PAGE = "Page";
// Acc Misc
static const char *ACC_MISC_NINJAJETPACK = "Ninjajetpack";
// Misc
static const char *MISC_AFKSPECTATE = "Spectate if Afk";
static const char *MISC_WEAPONINDICATOR = "Weapon Indicator";
static const char *MISC_HIDEBROADCASTS = "Hide Broadcasts";

// VIP
static const char *ACC_RAINBOW = "Rainbow";
static const char *ACC_BLOODY = "Bloody";
static const char *ACC_ATOM = "Atom";
static const char *ACC_TRAIL = "Trail";
// VIP Plus
static const char *ACC_RAINBOWHOOK = "Rainbow Hook";
static const char *ACC_ROTATINGBALL = "Rotating Ball";
static const char *ACC_EPICCIRCLE = "Epic Circle";
static const char *ACC_LOVELY = "Lovely";
static const char *ACC_RAINBOWSPEED = "Rainbow Speed";
static const char *ACC_SPARKLE = "Sparkle";

void CVotingMenu::Init(CGameContext *pGameServer)
{
	m_pGameServer = pGameServer;

	m_NumCollapseEntries = 0;
	m_vWantedPlayers.clear();

	for (int i = 0; i < MAX_CLIENTS; i++)
	{
		Reset(i);
	}

	str_copy(m_aPages[PAGE_VOTES].m_aName, "Vᴏᴛᴇs", sizeof(m_aPages[PAGE_VOTES].m_aName));
	str_copy(m_aPages[PAGE_ACCOUNT].m_aName, "Aᴄᴄᴏᴜɴᴛ", sizeof(m_aPages[PAGE_ACCOUNT].m_aName));
	str_copy(m_aPages[PAGE_MISCELLANEOUS].m_aName, "Mɪsᴄᴇʟʟᴀɴᴇᴏᴜs", sizeof(m_aPages[PAGE_MISCELLANEOUS].m_aName));

	for (int i = 0; i < NUM_PAGES; i++)
		for (int j = 0; j < NUM_PAGE_MAX_VOTES; j++)
			m_aPages[i].m_aaTempDesc[j][0] = 0;

	// Don't add the placeholders again when the map changes. The voteOptionHeap in CGameContext is not destroyed, so we do not have to add placeholders another time
	if (!m_Initialized)
	{
		AddPlaceholderVotes();
		m_Initialized = true;
	}
}

void CVotingMenu::Reset(int ClientID)
{
	m_aClients[ClientID].m_Page = PAGE_MISCELLANEOUS;
	m_aClients[ClientID].m_LastVoteMsg = 0;
	m_aClients[ClientID].m_NextVoteSendTick = 0;
	m_aClients[ClientID].m_ShowAccountInfo = false;
	m_aClients[ClientID].m_ShowAccountStats = false;
}

void CVotingMenu::InitPlayer(int ClientID)
{
	if(!Server()->IsMain(ClientID))
	{
		int DummyID = Server()->GetDummy(ClientID);
		if(DummyID != -1)
		{
			m_aClients[ClientID].m_Page = m_aClients[DummyID].m_Page;
		}
	}
}

void CVotingMenu::AddPlaceholderVotes()
{
	for (int i = 0; i < NUM_OPTION_START_OFFSET; i++)
	{
		GameServer()->AddVote("placeholder_voting_menu", "info");
	}
}

const char *CVotingMenu::GetPageDescription(int ClientID, int Page)
{
	static char aBuf[64];
	str_format(aBuf, sizeof(aBuf), "%s %s", Page == GetPage(ClientID) ? "☒" : "☐", m_aPages[Page].m_aName);
	return aBuf;
}

bool CVotingMenu::SetPage(int ClientID, int Page)
{
	if (Page < 0 || Page >= NUM_PAGES || Page == GetPage(ClientID))
		return false;

	m_aClients[ClientID].m_Page = Page;
	// Update dummy page, because otherwise dummy could be in vote page, so if you switch page, swap to dummy, and vote smth, it'll actually call a vote
	int DummyID = Server()->GetDummy(ClientID);
	if (DummyID != -1)
		m_aClients[DummyID].m_Page = Page;

	// Don't process normal votes, e.g. when another vote get's added
	if (Page != PAGE_VOTES)
	{
		if (GameServer()->m_apPlayers[ClientID]->m_SendVoteIndex != -1)
		{
			CNetMsg_Sv_VoteOptionGroupEnd EndMsg;
			Server()->SendPackMsg(&EndMsg, MSGFLAG_VITAL, ClientID);
			GameServer()->m_apPlayers[ClientID]->m_SendVoteIndex = -1;
			if (DummyID != -1)
				GameServer()->m_apPlayers[DummyID]->m_SendVoteIndex = -1;
		}
	}
	SendPageVotes(ClientID);
	return true;
}

void CVotingMenu::OnProgressVoteOptions(int ClientID, CMsgPacker *pPacker, int *pCurIndex, CVoteOptionServer **ppCurrent)
{
	// Just started sending votes, add the pages
	bool IsPageVotes = GetPage(ClientID) == PAGE_VOTES;
	int Index = IsPageVotes ? GameServer()->m_apPlayers[ClientID]->m_SendVoteIndex : 0;
	if (IsPageVotes && (Index < 0 || Index >= NUM_OPTION_START_OFFSET))
		return;

	// Depending on votesPerTick... That's why we start with an offset maybe
	for (int i = Index; i < NUM_OPTION_START_OFFSET; i++)
	{
		if (i < NUM_PAGES)
		{
			pPacker->AddString(GetPageDescription(ClientID, i), VOTE_DESC_LENGTH);
		}
		else
		{
			pPacker->AddString(PLACEHOLDER_DESC, VOTE_DESC_LENGTH);
		}
		if (ppCurrent && *ppCurrent)
			*ppCurrent = (*ppCurrent)->m_pNext;
		(*pCurIndex)++;
	}
}

bool CVotingMenu::OnMessage(int ClientID, const CNetMsg_Cl_CallVote *pMsg)
{
	if (m_aClients[ClientID].m_LastVoteMsg + Server()->TickSpeed() / 2 > Server()->Tick())
		return true;
	m_aClients[ClientID].m_LastVoteMsg = Server()->Tick();

	if(!str_utf8_check(pMsg->m_pType) || !str_utf8_check(pMsg->m_pValue) || !str_utf8_check(pMsg->m_pReason))
	{
		return true;
	}

	bool VoteOption = str_comp_nocase(pMsg->m_pType, "option") == 0;
	//bool VoteKick = str_comp_nocase(pMsg->m_Type, "kick") == 0;
	//bool VoteSpectate = str_comp_nocase(pMsg->m_Type, "spectate") == 0;

	if (!VoteOption)
	{
		// Process normal voting, we don't use kick/spec player votes currently
		return false;
	}

	int WantedPage = -1;
	for (int i = 0; i < NUM_PAGES; i++)
	{
		if (str_comp(pMsg->m_pValue, GetPageDescription(ClientID, i)) == 0)
		{
			WantedPage = i;
			break;
		}
	}

	if (WantedPage != -1)
	{
		SetPage(ClientID, WantedPage);
		return true;
	}

	if (GetPage(ClientID) == PAGE_VOTES)
	{
		// placeholder
		if (!pMsg->m_pValue[0])
			return true;
		// Process normal voting
		return false;
	}

	PrepareTempDescriptions(ClientID);

	const char *pDesc = 0;
	for (int i = 0; i < NUM_PAGE_MAX_VOTES; i++)
	{
		const char *pFullDesc = str_skip_voting_menu_prefixes(m_aPages[GetPage(ClientID)].m_aaTempDesc[i]);
		if (!pFullDesc || !pFullDesc[0])
			continue;

		const char *pValue = str_skip_voting_menu_prefixes(pMsg->m_pValue);
		if (pValue && pValue[0] && str_comp(pFullDesc, pValue) == 0)
		{
			pDesc = pFullDesc;
			break;
		}
	}

	// shouldnt happen, silent ignore
	if (!pDesc)
		return true;

	// Process voting option
	if (OnMessageSuccess(ClientID, pDesc, pMsg->m_pReason))
	{
		SendPageVotes(ClientID);
	}
	return true;
}

bool CVotingMenu::OnMessageSuccess(int ClientID, const char *pDesc, const char *pReason)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	CCharacter *pChr = pPlayer->GetCharacter();

	// For now set it to -1, if not a number
	int Reason = (pReason[0] && str_isallnum(pReason)) ? str_toint(pReason) : -1;

	if(GetPage(ClientID) == PAGE_ACCOUNT)
	{
		// Acc info
		if(IsOptionWithSuffix(pDesc, COLLAPSE_HEADER_ACC_INFO))
		{
			m_aClients[ClientID].m_ShowAccountInfo = !m_aClients[ClientID].m_ShowAccountInfo;
			return true;
		}
		if(IsOptionWithSuffix(pDesc, COLLAPSE_HEADER_ACC_STATS))
		{
			m_aClients[ClientID].m_ShowAccountStats = !m_aClients[ClientID].m_ShowAccountStats;
			return true;
		}

		// Acc Misc
		if(IsOption(pDesc, ACC_MISC_NINJAJETPACK))
		{
			pPlayer->m_NinjaJetpack = !pPlayer->m_NinjaJetpack;
			return true;
		}

		// VIP
		if(IsOption(pDesc, ACC_RAINBOW))
		{
			if(pChr) 
				pChr->SetRainbow(!pPlayer->m_Rainbow);
			return true;
		}
		//if (IsOption(pDesc, ACC_BLOODY))
		//{
		//	if(pChr)
		//		pChr->SetBloody(!pPlayer->m_Bloody);
		//	return true;
		//}
		//if (IsOption(pDesc, ACC_ATOM))
		//{
		//	if(pChr) 
		//		pChr->SetAtom(!pPlayer->m_Atom);
		//	return true;
		//}
		if(IsOption(pDesc, ACC_TRAIL))
		{
			if(pChr) 
				pChr->SetTrail(!pPlayer->m_Trail);
			return true;
		}

		//if (IsOption(pDesc, ACC_RAINBOWHOOK))
		//{
		//	if(pChr) 
		//		pChr->SetHookPower();
		//	return true;
		//}
		if(IsOption(pDesc, ACC_ROTATINGBALL))
		{
			if(pChr) 
				pChr->SetRotatingBall(!pPlayer->m_RotatingBall);
			return true;
		}
		if(IsOption(pDesc, ACC_EPICCIRCLE))
		{
			if(pChr) 
				pChr->SetEpicCircle(!pPlayer->m_EpicCircle);
			return true;
		}
		if(IsOption(pDesc, ACC_LOVELY))
		{
			if(pChr) 
				pChr->SetLovely(!pPlayer->m_Lovely);
			return true;
		}
		//if (IsOption(pDesc, ACC_SPARKLE))
		//{
		//	if(pChr) 
		//		pChr->SetSparkle(!pPlayer->m_Sparkle);
		//	return true;
		//}
		if(IsOptionWithSuffix(pDesc, ACC_RAINBOWSPEED))
		{
			if(Reason != -1)
				pPlayer->m_RainbowSpeed = Reason;
			else
				GameServer()->SendChatTarget(ClientID, "Please specify the rainbow speed using the reason field");
			return true;
		}
	}
	else if(GetPage(ClientID) == PAGE_MISCELLANEOUS)
	{
		if(IsOption(pDesc, MISC_AFKSPECTATE))
		{
			pPlayer->SetSpecAfk(!pPlayer->m_SpecAfkEnabled);
			return true;
		}
		if (IsOption(pDesc, MISC_WEAPONINDICATOR))
		{
			pPlayer->SetWeaponIndicator(!pPlayer->m_WeaponIndicator);
			return true;
		}
		if (IsOption(pDesc, MISC_HIDEBROADCASTS))
		{
			pPlayer->SetHideBroadcasts(!pPlayer->m_HideBroadcasts);
			return true;
		}
	}

	return false;
}

int CVotingMenu::PrepareTempDescriptions(int ClientID)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	if (!pPlayer)
		return 0;

	m_NumCollapseEntries = 0;
	int NumOptions = 0;

	const int Page = GetPage(ClientID);
	bool DoAnnouncement = false;
	char aBuf[VOTE_DESC_LENGTH];
	{
		if(DoAnnouncement)
		{
			DoLineText(Page, &NumOptions, aBuf, BULLET_POINT);
		}
	}

	if (DoAnnouncement)
	{
		DoLineSeperator(Page, &NumOptions);
	}

	if(GetPage(ClientID) == PAGE_ACCOUNT)
	{
		DoPageAccount(ClientID, &NumOptions);
	}
	else if(GetPage(ClientID) == PAGE_MISCELLANEOUS)
	{
		DoPageMiscellaneous(ClientID, &NumOptions);
	}

	return NumOptions;
}

void CVotingMenu::DoPageAccount(int ClientID, int *pNumOptions)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	CCharacter *pChr = pPlayer->GetCharacter();
	const int Page = GetPage(ClientID);

	char aBuf[128];
	time_t tmp;
	//int AccID = pPlayer->GetAccID();
	//CGameContext::AccountInfo *pAccount = &GameServer()->m_Accounts[AccID];

	//if (AccID < ACC_START)
	//{
	//	DoLineText(Page, pNumOptions, "You're not logged in.");
	//	DoLineText(Page, pNumOptions, "Use '/login' to see more information about your account.");
	//	return;
	//}

	//bool ShowEuros = GameServer()->Config()->m_SvEuroMode || pAccount->m_Euros > 0;
	bool ShowEuros = 0;
	if(DoLineCollapse(Page, pNumOptions, COLLAPSE_HEADER_ACC_INFO, m_aClients[ClientID].m_ShowAccountInfo, 5 + (int)ShowEuros))
	{
		//str_format(aBuf, sizeof(aBuf), "Account Name: %s", "pAccount->m_Username");
		DoLineText(Page, pNumOptions, aBuf);

		if("pAccount->m_RegisterDate" != 0)
		{
			//tmp = pAccount->m_RegisterDate;
			tmp = 1;
			//str_format(aBuf, sizeof(aBuf), "Registered: %s"); // GameServer()->GetDate(tmp, false));
			DoLineText(Page, pNumOptions, aBuf);
		}

		if(ShowEuros)
		{
			//str_format(aBuf, sizeof(aBuf), "Euros: %.2f", "pAccount->m_Euros");
			DoLineText(Page, pNumOptions, aBuf);
		}

		//str_format(aBuf, sizeof(aBuf), "Contact: %s", "pAccount->m_aContact");
		DoLineText(Page, pNumOptions, aBuf);
		//str_format(aBuf, sizeof(aBuf), "E-Mail: %s", "pAccount->m_aEmail");
		DoLineText(Page, pNumOptions, aBuf);

		// This does not count to NumEntries anymore, s_NumCollapseEntries is 0 by now again. We wanna add a seperator only if this thing is opened
		DoLineSeperator(Page, pNumOptions);
	}
	/*
	if (DoLineCollapse(Page, pNumOptions, COLLAPSE_HEADER_ACC_STATS, m_aClients[ClientID].m_ShowAccountStats, 12))
	{
		str_format(aBuf, sizeof(aBuf), "Level [%d]", pAccount->m_Level);
		DoLineText(Page, pNumOptions, aBuf);
		if (pChr && pChr->m_aLineExp[0] != '\0')
		{
			DoLineText(Page, pNumOptions, pChr->m_aLineExp);
		}
		else
		{
			str_format(aBuf, sizeof(aBuf), "XP [%lld/%lld]", pAccount->m_XP, GameServer()->GetNeededXP(pAccount->m_Level));
			DoLineText(Page, pNumOptions, aBuf);
		}
		str_format(aBuf, sizeof(aBuf), "Bank [%lld]", pAccount->m_Money);
		DoLineText(Page, pNumOptions, aBuf);
		if (pChr && pChr->m_aLineMoney[0] != '\0')
		{
			DoLineText(Page, pNumOptions, pChr->m_aLineMoney);
		}
		else
		{
			str_format(aBuf, sizeof(aBuf), "Wallet [%lld]", pPlayer->GetWalletMoney());
			DoLineText(Page, pNumOptions, aBuf);
		}
		str_format(aBuf, sizeof(aBuf), "Police [%d]", pAccount->m_PoliceLevel);
		DoLineText(Page, pNumOptions, aBuf);

		DoLineTextSubheader(Page, pNumOptions, "Cᴏʟʟᴇᴄᴛᴀʙʟᴇs");
		str_format(aBuf, sizeof(aBuf), "Taser Battery [%d]", pAccount->m_TaserBattery);
		DoLineText(Page, pNumOptions, aBuf);
		str_format(aBuf, sizeof(aBuf), "Portal Battery [%d]", pAccount->m_PortalBattery);
		DoLineText(Page, pNumOptions, aBuf);

		DoLineTextSubheader(Page, pNumOptions, "Bʟᴏᴄᴋ");
		str_format(aBuf, sizeof(aBuf), "Points: %d", pAccount->m_BlockPoints);
		DoLineText(Page, pNumOptions, aBuf);
		str_format(aBuf, sizeof(aBuf), "Kills: %d", pAccount->m_Kills);
		DoLineText(Page, pNumOptions, aBuf);
		str_format(aBuf, sizeof(aBuf), "Deaths: %d", pAccount->m_Deaths);
		DoLineText(Page, pNumOptions, aBuf);
	}*/

	DoLineSeperator(Page, pNumOptions);
	DoLineTextSubheader(Page, pNumOptions, "Oᴘᴛɪᴏɴs");

	//if (pAccount->m_VIP)
	{
		DoLineSeperator(Page, pNumOptions);
		DoLineToggleOption(Page, pNumOptions, ACC_RAINBOW, pChr && pPlayer->m_Rainbow);
		//if (pAccount->m_VIP == VIP_PLUS)
		{
			// let's put it here, next to rainbow which is influenced by it, not somewhere close to rainbowname i guess
			DoLineValueOption(Page, pNumOptions, ACC_RAINBOWSPEED, pPlayer->m_RainbowSpeed, 20, BULLET_ARROW);
		}
		DoLineToggleOption(Page, pNumOptions, ACC_BLOODY, pChr && (pPlayer->m_Bloody || pPlayer->m_StrongBloody));
		DoLineToggleOption(Page, pNumOptions, ACC_ATOM, pChr && pPlayer->m_Atom);
		DoLineToggleOption(Page, pNumOptions, ACC_TRAIL, pChr && pPlayer->m_Trail);

		//if (pAccount->m_VIP == VIP_PLUS)
		{
			DoLineToggleOption(Page, pNumOptions, ACC_RAINBOWHOOK, pChr && pPlayer->m_HookPower == RAINBOW);
			DoLineToggleOption(Page, pNumOptions, ACC_ROTATINGBALL, pChr && pPlayer->m_RotatingBall);
			DoLineToggleOption(Page, pNumOptions, ACC_EPICCIRCLE, pChr && pPlayer->m_EpicCircle);
			DoLineToggleOption(Page, pNumOptions, ACC_LOVELY, pChr && pPlayer->m_Lovely);
			DoLineToggleOption(Page, pNumOptions, ACC_SPARKLE, pPlayer->m_Sparkle);
		}
	}
}

void CVotingMenu::DoPageMiscellaneous(int ClientID, int *pNumOptions)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	int Page = GetPage(ClientID);

	DoLineTextSubheader(Page, pNumOptions, "Oᴘᴛɪᴏɴs");
	DoLineToggleOption(Page, pNumOptions, MISC_AFKSPECTATE, pPlayer->m_SpecAfkEnabled);
	DoLineToggleOption(Page, pNumOptions, MISC_WEAPONINDICATOR, pPlayer->m_WeaponIndicator);
	DoLineToggleOption(Page, pNumOptions, MISC_HIDEBROADCASTS, pPlayer->m_HideBroadcasts);
}

void CVotingMenu::Tick()
{
	// Check once per second if we have to auto update
	if (Server()->Tick() % Server()->TickSpeed() == 0)
	{
		for (int i = 0; i < MAX_CLIENTS; i++)
		{
			CPlayer *pPlayer = GameServer()->m_apPlayers[i];
			if (!pPlayer)
				continue;
			if (m_aClients[i].m_NextVoteSendTick > Server()->Tick())
				continue;

			CVotingMenu::SClientVoteInfo::SPrevStats Stats;
			// 0.7 doesnt have playerflag_in_menu anymore, so we update them automatically every 3s if something changed, even when not in menu /shrug
			bool Update = (pPlayer->m_PlayerFlags&PLAYERFLAG_IN_MENU) || Server()->IsSixup(i);
			if (!Update || m_aClients[i].m_Page == PAGE_VOTES || !FillStats(i, &Stats))
				continue;

			// Design doesn't have to be checked, because on design loading finish it will resend the votes anyways so it will be up to date
			if (mem_comp(&Stats, &m_aClients[i].m_PrevStats, sizeof(Stats)) != 0)
			{
				SendPageVotes(i);
			}
		}
	}
	// Clear for next tick, we wanna keep wantedplayers up-to-date at all time
	m_vWantedPlayers.clear();
}

bool CVotingMenu::FillStats(int ClientID, CVotingMenu::SClientVoteInfo::SPrevStats *pStats)
{
	CPlayer *pPlayer = GameServer()->m_apPlayers[ClientID];
	CCharacter *pChr = GameServer()->GetPlayerChar(ClientID);
	if (!pPlayer || !pStats)
		return false;

	const int Page = GetPage(ClientID);

	//int AccID = pPlayer->GetAccID();
	//CGameContext::AccountInfo *pAccount = &GameServer()->m_Accounts[AccID];
	int Flags = 0;

	// Misc
	if (Page == PAGE_MISCELLANEOUS)
	{
		if(pPlayer->m_SpecAfkEnabled)
			Flags |= PREVFLAG_MISC_AFKSPEC;
		if(pPlayer->m_WeaponIndicator)
			Flags |= PREVFLAG_MISC_WEAPONINDICATOR;
		if(pPlayer->m_HideBroadcasts)
			Flags |= PREVFLAG_MISC_HIDEBROADCASTS;
	}
	else if(Page == PAGE_ACCOUNT)
	{
		// VIP
		//if (pAccount->m_VIP)
		{
			if((pChr && pPlayer->m_Rainbow))
				Flags |= PREVFLAG_ACC_RAINBOW;
			if(pChr && (pPlayer->m_Bloody || pPlayer->m_StrongBloody))
				Flags |= PREVFLAG_ACC_BLOODY;
			if(pChr && pPlayer->m_Atom)
				Flags |= PREVFLAG_ACC_ATOM;
			if(pChr && pPlayer->m_Trail)
				Flags |= PREVFLAG_ACC_TRAIL;
			//if(pChr && pPlayer->m_aSpreadWeapon[WEAPON_GUN])
				//Flags |= PREVFLAG_ACC_SPREADGUN;

			// VIP Plus
			//if(pAccount->m_VIP == VIP_PLUS)
			{
				if (pChr && pPlayer->m_HookPower == RAINBOW)
					Flags |= PREVFLAG_ACC_RAINBOWHOOK;
				if(pChr && pPlayer->m_RotatingBall)
					Flags |= PREVFLAG_ACC_ROTATINGBALL;
				if(pChr && pPlayer->m_EpicCircle)
					Flags |= PREVFLAG_ACC_EPICCIRCLE;
				if(pChr && pPlayer->m_Lovely)
					Flags |= PREVFLAG_ACC_LOVELY;
				if (pPlayer->m_Sparkle)
					Flags |= PREVFLAG_ACC_SPARKLE;
				pStats->m_RainbowSpeed = pPlayer->m_RainbowSpeed;
			}
		}
		
		// Acc Misc
		//if (pAccount->m_Ninjajetpack && pPlayer->m_NinjaJetpack)
		//	Flags |= PREVFLAG_ACC_NINJAJETPACK;

		//if (m_aClients[ClientID].m_ShowAccountStats)
		//{
		//	pStats->m_Acc.m_XP = pAccount->m_XP;
		//	pStats->m_Acc.m_Money = pAccount->m_Money;
		//	pStats->m_Acc.m_WalletMoney = pPlayer->GetWalletMoney();
		//	pStats->m_Acc.m_PoliceLevel = pAccount->m_PoliceLevel;
		//	pStats->m_Acc.m_TaserBattery = pAccount->m_TaserBattery;
		//	pStats->m_Acc.m_PortalBattery = pAccount->m_PortalBattery;
		//	pStats->m_Acc.m_Points = pAccount->m_BlockPoints;
		//	pStats->m_Acc.m_Kills = pAccount->m_Kills;
		//	pStats->m_Acc.m_Deaths = pAccount->m_Deaths;
		//	pStats->m_Acc.m_Euros = pAccount->m_Euros;
		//	str_copy(pStats->m_Acc.m_aContact, pAccount->m_aContact, sizeof(pStats->m_Acc.m_aContact));
		//}
	}

	pStats->m_Flags = Flags;
	return true;
}

void CVotingMenu::SendPageVotes(int ClientID, bool ResendVotesPage)
{
	if (!GameServer()->m_apPlayers[ClientID])
		return;

	int Seconds = Server()->IsSixup(ClientID) ? 3 : 1;
	m_aClients[ClientID].m_NextVoteSendTick = Server()->Tick() + Server()->TickSpeed() * Seconds;
	const int Page = GetPage(ClientID);
	if (Page == PAGE_VOTES)
	{
		// No need to resend votes when we login, we only want to update the other pages with new values
		if (ResendVotesPage)
		{
			CNetMsg_Sv_VoteClearOptions VoteClearOptionsMsg;
			Server()->SendPackMsg(&VoteClearOptionsMsg, MSGFLAG_VITAL, ClientID);

			// begin sending vote options
			GameServer()->m_apPlayers[ClientID]->m_SendVoteIndex = 0;
		}
		return;
	}

	CVotingMenu::SClientVoteInfo::SPrevStats Stats;
	if (FillStats(ClientID, &Stats))
	{
		m_aClients[ClientID].m_PrevStats = Stats;
	}

	CNetMsg_Sv_VoteClearOptions VoteClearOptionsMsg;
	Server()->SendPackMsg(&VoteClearOptionsMsg, MSGFLAG_VITAL, ClientID);

	const int NumVotesPage = PrepareTempDescriptions(ClientID);
	const int NumVotesToSend = NUM_OPTION_START_OFFSET + NumVotesPage;
	int TotalVotesSent = 0;
	CMsgPacker Msg(NETMSGTYPE_SV_VOTEOPTIONLISTADD);
	while (TotalVotesSent < NumVotesToSend)
	{
		const int VotesLeft = std::min(NumVotesToSend - TotalVotesSent, (int)MAX_VOTES_PER_PACKET);
		Msg.AddInt(VotesLeft);

		int CurIndex = 0;
		if (TotalVotesSent == 0)
		{
			OnProgressVoteOptions(ClientID, &Msg, &TotalVotesSent);
			CurIndex = TotalVotesSent;
		}

		while(CurIndex < VotesLeft)
		{
			Msg.AddString(m_aPages[Page].m_aaTempDesc[TotalVotesSent], VOTE_DESC_LENGTH);
			TotalVotesSent++;
			CurIndex++;
		}

		if (!Server()->IsSixup(ClientID))
		{
			while (CurIndex < MAX_VOTES_PER_PACKET)
			{
				Msg.AddString("", VOTE_DESC_LENGTH);
				CurIndex++;
			}
		}
		Server()->SendMsg(&Msg, MSGFLAG_VITAL, ClientID);
		// Reset for next potentional run
		Msg.Reset();
	}
}

void CVotingMenu::ApplyFlags(int ClientID, int Flags)
{
	if (Flags & VOTEFLAG_PAGE_VOTES)
		m_aClients[ClientID].m_Page = PAGE_VOTES;
	if (Flags & VOTEFLAG_PAGE_ACCOUNT)
		m_aClients[ClientID].m_Page = PAGE_ACCOUNT;
	if (Flags & VOTEFLAG_PAGE_MISCELLANEOUS)
		m_aClients[ClientID].m_Page = PAGE_MISCELLANEOUS;
	if (Flags & VOTEFLAG_SHOW_ACC_STATS)
		m_aClients[ClientID].m_ShowAccountStats = true;
}

int CVotingMenu::GetFlags(int ClientID)
{
	int Flags = 0;
	int Page = GetPage(ClientID);
	if (Page == PAGE_VOTES)
		Flags |= VOTEFLAG_PAGE_VOTES;
	else if (Page == PAGE_ACCOUNT)
		Flags |= VOTEFLAG_PAGE_ACCOUNT;
	else if (Page == PAGE_MISCELLANEOUS)
		Flags |= VOTEFLAG_PAGE_MISCELLANEOUS;
	if (m_aClients[ClientID].m_ShowAccountInfo)
		Flags |= VOTEFLAG_SHOW_ACC_INFO;
	if (m_aClients[ClientID].m_ShowAccountStats)
		Flags |= VOTEFLAG_SHOW_ACC_STATS;
	return Flags;
}

#define ADDLINE_IMPL(desc, prefix, collapseHeader) do \
{ \
	dbg_assert((NUM_OPTION_START_OFFSET + *pNumOptions < NUM_PAGE_MAX_VOTES), "added too many votes options per page"); \
	char aLine[VOTE_DESC_LENGTH]; \
	if ((prefix)[0]) \
	{ \
		str_format(aLine, sizeof(aLine), "%s %s", (prefix), (desc)); \
	} \
	else \
	{ \
		str_copy(aLine, (desc), sizeof(aLine)); \
	} \
	bool AddCollapseFooter = false; \
	if (m_NumCollapseEntries > 0 && !(collapseHeader)) \
	{ \
		m_NumCollapseEntries--; \
		if (m_NumCollapseEntries == 0) \
		{ \
			AddCollapseFooter = true; \
		} \
		char aTemp[VOTE_DESC_LENGTH]; \
		str_format(aTemp, sizeof(aTemp), "│ %s", aLine); \
		str_copy(aLine, aTemp, sizeof(aLine)); \
	} \
	str_copy(m_aPages[Page].m_aaTempDesc[NUM_OPTION_START_OFFSET + *pNumOptions], aLine, VOTE_DESC_LENGTH); \
	(*pNumOptions)++; \
	if (AddCollapseFooter) \
	{ \
		dbg_assert((NUM_OPTION_START_OFFSET + *pNumOptions < NUM_PAGE_MAX_VOTES), "added too many votes options per page"); \
		str_copy(m_aPages[Page].m_aaTempDesc[NUM_OPTION_START_OFFSET + *pNumOptions], "╰───────────────────────", VOTE_DESC_LENGTH); \
		(*pNumOptions)++; \
	} \
} while(0)

#define ADDLINE(desc) ADDLINE_IMPL(desc, "", false)
#define ADDLINE_PREFIX(desc, prefix) ADDLINE_IMPL(desc, prefix, false)
#define ADDLINE_COLLAPSE(desc) ADDLINE_IMPL(desc, "", true)

void CVotingMenu::DoLineToggleOption(int Page, int *pNumOptions, const char *pDescription, bool Value)
{
	ADDLINE_PREFIX(pDescription, Value ? "☒" : "☐");
}

void CVotingMenu::DoLineSeperator(int Page, int *pNumOptions)
{
	ADDLINE(PLACEHOLDER_DESC);
}

void CVotingMenu::DoLineTextSubheader(int Page, int *pNumOptions, const char *pDescription)
{
	char aBuf[VOTE_DESC_LENGTH];
	str_format(aBuf, sizeof(aBuf), "─── %s ───", pDescription);
	ADDLINE(aBuf);
}

void CVotingMenu::DoLineValueOption(int Page, int *pNumOptions, const char *pDescription, int Value, int Max, int BulletPoint)
{
	char aBuf[VOTE_DESC_LENGTH];
	if (Max == -1)
	{
		str_format(aBuf, sizeof(aBuf), "%s: %d", pDescription, Value);
	}
	else
	{
		str_format(aBuf, sizeof(aBuf), "%s: %d/%d", pDescription, Value, Max);
	}
	DoLineText(Page, pNumOptions, aBuf, BulletPoint);
}

void CVotingMenu::DoLineText(int Page, int *pNumOptions, const char *pDescription, int BulletPoint)
{
	const char *pPrefix = "";
	if (BulletPoint == BULLET_POINT) pPrefix = "• ";
	else if (BulletPoint == BULLET_ARROW) pPrefix = "⇨ ";
	ADDLINE_PREFIX(pDescription, pPrefix);
}

bool CVotingMenu::DoLineCollapse(int Page, int *pNumOptions, const char *pDescription, bool ShowContent, int NumEntries)
{
	char aBuf[VOTE_DESC_LENGTH];
	if (ShowContent)
	{
		m_NumCollapseEntries = NumEntries;
		str_format(aBuf, sizeof(aBuf), "╭─           %s     [‒]", pDescription);
	}
	else
	{
		str_format(aBuf, sizeof(aBuf), ">            %s     [+]", pDescription);
	}
	ADDLINE_COLLAPSE(aBuf);
	return ShowContent;
}

#undef ADDLINE
#undef ADDLINE_PREFIX
#undef ADDLINE_COLLAPSE
