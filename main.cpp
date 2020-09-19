#include <Windows.h>
#include <iostream>
#include <shlobj.h>
#include "main.h"
#include <fstream>
#include <sstream>
#include <math.h>
using namespace std;

SAMPFUNCS* SF = new SAMPFUNCS();

stFontInfo* pFont;
int screen[2];
bool textd = false;
bool holdtextd = false;
int result = 2047;
POINT cursor;
float x[2049], y[2049], xf[2049], yf[2049];
DWORD tick = 0;

bool CALLBACK Present(CONST RECT* pSourceRect, CONST RECT* pDestRect, HWND hDestWindowOverride, CONST RGNDATA* pDirtyRegion)
{
	if (SUCCEEDED(SF->getRender()->BeginRender()))
	{
		if (!SF->getGame()->isGTAMenuActive())
		{

			/*char procID[10];
			sprintf(procID, "%d", GetTickCount());
			pFont->Print(procID, D3DCOLOR_ARGB(255, 255, 255, 255), 500, 500, false);*/

			char buffer[64];
			snprintf(buffer, sizeof buffer, "X: %f", PEDSELF->GetPosition()->fX);
			pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 255), screen[0] / 4.4, screen[1]/1.24, false);

			snprintf(buffer, sizeof buffer, "Y: %f", PEDSELF->GetPosition()->fY);
			pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 255), screen[0] / 4.4, screen[1] / 1.21, false);

			snprintf(buffer, sizeof buffer, "Z: %f", PEDSELF->GetPosition()->fZ);
			pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 255), screen[0] / 4.4, screen[1] / 1.18, false);

			snprintf(buffer, sizeof buffer, "Int: %d", SF->getSAMP()->getPlayers()->pLocalPlayer->byteCurrentInterior);
			pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 255), screen[0] / 4.4, screen[1] / 1.15, false);

			//int total_pickups = SF->getSAMP()->getInfo()->pPools->pPickup->iPickupsCount;

			float x, y, z, telax, telay;
			int model;
			for (int i = 0; i < 4096; i++)
			{
				if (SF->getSAMP()->getInfo()->pPools->pPickup->IsPickupExists(i))
				{
					x = SF->getSAMP()->getInfo()->pPools->pPickup->pickup[i].fPosition[0];
					y = SF->getSAMP()->getInfo()->pPools->pPickup->pickup[i].fPosition[1];
					z = SF->getSAMP()->getInfo()->pPools->pPickup->pickup[i].fPosition[2];

					float distx = PEDSELF->GetPosition()->fX - x;
					if (distx < 0) distx = distx * -1;
					float disty = PEDSELF->GetPosition()->fY - y;
					if (disty < 0) disty = disty * -1;
					float distz = PEDSELF->GetPosition()->fZ - z;
					if (distz < 0) distz = distz * -1;

					if (distx < 5 && disty < 5 && distz < 5)
					{
						model = SF->getSAMP()->getInfo()->pPools->pPickup->pickup[i].iModelID;
						SF->getGame()->convert3DCoordsToScreen(x, y, z, &telax, &telay);
						snprintf(buffer, sizeof buffer, "Model: %d\nX: %f\nY: %f\nZ: %f", model, x, y, z);
						pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 255), telax, telay, false);
					}
				}
			}

			if (textd == true)
			{
			for (int i = 0; i < 2048; i++)
			{
				if (SF->getSAMP()->getInfo()->pPools->pTextdraw->IsExists(i))
				{
					float x = 0, y = 0;
					x = SF->getSAMP()->getInfo()->pPools->pTextdraw->textdraw[i]->fX;
					y = SF->getSAMP()->getInfo()->pPools->pTextdraw->textdraw[i]->fY;

					if (holdtextd && result == i)
					{
						snprintf(buffer, sizeof buffer, "X: %f\nY: %f\nID: %d", x, y, i);
						pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 0), (screen[0] * x) / 640, (screen[1] * y) / 480, false);
					}
					else
					{
						snprintf(buffer, sizeof buffer, "X: %f\nY: %f\nID: %d", x, y, i);
						pFont->Print(buffer, D3DCOLOR_ARGB(255, 255, 255, 255), (screen[0] * x) / 640, (screen[1] * y) / 480, false);
					}
					SF->getRender()->DrawPolygon((screen[0] * x) / 640, (screen[1] * y) / 480, 7, 7, 0, 10, D3DCOLOR_ARGB(255, 255, 0, 0));
				}
				}
			}
		}
			SF->getRender()->EndRender();
	};

	return true;
};


void CALLBACK mainloop()
{
	static bool init = false;
	if (!init)
	{
		if (GAME == nullptr)
			return;
		if (GAME->GetSystemState() != eSystemState::GS_PLAYING_GAME)
			return;
		if (!SF->getSAMP()->IsInitialized())
			return;
		SF->getSAMP()->getChat()->AddChatMessage(D3DCOLOR_XRGB(0, 0xAA, 0), "SA-MP DevTools [by:SrMilton]");
		SF->getRender()->registerD3DCallback(eDirect3DDeviceMethods::D3DMETHOD_PRESENT, Present);
		pFont = SF->getRender()->CreateNewFont("Calibri", 9, FCR_BORDER);
		SF->getGame()->getScreenResolution(&screen[0], &screen[1]);
		init = true;
	}

	if (init)
	{
		if (SF->getGame()->isKeyPressed(VK_F2))
		{
			if (textd == false)
			{
				SF->getSAMP()->getMisc()->ToggleCursor(true);
				textd = true;
			}
			else if(holdtextd == false)
			{
				SF->getSAMP()->getMisc()->ToggleCursor(false);
				textd = false;
			}
		}

			if (SF->getGame()->isKeyPressed(VK_LBUTTON) && textd == true && holdtextd == false)
			{
				if (tick > GetTickCount()) return;
				result = 2047;
				cursor = SF->getGame()->getCursorPos();
				cursor.x = (640 * cursor.x) / screen[0];
				cursor.y = (480 * cursor.y) / screen[1];

				for (int i = 0; i < 2048; i++)
				{
					if (SF->getSAMP()->getInfo()->pPools->pTextdraw->IsExists(i))
					{
						x[i] = SF->getSAMP()->getInfo()->pPools->pTextdraw->textdraw[i]->fX;
						y[i] = SF->getSAMP()->getInfo()->pPools->pTextdraw->textdraw[i]->fY;

					}
					int valor = (cursor.x - x[i]) + (cursor.y - y[i]);
					if (valor < 0) valor = valor * -1;
					int bestvalor = (cursor.x - x[result]) + (cursor.y - y[result]);
					if (bestvalor < 0) bestvalor = bestvalor * -1;

					if (valor < bestvalor && valor < 3)
					{
						result = i;
					}
					
				}
			/*	char passoatual[24];
				snprintf(passoatual, sizeof(passoatual), "TextDraw ID: %d", result);
				SF->getSAMP()->getChat()->AddChatMessage(D3DCOLOR_XRGB(0, 0xAA, 0), passoatual);*/
				if (result != 2047)
				{
					holdtextd = true;
					tick = GetTickCount() + 50;
				}
			}

			if (holdtextd == true)
			{
				cursor = SF->getGame()->getCursorPos();
				cursor.x = (640 * cursor.x) / screen[0];
				cursor.y = (480 * cursor.y) / screen[1];
				SF->getSAMP()->getInfo()->pPools->pTextdraw->textdraw[result]->fX = cursor.x;
				SF->getSAMP()->getInfo()->pPools->pTextdraw->textdraw[result]->fY = cursor.y;
			}

			if (SF->getGame()->isKeyPressed(VK_LBUTTON) && holdtextd == true)
			{
				if (tick > GetTickCount()) return;
				holdtextd = false;
				tick = GetTickCount() + 50;
			}
	}
}

void showhidediag()
{
	int& iDialogStatus = SF->getSAMP()->getDialog()->iIsActive;
	iDialogStatus ^= true;
	if (!iDialogStatus)
		SF->getSAMP()->getMisc()->ToggleCursor(false);
	return;
}

bool IsDriving(int PlayerID)
{
	if (PlayerID == ACTOR_SELF) PlayerID = SF->getSAMP()->getPlayers()->sLocalPlayerID;
	int state;
	if (PlayerID == SF->getSAMP()->getPlayers()->sLocalPlayerID)
		state = SF->getSAMP()->getPlayers()->pLocalPlayer->pSAMP_Actor->pGTA_Ped->state;
	else
		state = SF->getSAMP()->getPlayers()->pRemotePlayer[PlayerID]->pPlayerData->pSAMP_Actor->pGTA_Ped->state;
	if (state == 50)
		return true;
	else
		return false;
};

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
	switch (dwReasonForCall)
	{
	case DLL_PROCESS_ATTACH:
		SF->initPlugin(mainloop, hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
