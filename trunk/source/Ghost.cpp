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

#include "Ghost.h"
#include "CameraModel.h"
#include "GameState.h"

#include "IwRandom.h"
#include "s3eTimer.h"
#include "IwDebug.h"
#include "s3eVibra.h"
#include "IwGx.h"
#include "Audio.h"

#define FOUND_ANIM_STEPS 200
#define ANIM_TURN_DEGS_PER_STEP 1.8f

Ghost::Ghost(GhostType ghostType, Player *player) {
	IwRandSeed((int32)s3eTimerGetMs());

	Ghost::ghostType = ghostType;
	Ghost::player = player;

	foundUnintialized = true;
	foundInitTime = 0;
	foundAnimTime = 0;
	foundAnimProgress = 0;

	ectoplasm = GHOST_MAX_ECTOPLASM;
	lastUpdate = 0;

	found = false;
	bearing = 0;

	initRotation = IwRandMinMax(0, 360);
	rotation = initRotation;
	ghostAttack = NULL;

	hitTime = 0;
	nextAttackInterval = 0;
	attackDefendable = false;
	ghostHasBeenHit = false;

	floatingAngle = 0;

	animDodge = animAttack = false;
	agroPlayed = capturedPlayed = false;
}

void Ghost::cameraInit() {
#ifndef GHOST_DEBUG
	int tries = 0;
	do {
		bearing = IwRandMinMax(0, 360);
		tries++;
	} while (abs(bearing - player->getHeading()) < 90 && 
		tries < 50);
#endif
}

void Ghost::ghostGotHit(int hit) {
	if (!ghostHasBeenHit) {
		ghostHasBeenHit = true;
		Audio::PlayAmbientCameraCombat();
	}

	// 75% of hits success
	if (IwRandMinMax(0, 100) <= 75) {
		ectoplasm -= hit;
		IwTrace(GHOST_HUNTER, ("Ghost got hit, ectoplasm: %d", ectoplasm));
		hitTime = clock();
		s3eVibraVibrate(100, 100);
		Audio::Play("sounds/camera_player_attack_hit.mp3");
		Audio::Play("sounds/camera_ghost_takes_damage.mp3");
	} else {
		IwTrace(GHOST_HUNTER, ("Ghost dodged a hit, ectoplasm: %d", ectoplasm));
		s3eVibraVibrate(300, 20);
		animDodge = true;
		Audio::Play("sounds/camera_player_attack_miss.mp3");
	}
}

clock_t Ghost::getHitTime() {
	return hitTime;
}

bool Ghost::ghostUpdate() {

	double ghostDistance = abs(bearing - player->getHeading());

	if (found && !gameIsHalt()) { // Ghost initialization and movement

		if (foundUnintialized) {
			foundInitTime = clock() + 1000;
			foundUnintialized = false;
			return true;
		} else if (clock() < foundInitTime) {
			return true;
		}

		if (foundAnimProgress < FOUND_ANIM_STEPS) {

			if (foundAnimProgress == 0) {
				// Ghost waits for 2 seconds before attacking
				nextAttackInterval = 2000;
			}

			if (clock() - foundAnimTime > 3) {
				foundAnimProgress++;
				foundAnimTime = clock();
			}

			if (rotation != 0) {
				// Ghost rotates torwards player
				if (initRotation < 180) {
					rotation = initRotation - (float)foundAnimProgress*ANIM_TURN_DEGS_PER_STEP;
					if (rotation < 0) rotation = 0;
				} else {
					rotation = initRotation + (float)foundAnimProgress*ANIM_TURN_DEGS_PER_STEP;
					if (rotation > 360) rotation = 0;
				}
			}
		}

		float ghostMoveSpeed;
		float moveSmooth;
			
		{
			// Ghost moves torwards heading
			if (ghostDistance < 1.3) {
				ghostMoveSpeed = 2.f;
			} else if (ghostDistance < 3) {
				ghostMoveSpeed = 4.f;
			} else if (ghostDistance < 6) {
				ghostMoveSpeed = 7.f;
			} else if (ghostDistance < 12) {
				ghostMoveSpeed = 12.f;
			} else if (ghostDistance < 20) {
				ghostMoveSpeed = 16.f;
			} else {
				ghostMoveSpeed = 20.f;
			}

			// Fix going over north point of compass
			if (bearing < 90 && player->getHeading() > 360-90) {
				bearing += 360;
			} else if (bearing > 360-90 && player->getHeading() < 90) {
				bearing -= 360;
			}

			bearing = 
				bearing * ((100.f-ghostMoveSpeed)/100) + 
				player->getHeading() * (ghostMoveSpeed/100);
		}
	}

	// Decrease next intervals
	if (!gameIsHalt()) {
		clock_t currentTime = clock();
		if (nextAttackInterval >= 0)
			nextAttackInterval -= currentTime - lastUpdate;
		if (animAttackInterval >= 0)
			animAttackInterval -= currentTime - lastUpdate;

		lastUpdate = currentTime;
	} else {
		lastUpdate = clock();
	}

	// Initiate new attacks
	if (found && !gameIsHalt() && getAttack() == NULL && nextAttackInterval <= 0) {
		if (getGameState()->getIntroState() == INTRO_ATTACK) {
			attackDefendable = false;
		} else {
			attackDefendable = IwRandMinMax(0, 9) < 8; // 20 % of attacks not defendable
		}
		ghostAttack = new GhostAttack(player, ghostType);
		nextAttackInterval = IwRandMinMax(4000, 6000);
		animAttack = true;
		animAttackInterval = GHOST_ATTACK_ANIM_WAIT;
	}

	// Update attacks
	if (getAttack() != NULL) {
		if (getAttack()->isOver()) {
			delete ghostAttack;
			ghostAttack = NULL;
		} else {
			getAttack()->Update();
		}
	}

	return true;
}

int Ghost::getDistance() {
	int distanceDiff = ghostType.getDistance() - ghostType.getInitialDistance();

	distanceDiff *= ((float)foundAnimProgress)/FOUND_ANIM_STEPS;

	return ghostType.getInitialDistance() + distanceDiff;
}

void Ghost::setFound() {
	found = true;
}

bool Ghost::isFound() {
	return found;
}

bool Ghost::isDead() {
	return getEctoplasm() <= 0;
}

double Ghost::getBearing() {
	return bearing;
}

float Ghost::getRotation() {
	float facingPlayer = 180;

	return facingPlayer + rotation;
}

int Ghost::getEctoplasm() {
	return ectoplasm;
}

// Ghost must be tapped for aggro
void Ghost::tapped() {
	/*
	if (player->isDead()) return;

	if (tappedCount == 0) {
		tappedTime = clock();
	}
	if (clock() - tappedTime < 1500) {
		tappedCount++;
	} else {
		tappedTime = 0;
		tappedCount = 0;
	}
	// Press and release both increase the count
	if (tappedCount > 1) {
		IwTrace(GHOST_HUNTER, ("Player touched a ghost"));
	}
	*/
}

GhostAttack* Ghost::getAttack() {
	return ghostAttack;
}

bool Ghost::isAttackDefendable() {
	return attackDefendable;
}

void Ghost::floatingAngleUpdate(float x, float y, float z) {

	double currentAngle;
	float ratio = y/960;
	if (ratio < 0) ratio *= -1;
	
	if (y < 0 && x > 0) { // 0 - 90 deg
		currentAngle = 0*ratio + 90*(1-ratio);
	} else if (y > 0 && x > 0) { // 90 - 180 deg
		currentAngle = 180*ratio + 90*(1-ratio);
	} else if (y > 0 && x < 0) { // 180 - 270 deg
		currentAngle = 180*ratio + 270*(1-ratio);
	} else { // 270 - 360 deg
		currentAngle = 360*ratio + 270*(1-ratio);
	}

	if (floatingAngle < 90 && currentAngle > 360-90) {
		floatingAngle += 360;
	} else if (floatingAngle > 360-90 && currentAngle < 90) {
		floatingAngle -= 360;
	}
	floatingAngle = floatingAngle*0.1f + currentAngle*0.9f;
}

double Ghost::getFloatingAngle() {
	return floatingAngle;
}

bool Ghost::pollAnimDodge() {
	return poll(animDodge);
}
bool Ghost::pollAnimAttack() {
	if (animAttackInterval > 0) return false;
	if (animAttack) Audio::Play("sounds/camera_ghost_attack.mp3");
	return poll(animAttack);
}
bool Ghost::pollAnimAgro() {
	if (!isFound() || agroPlayed) return false;
	agroPlayed = true;
	Audio::Play("sounds/camera_ghost_first_contact_agro.mp3");
	return true;
}
bool Ghost::pollAnimCaptured() {
	if (!isDead() || capturedPlayed) return false;
	capturedPlayed = true;
	return true;
}

bool Ghost::poll(bool& poll) {
	bool temp = poll;
	poll = false;
	return temp;
}

GhostType Ghost::getGhostType() {
	return ghostType;
}
