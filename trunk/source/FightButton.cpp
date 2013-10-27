/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#include "FightButton.h"
#include "GameState.h"

#include "IwDebug.h"
#include "IwGx.h"
#include "IwMaterial.h"
#include "IwTexture.h"
#include "IwGraphics.h"

static CIwFVec2 uvs[4] =
{
    CIwFVec2(0, 0),
    CIwFVec2(0, 1),
    CIwFVec2(1, 1),
    CIwFVec2(1, 0)
};

FightButton::FightButton() {

	fightBgTexture = new CIwTexture;
	fightBgTexture->LoadFromFile("textures/from_map_to_camera_bg.png");
	fightBgTexture->Upload();
	buttonPercY = 0.4f;

	fightTextTexture = new CIwTexture;
	fightTextTexture->LoadFromFile("textures/from_map_to_camera_text.png");
	fightTextTexture->Upload();

	{
		int16 w = IwGxGetScreenWidth();

		float whScale = (float)((double)fightBgTexture->GetWidth() / fightBgTexture->GetHeight());
		int16 h = w * 1/whScale;

		buttonBgXY = CIwSVec2(0, IwGxGetScreenHeight()-h);
		buttonBgWH = CIwSVec2(w, h);

		buttonTextXY = CIwSVec2(0, IwGxGetScreenHeight()-h);
		buttonTextWH = CIwSVec2(w, h);
	}

	tappedTime = 0;
	tappedCount = 0;
}

FightButton::~FightButton() {
	if (fightTextTexture)
		delete fightTextTexture;
}

void FightButton::Render() {
	{
		IwGxLightingOff();

		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(fightBgTexture);

		IwGxSetMaterial(pMat);
		IwGxSetUVStream(uvs);

		IwGxDrawRectScreenSpace(&buttonBgXY, &buttonBgWH);
	}

	{
		IwGxLightingOff();
	
		CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
		pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
		pMat->SetAlphaMode(CIwMaterial::ALPHA_BLEND);
		pMat->SetTexture(fightTextTexture);

		IwGxSetMaterial(pMat);
		IwGxSetUVStream(uvs);

		IwGxDrawRectScreenSpace(&buttonTextXY, &buttonTextWH);
	}
}

void FightButton::Touch(int x, int y) {
	if (y > buttonBgXY.y + buttonBgWH.y*buttonPercY) {
		if (clock() - tappedTime < 1500) {
			tappedCount++;
		} else {
			tappedTime = clock();
			tappedCount = 1;
		}
		// Press and release both increase the count
		if (tappedCount > 1) {
			IwTrace(GHOST_HUNTER, ("Player wants to fight"));
			getGameState()->setGameMode(CAMERA_MODE);
		}
	}
}
