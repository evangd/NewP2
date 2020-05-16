// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <getopt.h>
#include "Zombie.h"
#include "Player.h"
using namespace std;

struct Round {
	vector<Zombie> namedZombs;
	unsigned int num;
	unsigned int numRand;
	unsigned int numNamed;

	Round() {}

	Round(unsigned int roundNum, unsigned int numRand, unsigned int numNamed) : num(roundNum), numRand(numRand), numNamed(numNamed) {
		namedZombs.reserve(numNamed);
	}

};

int main(int argc, char *argv[]) {
	std::ios_base::sync_with_stdio(false);

	/*-------------GETOPT STUFF-------------
	// mode bools
	bool verbose = false;
	bool stats = false;
	bool median = false;

	opterr = true;
	int choice;
	int option_index = 0;
	option long_options[] = {
		{"verbose", no_argument, nullptr, 'v'},
		{"statistics", required_argument, nullptr, 's'},
		{"median", no_argument, nullptr, 'm'},
		{"help", no_argument, nullptr, 'h'},
		{nullptr, 0, nullptr, '\0'}
	};

	while ((choice = getopt_long(argc, argv, "vs:mh", long_options, &option_index)) != -1) {
		switch (choice) {
		case 'v':
			verbose = true;
			break;
		case 's':
			stats = true;
			break;
		case 'm':
			median = true;
		default:
			cerr << "Error: invalid option" << endl;
			exit(1);
		} // switch
	} // while
	/*-------------GETOPT STUFF-------------*/

	/*-------------READING STUFF------------*/

	queue<Round> rounds;
	string junk;
	int randSeed;
	unsigned int quiverCap;
	unsigned int maxDist;
	unsigned int maxSpeed;
	unsigned int maxHealth;
	unsigned int totalZombs = 0; // for reserving

	getline(cin, junk);

	//Read in starting parameters
	cin >> junk >> quiverCap >> junk >> randSeed >> junk >> maxDist >> junk >> maxSpeed >> junk >> maxHealth;

	// Read in rounds
	while (cin >> junk) {
		string name;
		unsigned int distance;
		unsigned int speed;
		unsigned int health;
		unsigned int roundNum;
		unsigned int randNum;
		unsigned int namedNum;
		cin >> junk >> roundNum >> junk >> randNum >> junk >> namedNum;

		totalZombs += randNum + namedNum;

		Round round(roundNum, randNum, namedNum);
		for (unsigned int i = 0; i < namedNum; ++i) {
			cin >> name >> junk >> distance >> junk >> speed >> junk >> health;
			Zombie zomb(name, distance, speed, health);
			round.namedZombs.push_back(zomb);
		}
		
		rounds.push(round);
	}

	/*------FIGHT LOOP*------*/

	Player player(quiverCap);
	vector<Zombie*> zombList;
	vector<Zombie*> liveList;
	priority_queue<Zombie*> shootList;
	unsigned int round = 1;

	zombList.reserve(totalZombs);
	liveList.reserve(totalZombs);

	while (player.isAlive() && !liveList.empty()) {

		player.fillArrows();

		// Move le zombies
		for (size_t i = 0; i < liveList.size(); ++i) {
			liveList[i]->update();
			if (liveList[i]->distance == 0)
				player.die(); // END GAME HERE?
		}

		// Zombie generation goes inside this if statement
		if (rounds.front().num == round) {
			
		}

	}

	return 0;
}