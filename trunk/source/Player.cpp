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

#include "Player.h"
#include "CameraModel.h"

#include "GameState.h"
#include "IwDebug.h"
#include "IwRandom.h"
#include "s3eTimer.h"
#include "s3eVibra.h"
#include "Audio.h"

Player::Player() {
	IwRandSeed((int32)s3eTimerGetMs());
	ready = 0;
	heading = 0;
	headingFilter = 0;
	strike = new Strike();
	hitTime = 0;
	init();

	animAttack = false;
};

void Player::init() {
	ghost = NULL;
	vitality = PLAYER_MAX_VITALITY;
	mana = PLAYER_MAX_MANA;
}

bool Player::headingUpdate() {

	// Fix going over north point of compass
	if (headingFilter < 90 && heading > 360-90) {
		headingFilter += 360;
	} else if (headingFilter > 360-90 && heading < 90) {
		headingFilter -= 360;
	}

	float filterStrength = getGameState()->getGameMode() == CAMERA_MODE ? 
		HEADING_FILTER_CAMERA : HEADING_FILTER_MAP;

	headingFilter = heading * filterStrength + 
		headingFilter * (1.f - filterStrength);

	return true;
}

bool Player::isReady() {
	return ready > 1;
}

void Player::playerGotHit(int hit) {
	Player::mana -= hit;
	if (hit > 0) hitTime = clock();
	s3eVibraVibrate(100, 230);
	Audio::Play("sounds/camera_player_takes_damage.mp3");
	IwTrace(GHOST_HUNTER, ("Player vitality %d", getVitality()));
}

int Player::getVitality() {
	return Player::vitality;
}

int Player::getMana() {
	return Player::mana;
}

void Player::initFight() {
	mana = PLAYER_MAX_MANA;
	ghostSeen = false;
}

void Player::lostBattle() {
	Audio::Play("sounds/camera_player_lose.mp3");
	setGhost(NULL);
	vitality -= 2;
}

void Player::wonBattle() {
	Audio::Play("sounds/camera_player_win.mp3");
	setGhost(NULL);
	vitality += 1;
	if (vitality > PLAYER_MAX_VITALITY) vitality = PLAYER_MAX_VITALITY;
}

void Player::compassUpdate(double heading, bool error) {
	Player::heading = heading;
	if (!isReady()) {
		headingFilter = heading;
		ready++;
	}
}

double Player::getHeading() {
	return headingFilter;
}

void Player::accelometerUpdate(int32 x, int32 y, int32 z) {
	int strikeRes = strike->strikeUpdate(x, y, z);

	int ghostAngle = abs(getHeading() - getGameState()->getGhost()->getBearing());
	bool angleOk = ghostAngle < 40;

	FightTutorial* tutorial = getFightTutorial();
	if (angleOk) {

		if (tutorial != NULL) {
			tutorial->counterTrigger(TUTORIAL_SEARCH);
			tutorial->triggerTutorial(TUTORIAL_ATTACK);
		}

		if (!ghostSeen) {
			ghostSeen = true;
			Audio::Play("sounds/camera_ghost_is_seen.mp3");
		}
	}

	if (strikeRes > 0 && isReady()) {

		{ // Initial hit?
			if (angleOk && ghost == NULL) {
				setGhost(getGameState()->getGhost());
				ghost->setFound();
			}
		}

		if (getFightTutorial() != NULL)
				getFightTutorial()->counterTrigger(TUTORIAL_ATTACK);

		if (!gameIsHalt() && ghost != NULL) {
			int manaLoss = 2;
			mana -= manaLoss;
			IwTrace(GHOST_HUNTER, ("Player loses mana as she hits %d, mana: %d", manaLoss, mana));

			// 7-10 successful hits to kill a ghost
			int hit = IwRandMinMax(GHOST_MAX_ECTOPLASM/100*7, GHOST_MAX_ECTOPLASM/100*10);
			if (IwRandMinMax(0, 15) == 15) hit *= 2; // Player crit, 1/16 chance

			IwTrace(GHOST_HUNTER, ("Player is hitting %d", hit));
			
			animAttack = true;
			ghost->ghostGotHit(hit);
		}
	}
}

void Player::setGhost(Ghost *ghost) {
	Player::ghost = ghost;
}

bool Player::isDead() {
	return vitality <= 0;
}

void Player::resurrect() {
	init();
}

clock_t Player::getHitTime() {
	return hitTime;
}

bool Player::pollAnimAttack() {
	return poll(animAttack);
}

bool Player::poll(bool& poll) {
	bool temp = poll;
	poll = false;
	return temp;
}

int Player::getWinCount() {
	return winCount;
}
int Player::getLoseCount() {
	return loseCount;
}
int Player::getDeadCount() {
	return deadCount;
}
void Player::increaseWinCount() {
	winCount++;
}
void Player::increaseLoseCount() {
	loseCount++;
}
void Player::increaseDeadCount() {
	deadCount++;
}
