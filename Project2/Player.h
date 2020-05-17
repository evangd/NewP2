// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#pragma once
#include "Zombie.h"

class Player
{
	unsigned int arrows = 0;
	unsigned int quiverCap;
	bool alive = true;

public:
	bool isAlive() {
		return alive;
	}

    bool quiverEmpty() {
        if (arrows == 0)
            return true;
        return false;
    }

	void die() {
		alive = false;
	}

    void shoot() {
        --arrows;
    }

	void fillArrows() {
		arrows = quiverCap;
	}

	Player() {} // Do I need this? It won't ever be used.

	Player(unsigned int quivCap) : quiverCap(quivCap) {}
};

