//
// event.inc
//

#if defined EVENT_INC
    #endinput
#endif 
#define EVENT_INC 

#include <a_samp>
#include <YSI\y_hooks>

#if !defined EVENT_DATABASE 
    #define EVENT_DATABASE          "events.db"
#endif 

#if !defined EVENT_DATABASE_TABLE
    #define EVENT_DATABASE_TABLE    "events"
#endif 

#if !defined EVENT_FILE_DIRECTORY
    #define EVENT_FILE_DIRECTORY    "events/"
#endif 

#if !defined EVENT_MAX_CONCURRENT
    #define EVENT_MAX_CONCURRENT    (5)
#endif

#if !defined EVENT_MAX_NAME_LEN 
    #define EVENT_MAX_NAME_LEN      (30)
#endif

#if !defined EVENT_MAX_VERSION_LEN
    #define EVENT_MAX_VERSION_LEN   (6)
#endif

#if !defined EVENT_MAX_FILE_DIR_LEN
    #define EVENT_MAX_FILE_DIR_LEN  (40)
#endif

#if !defined EVENT_MAX_COMMAND_LEN
    #define EVENT_MAX_COMMAND_LEN   (20)
#endif

enum eEvent 
{
    eID,
    eName[EVENT_MAX_NAME_LEN],
    eAuthor[MAX_PLAYER_NAME + 1],
    eVersion[EVENT_MAX_VERSION_LEN],
    eFile[EVENT_MAX_FILE_DIR_LEN],
    eCommand[EVENT_MAX_COMMAND_LEN]
};

static 
    gPlayerEvent[MAX_PLAYERS],
    gEventData[EVENT_MAX_CONCURRENT][eEvent],
    DB: gDBHandle;

hook OnGameModeInit() 
{
    gDBHandle = db_open(EVENT_DATABASE);

    if(!IsValidDB(gDBHandle)) {
        printf("[ERROR - event.inc] Could not connect to the database '%s'", EVENT_DATABASE);
    } else {
        CreateDefaultTable();
    }
    return 1;
}

hook OnGameModeExit()
{
    if(IsValidDB(gDBHandle)) {
        db_close(gDBHandle);
    }
    return 1;
}

hook OnPlayerDisconnect(playerid, reason) {

    RemovePlayerFromEvent(playerid);
    return 1;
}

bool: IsPlayerInEvent(playerid) {
    if(gPlayerEvent[playerid] != 0) {
        return true;
    }
    return false;
}

hook OnPlayerSpawn(playerid) {

    if(IsPlayerInEvent(playerid)) {
        CallRemoteFunction("OnPlayerSpawnInEvent", "ii", playerid, gPlayerEvent[playerid]);
    }
    return 1;
}

hook OnPlayerDeath(playerid, killerid, reason) {

    if(IsPlayerInEvent(playerid)) {
        CallRemoteFunction("OnPlayerDeathInEvent", "iiii", playerid, killerid, reason, gPlayerEvent[playerid]);
    }
    return 1;
}

hook OnPlayerCommandText(playerid, cmdtext[]) {

    if(gPlayerEvent[playerid] != 0) {
        return 0;
    }

    for(new i = 0; i < EVENT_MAX_CONCURRENT; i++) {
        if(gEventData[i][eID] != 0) {

            if(!strcmp(cmdtext, gEventData[i][eCommand], true)) {
                PutPlayerInEvent(playerid, i);
                return 1;
            }
        }
    }
    return 0;
}

stock LoadEvent(id) {
    
    new 
        DBResult: result,
        query[128];

    format(query, sizeof(query), "SELECT * FROM `"EVENT_DATABASE_TABLE"` WHERE id = %i", id);
    result = db_query(gDBHandle, query);

    if(result) {
        if(!db_num_rows(result)) {
            printf("[ERROR - event.inc] Could not load mode id %i from database", id);
        } else {

            new index = GetOpenEventSlot();

            if(index == -1) {
                print("[ERROR - event.inc] Max number of modes loaded. Try increasing 'EVENT_MAX_CONCURRENT'."); 
            } else {
               gEventData[index][eID] = id;
               db_get_field_assoc(result, "name", gEventData[index][eName], EVENT_MAX_NAME_LEN);
               db_get_field_assoc(result, "author", gEventData[index][eAuthor], MAX_PLAYER_NAME + 1);
               db_get_field_assoc(result, "version", gEventData[index][eVersion], EVENT_MAX_VERSION_LEN);
               db_get_field_assoc(result, "file", gEventData[index][eFile], EVENT_MAX_FILE_DIR_LEN);
               db_get_field_assoc(result, "command", gEventData[index][eCommand], EVENT_MAX_COMMAND_LEN);

               db_free_result(result);

               query[0] = EOS;
               strcat(query, EVENT_FILE_DIRECTORY);
               strcat(query, gEventData[index][eFile]);
               format(query, sizeof(query), "loadfs %s", query);

               SendRconCommand(query);
               CallRemoteFunction("OnEventInit", "i", index);
               return 1;
           }
        }
        db_free_result(result);
    }
    return 0;
}

stock UnloadEvent(id) {

    if(gEventData[id][eID] == 0) {
        printf("[ERROR - event.inc] Cannot unload event id %i - event not loaded", id);
        return 0;
    } 

    new buffer[128];
    
    strcat(buffer, EVENT_FILE_DIRECTORY);
    strcat(buffer, gEventData[id][eFile]);
    format(buffer, sizeof(buffer), "unloadfs %s", buffer);

    gEventData[id][eID] = 0;
    gEventData[id][eName][0] = EOS;
    gEventData[id][eAuthor][0] = EOS;
    gEventData[id][eVersion][0] = EOS;
    gEventData[id][eFile][0] = EOS;
    gEventData[id][eCommand][0] = EOS;

    CallRemoteFunction("OnEventExit", "i", id);
    return 1;
}

static PutPlayerInEvent(playerid, eventid) {
    if(gEventData[eventid][eID] != 0) {
        gPlayerEvent[playerid] = eventid;
        CallRemoteFunction("OnPlayerJoinEvent", "ii", playerid, eventid);
        return 1;
    }
    return 0;
}

static GetOpenEventSlot() {
    for(new i = 0; i < EVENT_MAX_CONCURRENT; i++) {
        if(gEventData[i][eID] == 0) {
            return i;
        }
    }
    return 0;
}

static IsValidDB(DB: handle) {
    if(handle != DB:0) {
        return 1;
    }
    return 0;
}

static CreateDefaultTable() {
    if(IsValidDB(gDBHandle)) {
        db_query(gDBHandle, "CREATE TABLE IF NOT EXISTS `"EVENT_DATABASE_TABLE"` ( \
            `id` INTEGER PRIMARY KEY AUTOINCREMENT, \
            `name` TEXT, \
            `author` TEXT, \
            `version` TEXT, \
            `file` TEXT, \
            `command` TEXT \
        );");
    }
}

forward RemovePlayerFromEvent(playerid);
public RemovePlayerFromEvent(playerid) {
    if(IsPlayerInEvent(playerid)) {
        CallRemoteFunction("OnPlayerLeaveEvent", "ii", playerid, gPlayerEvent[playerid]);
        gPlayerEvent[playerid] = 0;
        return 1;
    }
    return 0;
}

forward RemoveAllPlayersFromEvent(eventid);
public RemoveAllPlayersFromEvent(eventid) {
    for(new i = 0; i < MAX_PLAYERS; i++) {
        if(gPlayerEvent[i] == eventid) {
            RemovePlayerFromEvent(i);
        }
    }
    return 1;
}