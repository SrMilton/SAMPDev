#pragma once

#include "SAMPFUNCS_API.h"
#include "game_api.h"

extern SAMPFUNCS *SF;

void click();
void exitcar();
void showhidediag();
bool IsDriving(int PlayerID);
void executar();
void PutActorInCar(int vehicle_id, int seat_id);
void CALLBACK OnDialogResponse(int dialogId, int buttonId, int listItem, const char* input);