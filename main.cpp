#include "Board.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <fstream>
using namespace std;
int main() {

    //read from file (using placeholder values)
    ifstream file("files/config.cfg");
    vector<int> params;

    if (file.is_open()) {
        string line;
        for(int i = 0; i < 3; i++){
            getline(file, line);
            params.push_back(stoi(line));
        }
    }
    else{
        cout << "Error reading file" << endl;
    }

    int columns = params[0];
    int rows = params[1];
    int numMines = params[2];


    Board playerBoard(rows,columns,numMines); // 5x5 board with 5 bombs
    int** board = playerBoard.Initialize();
    playerBoard.Welcome(playerBoard,float(columns), float(rows), numMines);

    return 0;

}