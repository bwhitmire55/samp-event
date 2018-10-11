# samp-event

[![sampctl](https://shields.southcla.ws/badge/sampctl-samp--event-2f2f2f.svg?style=for-the-badge)](https://github.com/bwhitmire55/samp-event)

samp-event is a dynamic event system which allows events (coded as filterscripts) to be loaded and unloaded on demand.

samp-event uses SA:MP native SQLite database system for storage and Y_Less' YSI hooks library for callback hooking.

## Installation

Simply install to your project:

```bash
sampctl package install bwhitmire55/samp-event
```

Include in your code and begin using the library:

```pawn
#include <samp-event>
```

## Functions

The core of the script only includes two functions to call locally from the gamemode

```pawn
/*
PARAMS:
id - The ID of the event (in the database) to be loaded

RETURNS:
1 on success, otherwise 0
*/
stock LoadEvent(id);
```

```pawn
/*
PARAMS:
id - The ID of the event (in the database) to be unloaded

RETURNS:
1 on success, otherwise 0
*/
stock UnloadEvent(id);
```

## Events

Events are generally defined as minigames within a server. With this event system we can make these events as simple or complicated as warranted as they are all loaded individually as filterscripts. Maybe we want a simple minigun deathmatch, or a quick race, or a more complicated PUGB side event. These can all be scripted as standalone filterscripts and loaded via LoadEvent().

## Usage

#### Setting up the database

Before we do anything, we'll want to set-up rows in our database about all the events we'll have. For this example, we'll have two deathmatch scripts; a minigun deathmatch and a sniper deathmatch. Our table structure is as such:
```pawn
`id` INTEGER PRIMARY KEY AUTOINCREMENT      -> The unique ID of the event (passed to LoadEvent())
`name` TEXT                                 -> The name of the event (i.e. "Minigun Deathmatch")
`author` TEXT                               -> The author of the event
`version` TEXT                              -> The version of the event (1.0)
`file` TEXT                                 -> The filterscript file of the event (excluding extension)
`command` TEXT                              -> The command player's use to join the event (/minigun)
```
Let's add our information to the database via a simple query through a database editor
```sql
INSERT INTO `table` (name, author, version, file, command) VALUES ('Minigun Deathmatch', 'nG Inverse', '1.0', 'minigun-dm', '/minigun')
INSERT INTO `table` (name, author, version, file, command) VALUES ('Sniper Deathmatch', 'nG Inverse', '1.0', 'sniper-dm', '/sniper')
```
Note that the file does not include a directory or an extension. These are both added to the file within the script prior to loading. Also note the command includes the '/'. This was out of pure laziness.

#### Creating the event scripts

Now we need to create our event scripts. These are coded simply as filterscripts. The plus side of this is we have another file (event_api.inc) which gives us access to a few new callbacks and functions to use from within our event scripts. A short list:
```pawn
// Callbacks
forward OnEventInit();
forward OnEventExit();
forward OnPlayerJoinEvent(playerid);
forward OnPlayerLeaveEvent(playerid);
forward OnPlayerSpawnInEvent(playerid);
forward OnPlayerDeathInEvent(playerid, killerid, reason);

// Functions
EndEvent();
RemovePlayer(playerid);
RemoveAllPlayers();
```
These are all fairly self explanatory, but it's worth mentioning that these callbacks will only be called from within this event script. So, if you have two events loaded (minigun and sniper) and a player joins the first event (minigun), OnPlayerJoinEvent will only be called from the first (minigun) script.
```
You also have use to any other functions and callbacks included in the file. Just note that if you use 'OnPlayerRequestClass' in the event script, it will be called for EVERY player, not just the ones currently in the event. Only the custom callbacks above will be called by only players within the event.

Below is a fully working Minigun Deathmatch event:
```pawn
//
// minigun-dm.pwn
//

#include <a_samp>
#include <event_api>

static const Float: gSpawnPoints[][4] = 
{
	{ 2552.7036,2813.3635,27.8203,180.5949 },
	{ 2610.1797,2820.7148,27.8203,173.7014 },
	{ 2643.6199,2781.2141,23.8222,92.8372 },
	{ 2595.3230,2769.1152,23.8222,0.7396 },
	{ 2600.1411,2717.1921,25.8222,359.1730 },
	{ 2644.3525,2698.4055,25.8222,10.1399 },
	{ 2632.2617,2708.5701,36.2673,69.0238 },
	{ 2648.4844,2778.9143,19.3222,189.9482 },
	{ 2654.0681,2720.1941,19.3222,86.8605 },
	{ 2670.7922,2796.8652,17.6896,95.0074 },
	{ 2670.1763,2759.5867,14.2559,91.4160 }, 
	{ 2648.9268,2732.3545,10.8203,317.1397 },
	{ 2668.6140,2731.2058,14.2559,0.7659 },
	{ 2639.8906,2817.8516,38.3222,181.1194 },
	{ 2700.6941,2818.0681,38.3222,111.8721 },
	{ 2657.1606,2662.7925,37.9877,9.8193 },
	{ 2571.6704,2662.6423,37.9129,318.0666 }
};

new gPlayerDeaths[MAX_PLAYERS];
new gPlayerKills[MAX_PLAYERS];

public OnEventInit() {

    SendClientMessageToAll(0x00FF00FF, "** Event Minigun Deathmatch has begun! Use /minigun to join!");
    return 1;
}

public OnEventExit() {
    
    SendClientMessageToAll(0x00FF00FF, "** Event Minigun Deathmatch has ended!");
    RemoveAllPlayers();
    return 1;
}

public OnPlayerJoinEvent(playerid) {
    SendClientMessage(playerid, 0x00FF00FF, "Welcome! Your objective is to kill every other player with your minigun!");
    // remove their current weapons
    ResetPlayerWeapons(playerid);
    SetUpPlayer(playerid);
    return 1;
}

public OnPlayerLeaveEvent(playerid) {
    // remove their minigun and respawn them
    ResetPlayerWeapons(playerid);
    SpawnPlayer(playerid);
    gPlayerDeaths[playerid] = 0;
    gPlayerKills[playerid] = 0;
    return 1;
}

public OnPlayerSpawnInEvent(playerid) {
    SetUpPlayer(playerid);
    return 1;
}

public OnPlayerDeathInEvent(playerid, killerid, reason) {
    // for example sake, remove the player after 5 deaths
    gPlayerDeaths[playerid]++;

    if(gPlayerDeaths[playerid] == 5) {
        RemovePlayer(playerid);
    }

    // end the event after someone gets 10 kills
    if(IsPlayerConnected(killerid)) {
        gPlayerKills[killerid]++;

        if(gPlayerKills[killerid] == 10) {
            
            new string[128], name[MAX_PLAYER_NAME + 1];
            GetPlayerName(killerid, name, sizeof(name));
            format(string, sizeof(string), "** %s has won the event with 10 kills!", name);
            SendClientMessageToAll(0x00FF00FF, string);

            EndEvent();
        }
    }
    return 1;
}

SetUpPlayer(playerid) {
    new idx = random(sizeof(gSpawnPoints));
    SetPlayerPos(playerid, gSpawnPoints[idx][0], gSpawnPoints[idx][1], gSpawnPoints[idx][2]);
    SetPlayerFacingAngle(playerid, gSpawnPoints[idx][3]);
    GivePlayerWeapon(playerid, 38, 5000);
}
```

#### Loading our events

Now let's just have a way for our events to be loaded
```pawn
public OnPlayerCommandText(playerid, cmdtext[]) {
    if(!strcmp(cmdtext, "/leminigun", true)) {
        LoadEvent(1);
        return 1;
    }
    if(!strcmp(cmdtext, "/lesniper", true)) {
        LoadEvent(2);
        return 1;
    }
}
```

And you are done! Players can use the commands from within the database to join an event once it has been loaded. They may also leave the event with the predefined command EVENT_LEAVE_COMMAND (default /leave).

## Macros

All macros are as followed:

```pawn
// The database file
#define EVENT_DATABASE          "mydatabase.db"
```

```pawn
// The database table to store the events
#define EVENT_DATABASE_TABLE    "mydatabasetable"
```

```pawn
// The sub-directory within 'filterscripts' folder where events are stored
#define EVENT_FILE_DIRECTORY    "events/"
```

```pawn
// The command for player's to leave events
#define EVENT_LEAVE_COMMAND     "/leave"
```

```pawn
// The number of events that can be loaded concurrently
#define EVENT_MAX_CONCURRENT    (5)
```

```pawn
// Maximum length of an event name
#define EVENT_MAX_NAME_LEN      (30)
```

```pawn
// Maximum length of the version text
#define EVENT_MAX_VERSION_LEN   (6)
```

```pawn
// Maximum length of the file directory
#define EVENT_MAX_FILE_DIR_LEN  (40)
```

```pawn
// Maximum length of the event commands
#define EVENT_MAX_COMMAND_LEN   (20)
```

All of these can be redefined to suite your script
```pawn
#define EVENT_DATABASE          "COD-DB.db"
#define EVENT_DATABASE_TABLE    "events"
#define EVENT_LEAVE_COMMAND     "/eleave"
#define EVENT_MAX_CONCURRENT    (20)
#include <event>
```

## Testing

To test, simply run the package:

```bash
sampctl package run
```
