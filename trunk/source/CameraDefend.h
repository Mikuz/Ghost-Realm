/*
 * (C) 2013-2024 Ghost Hunter Project.
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 */

#ifndef _GHOST_DEFEND_H
#define _GHOST_DEFEND_H

#include "GhostAttack.h"

#include <time.h>
#include "IwGraphics.h"

class CameraDefend {
	private:
	Ghost* ghost;

	CIwTexture* defendingDotTexture;
	CIwFVec2 defendVertsLeft[4];
	CIwFVec2 defendVertsRight[4];

	CIwFVec2 drawInit;
	bool drawing;
	bool over;

	public:
	CameraDefend(Ghost* ghost);
	~CameraDefend();
	void Update();
	void Render();
	void Touch(int32 x, int32 y);
	void Motion(int32 x, int32 y);
	bool isOver();
};

#endif