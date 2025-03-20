#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

MACRO_CONFIG_INT(SvSpeed, fs_speed, 1, 1, 4, CFGFLAG_SERVER | CFGFLAG_GAME, "Speed")
MACRO_CONFIG_STR(SvGameTypeName, fs_gametype_name, 32, "FoxNetwork", CFGFLAG_SERVER | CFGFLAG_GAME, "game type name")

MACRO_CONFIG_INT(SvSendMotd, fs_send_motd, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Show Motd on Enter")
MACRO_CONFIG_INT(SvSendWelcome, fs_send_welcome, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Send a Welcome broadcast to players who joined")
MACRO_CONFIG_STR(SvWelcomeMessage, fs_welcome_message, 256, "< Welcome to the Server %s >", CFGFLAG_SERVER | CFGFLAG_GAME, "Welcome Message")

MACRO_CONFIG_INT(SvInstantCoreUpdate, fs_instant_core_update, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Send a Welcome broadcast to players who joined")
MACRO_CONFIG_INT(SvAbilityIndicator, fs_ability_indicator, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Enable Indicators for every player")
MACRO_CONFIG_INT(SvCustomAbilities, fs_custom_abilities, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "you can set a players a bility using set_ability, by using f3/f4 you can use them (vote yes/no)")
MACRO_CONFIG_INT(SvResetAbilityOnKill, fs_reset_ability_on_kill, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Reset Players Ability if they kill")
MACRO_CONFIG_INT(SvNoAuthCooldown, fs_no_auth_cooldown, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "whether theres a cooldown for abilities on authed players")
MACRO_CONFIG_INT(SvAbilityCooldown, fs_ability_cooldown, 1, 1, 100, CFGFLAG_SERVER | CFGFLAG_GAME, "Cooldown for Abilities")

MACRO_CONFIG_INT(SvAntiAdBot, fs_anti_ad_bot, 2, 0, 2, CFGFLAG_SERVER | CFGFLAG_GAME, "Anti Ad Bot, 1 Normal Detection, 2 tries to not ban people if they ping someone with a fancy name")

MACRO_CONFIG_INT(SvSoloOnSpawn, fs_solo_on_spawn, 1, -1, 11, CFGFLAG_SERVER | CFGFLAG_GAME, "Solo the Player for x amount of seconds on spawn")
MACRO_CONFIG_INT(SvSoloOnSpawnSec, fs_solo_on_spawn_seconds, 5, 0, 100, CFGFLAG_SERVER | CFGFLAG_GAME, "Solo the Player for x amount of seconds on spawn")

MACRO_CONFIG_INT(SvForcePauseAfk, fs_force_pause_afk, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "To Prevent AFK blocking, put players into spectator mode if afk")
MACRO_CONFIG_INT(SvBanSyncing, fs_ban_syncing, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Whether to Sync bans every fs_ban_syncing_delay mins across servers")
MACRO_CONFIG_INT(SvBanSyncingDelay, fs_ban_syncing_delay, 15, 1, 500, CFGFLAG_SERVER | CFGFLAG_GAME, "How long the server waits between syncs")

MACRO_CONFIG_INT(SvCommandOutput, fs_command_output, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "when using a command on someone, it will show them what happened")

MACRO_CONFIG_INT(SvPingEveryone, fs_allow_ping_everyone, 1, 0, 2, CFGFLAG_SERVER | CFGFLAG_GAME, "When turned on, it will ping everyone if any moderator or admin says \n@everyone\" | 1 = only mods/admins | 2 = anyone")

MACRO_CONFIG_INT(SvAutoHammer, fs_auto_hammer, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "auto enables hammer on kill")
MACRO_CONFIG_INT(SvAutoGun, fs_auto_gun, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "auto enables gun on kill")
MACRO_CONFIG_INT(SvAutoGrenade, fs_auto_grenade, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "auto enables grenade on kill")
MACRO_CONFIG_INT(SvAutoLaser, fs_auto_laser, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "auto enables laser on kill")
MACRO_CONFIG_INT(SvAutoShotgun, fs_auto_shotgun, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "auto enables shotgun on kill")

MACRO_CONFIG_INT(SvAutoExplGun, fs_explgun, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "enables the explosion gun for everyone")

MACRO_CONFIG_INT(SvConfettiGun, fs_confettigun, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "when shooting it displays confetti")

MACRO_CONFIG_INT(SvDisableFreeze, fs_disable_freeze, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "enables the 'better version' of the explosion gun")
MACRO_CONFIG_INT(SvFakeGrenade, fs_fake_grenade, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Grenade doesnt explode")
MACRO_CONFIG_INT(SvWhisperLog, fs_whisper_log, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Log Whispers to Remote Console (Rcon)")