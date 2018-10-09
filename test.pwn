#include "event.inc"

main() {
    
}

public OnGameModeInit() {
    print("Test mode for event.inc");
    return 1;
}

public OnPlayerCommandText(playerid, cmdtext[]) {
    if(!strcmp(cmdtext, "/test1", true)) {
        LoadEvent(1);
        return 1;
    }

    if(!strcmp(cmdtext, "/test2", true)) {
        LoadEvent(2);
        return 1;
    }
    return 0;
}