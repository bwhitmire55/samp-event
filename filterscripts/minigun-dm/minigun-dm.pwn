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

public OnEventInit() {

    SendClientMessageToAll(0x00FF00FF, "** Event Minigun Deathmatch has begun! Use /minigun to join!");
    return 1;
}

public OnEventExit() {
    
    SendClientMessageToAll(0x00FF00FF, "** Event Minigun Deathmatch has ended!");
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
    return 1;
}

SetUpPlayer(playerid) {
    new idx = random(sizeof(gSpawnPoints));
    SetPlayerPos(playerid, gSpawnPoints[idx][0], gSpawnPoints[idx][1], gSpawnPoints[idx][2]);
    SetPlayerFacingAngle(playerid, gSpawnPoints[idx][3]);
    GivePlayerWeapon(playerid, 38, 5000);
}