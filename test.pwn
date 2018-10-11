#include "event.inc"

main() {
    
}

public OnGameModeInit() {
    print("Test mode for event.inc");

    AddPlayerClass(0, 0.00, 0.00, 0.00, 0.00, 0, 0, 0, 0, 0, 0);
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

    if(!strcmp(cmdtext, "/utest1", true)) {
        UnloadEvent(1);
        return 1;
    }

    if(!strcmp(cmdtext, "/utest2", true)) {
        UnloadEvent(2);
        return 1;
    }
    return 0;
}