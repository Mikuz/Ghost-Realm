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

#include "IwGraphics.h"
#include "Iw2D.h"

class ManaBar {
	private:
	CIw2DImage* borderTexture;
	CIw2DImage* centerTexture;
	CIw2DImage* endTexture;
	CIw2DImage* startTexture;

	CIwFVec2 startVerts[4];

	CIwFVec2 borderTopLeft;
	CIwFVec2 borderSize;

	public:
	ManaBar();
	~ManaBar();

	void Render();

};
