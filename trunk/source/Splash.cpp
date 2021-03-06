/* ========================================================================= *
 * Ghost Hunter Project                                                      *
 * https://ghosthuntermobile.wordpress.com/                                  *
 * ========================================================================= *
 * Copyright (C) 2013 Ghost Hunter Project                                   *
 *                                                                           *
 * Licensed under the Apache License, Version 2.0 (the "License");           *
 * you may not use this file except in compliance with the License.          *
 * You may obtain a copy of the License at                                   *
 *                                                                           *
 *     http://www.apache.org/licenses/LICENSE-2.0                            *
 *                                                                           *
 * Unless required by applicable law or agreed to in writing, software       *
 * distributed under the License is distributed on an "AS IS" BASIS,         *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
 * See the License for the specific language governing permissions and       *
 * limitations under the License.                                            *
 * ========================================================================= */

#include "Splash.h"
#include "Audio.h"

Splash::Splash() {
	texture = new CIwTexture;
	texture->LoadFromFile ("textures/splash.png");
	texture->Upload();

	double w = IwGxGetScreenWidth();
	double h = IwGxGetScreenHeight();
	{
		double whRatio = (double)texture->GetWidth() /  texture->GetHeight();

		if (whRatio > w/h) {
			w = h * whRatio;
		} else if (whRatio < w/h) {
			h = w * 1/whRatio;
		}
	}

	int16 x1 = (int16)(-abs(w - IwGxGetScreenWidth())/2);
	int16 x2 = (int16)(IwGxGetScreenWidth()  - x1);
	int16 y1 = (int16)(-abs(h - IwGxGetScreenHeight())/2);
	int16 y2 = (int16)(IwGxGetScreenHeight() - y1);
	
	vert[0].x = x1, vert[0].y = y1;
	vert[1].x = x1, vert[1].y = y2;
	vert[2].x = x2, vert[2].y = y2;
	vert[3].x = x2, vert[3].y = y1;

	uvs[0] = CIwFVec2(0, 0);
	uvs[1] = CIwFVec2(0, 1);
	uvs[2] = CIwFVec2(1, 1);
	uvs[3] = CIwFVec2(1, 0);

	this->Render();
	Audio::PlayAmbientMapInstantly();
}

Splash::~Splash() {
	if (texture)
		delete texture;
}

void Splash::Render() {
	IwGxClear(IW_GX_COLOUR_BUFFER_F | IW_GX_DEPTH_BUFFER_F);

	IwGxLightingOff();
	
	CIwMaterial* pMat = IW_GX_ALLOC_MATERIAL();
    pMat->SetModulateMode(CIwMaterial::MODULATE_NONE);
    pMat->SetTexture(texture);

	IwGxSetMaterial(pMat);

	IwGxSetScreenSpaceSlot(-1);

    IwGxSetUVStream(uvs);
    IwGxSetVertStreamScreenSpace(vert, 4);

    IwGxDrawPrims(IW_GX_QUAD_LIST, NULL, 4);

	IwGxFlush();
    IwGxSwapBuffers();
}
