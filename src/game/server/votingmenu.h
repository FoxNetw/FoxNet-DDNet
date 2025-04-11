// made by fokkonaut
// edited by qxdFox
#ifndef GAME_SERVER_VOTING_MENU_H
#define GAME_SERVER_VOTING_MENU_H

#include <engine/shared/protocol.h>
#include <game/generated/protocol.h>
#include <game/voting.h>
#include <base/math.h>
#include <vector>
#include <engine/message.h>
#include <base/system.h>

class CGameContext;
class IServer;

class CVotingMenu
{
	enum AccFlags
	{
		VOTEFLAG_PAGE_VOTES = 1 << 0,
		VOTEFLAG_PAGE_ACCOUNT = 1 << 1,
		VOTEFLAG_PAGE_MISCELLANEOUS = 1 << 2,
		VOTEFLAG_SHOW_ACC_INFO = 1 << 3,
		VOTEFLAG_SHOW_ACC_STATS = 1 << 4,
	};

	enum EVotingPage
	{
		PAGE_VOTES = 0,
		PAGE_ACCOUNT,
		PAGE_MISCELLANEOUS,
		NUM_PAGES,

		NUM_PAGE_SEPERATORS = 2,
		NUM_OPTION_START_OFFSET = NUM_PAGES + NUM_PAGE_SEPERATORS,
		// When more than 47 the client will bug the scrollheight to somewhere in the middle or something when updating. let's just
		//NUM_PAGE_MAX_VOTES = 47,
		NUM_PAGE_MAX_VOTES = 128 - NUM_OPTION_START_OFFSET,
		NUM_WANTEDS_PER_PAGE = 4,
	};

	enum
	{
		// Misc
		PREVFLAG_MISC_AFKSPEC = 1 << 0,
		PREVFLAG_MISC_WEAPONINDICATOR = 1 << 1,
		PREVFLAG_MISC_HIDEBROADCASTS = 1 << 2,
		// VIP
		PREVFLAG_ACC_RAINBOW = 1 << 6,
		PREVFLAG_ACC_BLOODY = 1 << 7,
		PREVFLAG_ACC_ATOM = 1 << 8,
		PREVFLAG_ACC_TRAIL = 1 << 9,
		//PREVFLAG_ACC_SPREADGUN = 1 << 10,
		// VIP Plus
		PREVFLAG_ACC_RAINBOWHOOK = 1 << 10,
		PREVFLAG_ACC_ROTATINGBALL = 1 << 11,
		PREVFLAG_ACC_EPICCIRCLE = 1 << 12,
		PREVFLAG_ACC_LOVELY = 1 << 13,
		PREVFLAG_ACC_RAINBOWNAME = 1 << 14,
		PREVFLAG_ACC_SPARKLE = 1 << 15,
		// Acc Misc
		PREVFLAG_ACC_NINJAJETPACK = 1 << 16,
	};

	CGameContext *m_pGameServer;
	CGameContext *GameServer() const;
	IServer *Server() const;
	bool m_Initialized;
	std::vector<int> m_vWantedPlayers;

	struct SClientVoteInfo
	{
		int m_Page;
		int64_t m_LastVoteMsg;
		int64_t m_NextVoteSendTick;
		// Collapses
		bool m_ShowAccountInfo;
		bool m_ShowAccountStats;

		struct SPrevStats
		{
			int m_Flags = 0;
			int m_RainbowSpeed = 0;
			int64_t m_DestroyEndTick = 0;
			struct
			{
				int64_t m_XP = 0;
				int64_t m_Money = 0;
				int64_t m_WalletMoney = 0;
				int m_PoliceLevel = 0;
				int m_TaserBattery = 0;
				int m_PortalBattery = 0;
				int m_Points = 0;
				int m_Kills = 0;
				int m_Deaths = 0;
				int m_Euros = 0;
				char m_aContact[128] = { 0 };
			} m_Acc;
		};
		SPrevStats m_PrevStats;
	} m_aClients[MAX_CLIENTS];
	bool FillStats(int ClientID, CVotingMenu::SClientVoteInfo::SPrevStats *pStats);
	int GetPage(int ClientID) { return m_aClients[ClientID].m_Page; }

	struct SPageInfo
	{
		char m_aName[64];
		char m_aaTempDesc[NUM_PAGE_MAX_VOTES][VOTE_DESC_LENGTH];
	} m_aPages[NUM_PAGES];

	int GetNumWantedPages() { float f = (float)m_vWantedPlayers.size() / (float)NUM_WANTEDS_PER_PAGE; return round_to_int(f + 0.5f); }
	
	bool SetPage(int ClientID, int Page);
	const char *GetPageDescription(int ClientID, int Page);

	bool IsOptionWithSuffix(const char *pDesc, const char *pWantedOption) { return str_startswith(pDesc, pWantedOption) != 0; }
	bool IsOption(const char *pDesc, const char *pWantedOption) { return !str_comp(pDesc, pWantedOption); }
	bool OnMessageSuccess(int ClientID, const char *pDesc, const char *pReason);

	int PrepareTempDescriptions(int ClientID);
	void DoPageAccount(int ClientID, int *pNumOptions);
	void DoPageMiscellaneous(int ClientID, int *pNumOptions);
	void DoLineToggleOption(int Page, int *pNumOptions, const char *pDescription, bool Value);
	void DoLineValueOption(int Page, int *pNumOptions, const char *pDescription, int Value, int Max = -1, int BulletPoint = BULLET_NONE);
	void DoLineSeperator(int Page, int* pNumOptions);
	enum
	{
		BULLET_NONE,
		BULLET_POINT,
		BULLET_ARROW,
	};
	void DoLineText(int Page, int *pNumOptions, const char *pDescription, int BulletPoint = BULLET_NONE);
	void DoLineTextSubheader(int Page, int *pNumOptions, const char *pDescription);
	bool DoLineCollapse(int Page, int *pNumOptions, const char *pDescription, bool ShowContent, int NumEntries);
	int m_NumCollapseEntries;

public:
	CVotingMenu()
	{

		m_Initialized = false;
	}
	void Init(CGameContext *pGameServer);
	void Tick();
	void AddPlaceholderVotes();

	bool OnMessage(int ClientID, const CNetMsg_Cl_CallVote *pMsg);
	void OnProgressVoteOptions(int ClientID, CMsgPacker *pPacker, int *pCurIndex, CVoteOptionServer **ppCurrent = nullptr);
	void SendPageVotes(int ClientID, bool ResendVotesPage = true);

	void Reset(int ClientID);
	void InitPlayer(int ClientID);

	void SetWantedPlayer(int ClientID) { m_vWantedPlayers.push_back(ClientID); }
	void ApplyFlags(int ClientID, int Flags);
	int GetFlags(int ClientID);

	enum
	{
		MAX_VOTES_PER_PACKET = 15,
	};
};
#endif //GAME_SERVER_VOTING_MENU_H
