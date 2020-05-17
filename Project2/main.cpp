// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <getopt.h>
#include "Zombie.h"
#include "Player.h"
#include "P2random.h"
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

	/*-------------GETOPT STUFF-------------*/
	// mode bools
	bool verbose = false;
	bool stats = false;
	bool median = false;
    size_t statsNum = 0;

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
			statsNum = static_cast<size_t>(atoi(optarg));
			break;
		case 'm':
			median = true;
			break;
		default:
			cerr << "Error: invalid option" << endl;
			exit(1);
		} // switch
	} // while
	/*-------------GETOPT STUFF-------------*/

	/*-------------READING STUFF------------*/

	queue<Round> rounds;
	string junk;
	unsigned int randSeed;
	unsigned int quiverCap;
	unsigned int maxDist;
	unsigned int maxSpeed;
	unsigned int maxHealth;
	unsigned int totalZombs = 0; // for reserving

	getline(cin, junk);

	//Read in starting parameters and initialize number generator
	cin >> junk >> quiverCap >> junk >> randSeed >> junk >> maxDist >> junk >> maxSpeed >> junk >> maxHealth;
	P2random::initialize(randSeed, maxDist, maxSpeed, maxHealth);

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
    vector<Zombie*> deadList;
	priority_queue<Zombie*, vector<Zombie*>, Zombie::shootOrder> shootList;
    Zombie killer;
	unsigned int round = 0; // Starts at 0 because increment happens towards beginning of loop
	size_t liveTotal = 0;

	zombList.reserve(totalZombs);
    deadList.reserve(totalZombs);

	while (player.isAlive()) {
        if (liveTotal == 0 && rounds.empty()) {
            cout << "VICTORY IN ROUND " << round << "! " << deadList.back()->name << " was the last zombie.\n";
            break;
        }
        ++round;

        if (verbose)
            cout << "Round: " << round << "\n";

		player.fillArrows();

		// Move le zombies
		if (liveTotal != 0) {
			for (size_t i = 0; i < zombList.size(); ++i) {
				if (zombList[i]->alive) {
					zombList[i]->update();
					if (verbose)
						cout << "Moved: " << zombList[i]->name << " (distance: " << zombList[i]->distance << ", speed: "
						<< zombList[i]->speed << ", health: " << zombList[i]->health << ")\n";
					if (zombList[i]->distance == 0) {
						player.die();
						killer = *zombList[i];
					}
				}
			}
		}

        // Check if player was killed
        if (!player.isAlive()) {
            cout << "DEFEAT IN ROUND " << round << "! " << killer.name << " ate your brains!\n";
            break;
        }

		// Zombie generation goes inside this if statement
		if (!rounds.empty() && rounds.front().num == round) {
            // Create random zombies
            for (unsigned int i = 0; i < rounds.front().numRand; ++i) {
				string name = P2random::getNextZombieName();
				unsigned int dist = P2random::getNextZombieDistance();
				unsigned int speed = P2random::getNextZombieSpeed();
				unsigned int health = P2random::getNextZombieHealth();
				Zombie *newRando = new Zombie(name, dist, speed, health);

                zombList.push_back(newRando);
                shootList.push(newRando);
				++liveTotal;
                if (verbose)
                    cout << "Created: " << newRando->name << " (distance: " << newRando->distance << ", speed: "
                    << newRando->speed << ", health: " << newRando->health << ")\n";
            }

            // Create named zombies
            for (unsigned int i = 0; i < rounds.front().numNamed; ++i) {
				Zombie temp = rounds.front().namedZombs[i]; // Wonder if storying these in the round queue as Zombies is wasteful
				Zombie *newNamed = new Zombie(temp.name, temp.distance, temp.speed, temp.health);
                zombList.push_back(newNamed);
                shootList.push(newNamed);
				++liveTotal;
                if (verbose)
                    cout << "Created: " << newNamed->name << " (distance: " << newNamed->distance << ", speed: " 
                    << newNamed->speed << ", health: " << newNamed->health << ")\n";
            }
            rounds.pop();
		} // if statement for zombie generation

        // Gegen die Zombies kämpfen
        while (!player.quiverEmpty() && liveTotal != 0) {
            player.shoot();
            --shootList.top()->health;
            if (shootList.top()->health == 0) {
				shootList.top()->alive = false;
                deadList.push_back(shootList.top());
				--liveTotal;
                if (verbose)
                    cout << "Destroyed: " << shootList.top()->name << " (distance: " << shootList.top()->distance << ", speed: "
                    << shootList.top()->speed << ", health: " << shootList.top()->health << ")\n";

                shootList.pop();
            }


        } // shoot loop

        if (median && !deadList.empty()) {
            // Print median, but I'm going to think about how to do median a bit better
        }
	} // round loop

    if (stats) {

        cout << "Zombies still active: " << zombList.size() - deadList.size() << "\n";

		for (size_t i = 0; i < min(statsNum, deadList.size()); ++i) {

		}

    } // Statistics stuff

	for (size_t i = 0; i < zombList.size(); ++i) {
		if (i < deadList.size())
			deadList[i] = nullptr;
		/*if (!shootList.empty()) {
			shootList.top() = nullptr;
			shootList.pop();
		}*/
		delete zombList[i];
		zombList[i] = nullptr;
	}

	return 0;
}