// Project Identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include <string>
#include <vector>
#include <iostream>
#include <queue>
#include <algorithm>
#include <getopt.h>
#include "Zombie.h"
#include "Player.h"
//#include "P2Random.h"
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
            // Set statsNum to argument, need to figure out how to do that
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

    // Initialize random number generator here

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
    vector<Zombie*> deadList;
	priority_queue<Zombie*> shootList;
    Zombie killer;
	unsigned int round = 0; // Starts at 0 because increment happens towards beginning of loop

	zombList.reserve(totalZombs);
	liveList.reserve(totalZombs);
    deadList.reserve(totalZombs);

	while (player.isAlive()) {
        if (liveList.empty() && rounds.empty()) {
            cout << "VICTORY IN ROUND " << round << "! " << deadList.back()->name << " was the last zombie.\n";
            break;
        }
        ++round;

        if (verbose)
            cout << "Round: " << round << "\n";

		player.fillArrows();

		// Move le zombies
		for (size_t i = 0; i < liveList.size(); ++i) {
			liveList[i]->update();
            if (verbose)
                cout << "Moved: " << liveList[i]->name << "(distance: " << liveList[i]->distance << ", speed: " 
                << liveList[i]->speed << ", health: " << liveList[i]->health <<  ")\n";
            if (liveList[i]->distance == 0) {
                player.die();
                killer = *liveList[i];
            }
		}

        // Check if player was killed
        if (!player.isAlive()) {
            cout << "DEFEAT IN ROUND " << round << "! " << killer.name << " ate your brains!\n";
            break;
        }

		// Zombie generation goes inside this if statement
		if (rounds.front().num == round) {
            // Create random zombies
            for (unsigned int i = 0; i < rounds.front().numRand; ++i) {
                // Zombie *rando = new Zombie() --- Generate random zombies once I can get random number files downloaded
                Zombie *newRando = nullptr;
                zombList.push_back(newRando);
                liveList.push_back(newRando);
                shootList.push(newRando);
                if (verbose)
                    cout << "Created: " << newRando->name << "(distance: " << newRando->distance << ", speed: "
                    << newRando->speed << ", health: " << newRando->health << ")\n";
            }

            // Create named zombies
            for (unsigned int i = 0; i < rounds.front().numNamed; ++i) {
                Zombie *newNamed = &rounds.front().namedZombs[i];
                zombList.push_back(newNamed);
                liveList.push_back(newNamed);
                shootList.push(newNamed);
                if (verbose)
                    cout << "Created: " << newNamed->name << "(distance: " << newNamed->distance << ", speed: " 
                    << newNamed->speed << ", health: " << newNamed->health << ")\n";
            }
            rounds.pop();
		} // if statement for zombie generation

        // Gegen die Zombies kämpfen
        while (!player.quiverEmpty()) {
            player.shoot();
            --shootList.top()->health;
            if (shootList.top()->health == 0) {
                deadList.push_back(shootList.top());
                // liveList.erase(shootList.top()); Need to figure this out.
                if (verbose)
                    cout << "Destroyed: " << shootList.top()->name << "(distance: " << shootList.top()->distance << ", speed: "
                    << shootList.top()->speed << ", health: " << shootList.top()->health << ")\n";

                shootList.pop();
            }


        } // shoot loop

        if (median && !deadList.empty()) {
            // Print median, but I'm going to think about how to do median a bit better
        }
	} // round loopx

    if (stats) {

        cout << "Zombies still active: " << liveList.size() << "\n";

        for (size_t i = 0; i < min(statsNum, deadList.size()); ++i)

    } // Statistics stuff

	return 0;
}