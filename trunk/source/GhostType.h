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

#ifndef _GHOST_TYPE_H
#define _GHOST_TYPE_H

#define GHOST_TYPE_REFERENCE_DISTANCE 700

class GhostType {
	public:
	static const GhostType VIKING;
	static const GhostType SKELMAN;

	GhostType() {}

	private:
	int id;
	int distance;
	int initialDistance;
	float strength;

	GhostType(int id, int distance, int initialDistance, float strength);

	public:
	int getDistance();
	int getInitialDistance();
	float getStrength();
	bool operator==(const GhostType&) const;
};

#endif
