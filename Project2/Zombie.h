// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043
#pragma once
#include <string>
#include <algorithm>
using namespace std;

struct Zombie
{
	string name;
	unsigned int distance;
	unsigned int speed;
	unsigned int health;
	unsigned int roundsActive = 1;
	unsigned int eta;
	bool alive = true;

	Zombie() {}

	Zombie(string name, unsigned int distance, unsigned int speed, unsigned int health) :
		name(name), distance(distance), speed(speed), health(health) {
		eta = distance / speed;
	}

	struct shootOrder {
		bool operator()(const Zombie *left, const Zombie *right) {
			if (left->eta == right->eta) {
				if (left->health == right->health) {
					return left->name > right->name;
				}
				return left->health > right->health;
			}
			return left->eta > right->eta;
		}
	};

	struct leastActive {
		bool operator()(const Zombie *left, const Zombie *right) {
			if (left->roundsActive == right->roundsActive) {
				return left->name < right->name;
			}
			return left->roundsActive < right->roundsActive;
		}
	};

	struct mostActive {
		bool operator()(const Zombie *left, const Zombie *right) {
			if (left->roundsActive == right->roundsActive) {
				return left->name < right->name;
			}
			return left->roundsActive > right->roundsActive;
		}
	};
};

