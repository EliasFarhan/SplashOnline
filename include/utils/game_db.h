#ifndef SPLASHONLINE_GAME_DB_H_
#define SPLASHONLINE_GAME_DB_H_


#ifdef ENABLE_DESYNC_DEBUG

#include "engine/system.h"
#include "engine/input_manager.h"
#include "rollback/rollback_system.h"


namespace splash
{

void OpenDatabase(int playerNumber);

void CloseDatabase();

void AddConfirmFrame(const Checksum<7>& checksum, int confirmFrame);
void AddLocalInput(int currentFrame, PlayerInput playerInput);
void AddRemoteInput(int currentFrame, int remoteFrame, int playerNumber, PlayerInput playerInput);

};
#endif

#endif //SPLASHONLINE_GAME_DB_H_