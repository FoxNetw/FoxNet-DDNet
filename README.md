FoxNetwork, a simple open source DDNet Server improvement

From the Same Creator as [Aiodob Client](https://github.com/qxdFox/Aiodob-Client-DDNet)

To Download this extension you can head to the [Actions Tab](https://github.com/qxdFoxs/FoxNet-DDNet/actions/workflows/build.yml), go to the newest build if it succeded and download the artifact for your system. You need to be logged in for this

If you wish to build it yourself you can read the instructions in the [DDNet Readme](https://github.com/ddnet/ddnet)

#
### Current Features

- Allowing Ports past 8310 -> now up to 8360, meaning you can start around ~50 servers without needing to change any configurations
- Solo on Spawn -> As the name suggests, when you spawn you will be put into solo and have a shield(changeable with fs_solo_on_spawn | 0 = off) floating above you
- quicker rcon commands loadtime
- Welcome Text
- quick and simple gametype renaming using the fs_gametype_name config
- Allow using alot more commands on other player (e.g. grenade (id), infinite_jump (id), etc.)
- Edit weapons you spawn with using commands that start with fs_auto_ and then the weapon name
- explosion gun -> when you shoot it acts like a grenade and explodes if it hits something -> can also be automatically activated using fs_explgun 1
- automatic ban saving/executing -> every 30 minutes the server will execute "Bans.cfg" and after save every ban to "Bans.cfg", meaning servers sync bans every 30 mins
- disabling freeze and Rocket launcher grenade (fs_disable_freeze 1, fs_fake_grenade 1)
- Slow Gores -> you can use the fs_speed config to change the "speed" at which the game runs (fs_speed 1 - normal speed | fs_speed 2 - 2x slow | fs_speed 3 - 4x slow | fs_speed 4 - 8x slow)
- telekinesis weapon from [F-DDRace](https://github.com/fokkonaut/F-DDrace) -> using the command telekinesis you can give yourself (or the id) the telekinesis weapon, using the scrollwheel you can swtich to it (its just a second gun)
- telekinesis immunity -> no one can pull the player who has it enbaled
- Moving Freeze and polygon Freeze (basically any shape) from [this ddnet pull](https://github.com/ddnet/ddnet/pull/9469)
- @everyone Ping (fs_allow_ping_everyone, default is on) -> if a mod or admin sends a message containing "@everyone" it sends a message to every player and replaces "@everyone" with their Username
- random_map (kind of) works now, if you have sub directories for your maps it might try to choose the subdirectory as a map, so i recommend recoding it if you do
- Rainbow command
- Invisible command
- commands to change other peoples info (Name, Clan, Skin, etc..)
- Heart Gun from (F-DDRace)
- Weapon Indicator for custom weapons (F-DDRace)
- Automatic Bans if a player send a flagged message (fs_anti_ad_bot), using disallow_string you can add strings that the server will search for in message sent by players,
  if it finds two strings in one message it will ban the player for a few hours -> it also bans krx whisper messages automatically by default