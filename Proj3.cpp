#include <GLUT/GLUT.h>
#include <iostream>
#include "board.h"

using namespace std;


int main(int argc, char** argv) {
	string input;
	cout << "Color type: Black or White" << endl;
	cin >> input;
	Board* Othello = new Board;
	Othello->SetSquare();
	Player* player = Board::player;
	AI* ai = Board::ai;
	if(input.compare("Black") == 0) {
		player->setType(BLACK);
		ai->setType(WHITE);
		Othello->initializeBoard(argc, argv);
	}
	else if(input.compare("White") == 0) {
		player->setType(WHITE);
		ai->setType(BLACK);
		Othello->initializeBoard(argc, argv);
	}
	else {
		cout << "Invalid choice." << endl;
	}
}