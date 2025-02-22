#ifndef MACRO_CONFIG_INT
#error "The config macros must be defined"
#define MACRO_CONFIG_INT(Name, ScriptName, Def, Min, Max, Save, Desc) ;
#define MACRO_CONFIG_COL(Name, ScriptName, Def, Save, Desc) ;
#define MACRO_CONFIG_STR(Name, ScriptName, Len, Def, Save, Desc) ;
#endif

MACRO_CONFIG_INT(SvSpeed, fs_speed, 1, 1, 4, CFGFLAG_SERVER | CFGFLAG_GAME, "Speed")
MACRO_CONFIG_STR(SvGameTypeName, fs_gametype_name, 64, "FoxNetwork", CFGFLAG_SERVER | CFGFLAG_GAME, "game type name")

MACRO_CONFIG_INT(SvSendMotd, fs_send_motd, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Show Motd on Enter")

MACRO_CONFIG_INT(SvSoloOnSpawn, fs_solo_on_spawn, 1, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "Solo the Player for x amount of seconds on spawn")
MACRO_CONFIG_INT(SvSoloOnSpawnSec, fs_solo_on_spawn_seconds, 5, 0, 20, CFGFLAG_SERVER | CFGFLAG_GAME, "Solo the Player for x amount of seconds on spawn")

MACRO_CONFIG_INT(SvCommandOutput, fs_command_output, 0, 0, 1, CFGFLAG_SERVER | CFGFLAG_GAME, "when using a command on someone, it will show them what happened")

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