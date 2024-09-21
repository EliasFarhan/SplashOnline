#ifndef SPLASHONLINE_GAME_DB_H_
#define SPLASHONLINE_GAME_DB_H_


#ifdef ENABLE_DESYNC_DEBUG

#include "engine/system.h"
#include "rollback/rollback_system.h"




namespace splash
{

void OpenDatabase(int playerNumber);

void CloseDatabase();

void AddConfirmFrame(const Checksum<7>& checksum, int confirmFrame);
};
#endif

#endif //SPLASHONLINE_GAME_DB_H_