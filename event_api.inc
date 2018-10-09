//
// event_api.inc
//

forward OnEventInit();
forward OnEventExit();
forward OnPlayerJoinEvent(playerid);
forward OnPlayerLeaveEvent(playerid);
forward OnPlayerSpawnInEvent(playerid);

static gEventID = 0;

forward API_OnEventInit(id);
public API_OnEventInit(id) {
    if(gEventID == 0) {
        gEventID = id;
        CallLocalFunction("OnEventInit", "");
    }
    return 1;
}

forward API_OnEventExit(id);
public API_OnEventExit(id) {
    if(id == gEventID) {
        CallLocalFunction("OnEventExit", "");
    }
    return 1;
}

forward API_OnPlayerJoinEvent(playerid, id);
public API_OnPlayerJoinEvent(playerid, id) {
    if(id == gEventID) {
        CallLocalFunction("OnPlayerJoinEvent", "i", playerid);
    }
    return 1;
}

forward API_OnPlayerLeaveEvent(playerid, id);
public API_OnPlayerLeaveEvent(playerid, id) {
    if(id == gEventID) {
        CallLocalFunction("OnPlayerLeaveEvent", "i", playerid);
    }
    return 1;
}

forward API_OnPlayerSpawnInEvent(playerid, id);
public API_OnPlayerSpawnInEvent(playerid, id) {
    if(id == gEventID) {
        CallLocalFunction("OnPlayerSpawnInEvent", "i", playerid);
    }
    return 1;
}

stock GetName() {
    new buffer[60];
    CallRemoteFunction("GetEventName", "iai", gEventID, buffer, sizeof(buffer));
    return buffer;
}

stock GetCommand() {
    return CallRemoteFunction("GetEventCommand", "i", gEventID);
}

stock RemovePlayer(playerid) {
    CallRemoteFunction("RemovePlayerFromEvent", "i", playerid);
}