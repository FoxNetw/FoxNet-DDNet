FoxNet, a simple DDNet Server improvement

From the Same Creator as [Aiodob Client](https://github.com/qxdFox/Aiodob-Client-DDNet)

To Download this extension you can head to the [Actions Tab](https://github.com/qxdFoxs/FoxNet-DDNet/actions/workflows/build.yml), go to the newest build if it succeded and download the artifact for your system. You need to be logged in for this

#
### Current Features

- Allowing Ports past 8310 -> now up to 8360, meaning you can start around ~50 servers without needing to change any configurations
- Solo on Spawn -> As the name suggests when you respawn you will be put into solo and have a shield floating above you.
- quicker rcon command loadtime
- Welcome Text
- quick and simple gametype renaming using the fs_gametype_name command and then reloading
- Allow using alot more commands on other player (e.g. grenade (id), infinite_jump (id), etc.)
- Edit weapons you spawn with using commands that start with fs_auto_ and then the weapon name
- explosion gun -> when you shoot it acts like a grenade and explodes if it hits something -> can also be automatically activated using fs_explgun 1
- automatic ban saving/executing -> every 30 minutes the server will execute "Bans.cfg" and after save every ban to "Bans.cfg", meaning servers sync bans every 30 mins
- disabling freeze and Rocket launcher grenade (fs_disable_freeze 1, fs_fake_grenade 1)
- Slow Gores -> you can use the fs_speed to change the "speed" at which the game runs (fs_speed 1 - normal speed | fs_speed 2 - 2x slow | fs_speed 3 - 4x slow | fs_speed 4 - 8x slow)
- telekinesis weapon -> using the command telekinesis you can give yourself (or the id) the telekinesis weapon, using the scrollwheel you can swtich to it (its just a second gun) -> pressing on a player will move them to where your cursor is located
