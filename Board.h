//CREATED BY NICOLAS SLENKO
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <map>
#include <sstream>
#include <fstream>
#include <SFML/System/Clock.hpp>
#include <algorithm>


#ifndef PROJECT3_BOARD_H
#define PROJECT3_BOARD_H

using namespace std;

struct Player {
    string name;
    string time;
};

bool comparePlayers(Player& a, Player& b) {
    string timeA = a.time;
    timeA.erase(remove(timeA.begin(), timeA.end(), ','), timeA.end());
    int intTimeA = stoi(timeA);

    string timeB = b.time;
    timeB.erase(remove(timeB.begin(), timeB.end(), ','), timeB.end());
    int intTimeB = stoi(timeB);

    return intTimeA < intTimeB;
}

class Tile{

public:
    int value;
    sf::Texture texture;
    sf::Sprite sprite;
    float x;
    float y;
    bool isRevealed = false;
    int time = 0;


    Tile(float x, float y, int val){
        this->texture = texture;
        this->x = x;
        this->y = y;
        this->value = val;


        texture.loadFromFile("./files/images/tile_hidden.png");
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    Tile(float x, float y){
        this->texture = texture;
        this->x = x;
        this->y = y;

        texture.loadFromFile("./files/images/tile_revealed.png");
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(sprite);
    }

    void drawNumber(sf::RenderWindow& window) const {
        sf::Texture numberTexture;

        if (value == 0) {
            numberTexture.loadFromFile("./files/images/tile_revealed.png");
        }
        else if (value >= 1 && value <= 8) {
            numberTexture.loadFromFile("./files/images/number_" + to_string(value) + ".png");
        }
        else if (value == -1) {
            numberTexture.loadFromFile("./files/images/mine.png");
        }
        sf::Sprite numberSprite(numberTexture);
        numberSprite.setPosition(x, y);
        window.draw(numberSprite);
    }


    void changeSprite() {
        sprite.setColor(sf::Color(255, 255, 255, 0)); //(transparent)
    }

    void revertSprite(){
        sprite.setColor(sf::Color(255, 255, 255, 255));
    }

};

class Flag{
public:
    int value;
    sf::Texture texture;
    sf::Sprite sprite;
    float x;
    float y;
    bool isRevealed = false;




    Flag(float x, float y){
        this->texture = texture;
        this->x = x;
        this->y = y;

        sprite.setPosition(x, y);
        sprite.setColor(sf::Color(255, 255, 255, 0));

        isRevealed = false;
    }

    void hide(){
        sprite.setColor(sf::Color(255, 255, 255, 0));
        isRevealed = false;
    }

    void reveal(){
        sprite.setColor(sf::Color(255, 255, 255, 255));
        isRevealed = true;
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(sprite);
    }
};



class Board{
public:

    const int numRows = 0;
    const int numCols = 0;
    int numBombs = 0;
    string name;
    string nameFinal;
    string time;
    map<string, string> leaderboard;
    bool paused = false;
    bool lose = false;
    bool win = false;
    bool debugvar = false;
    bool leaderboardVar = false;
    int numFlags = 0;
    bool firstTime = true;
    float finalSeconds = 0;
    bool checked = false;

    int **grid;

    Board(const int rows, const int cols, int numBombs) : numRows(rows), numCols(cols), numBombs(numBombs) {
        grid = new int*[numRows];
        for (int i = 0; i < numRows; ++i) {
            grid[i] = new int[numCols];
            for (int j = 0; j < numCols; ++j) {
                grid[i][j] = 0;
            }
        }
    }

    ~Board() {
        for (int i = 0; i < numRows; ++i) {
            delete[] grid[i];
        }
        delete[] grid;
    }

    int** Initialize() {

        std::srand(std::time(nullptr));

        //place bombs
        int bombsPlaced = 0;
        while (bombsPlaced < numBombs) {
            int row = rand() % numRows;
            int col = rand() % numCols;
            if (grid[row][col] != -1) {
                grid[row][col] = -1;
                bombsPlaced++;
            }
        }


        //generate numbers for tiles
        for (int i = 0; i < numRows; i++) {
            for (int j = 0; j < numCols; j++) {
                if (grid[i][j] != -1) {
                    int bombCount = 0;
                    //look in a square radius
                    for (int k = -1; k <= 1; k++) {
                        for (int l = -1; l <= 1; l++) {
                            int m = i + k;
                            int n = j + l;
                            if (m >= 0 && m < numRows && n >= 0 && n < numCols && grid[m][n] == -1) {
                                bombCount++;
                            }
                        }
                    }
                    grid[i][j] = bombCount;
                }
            }
        }
        return grid;
    }

    void setText(sf::Text &text, float x, float y){
        sf::FloatRect textRect = text.getLocalBounds();
        text.setOrigin(textRect.left + textRect.width/2.0f,textRect.top + textRect.height/2.0f);
        text.setPosition(sf::Vector2f(x, y));
    }

    void Welcome(Board &Game_Board, float cols, float rows, int numMines) {
        int welcomeHeight = (rows * 32) + 100;
        int welcomeWidth = (cols * 32);

        sf::RenderWindow window(sf::VideoMode(welcomeWidth, welcomeHeight), "Minesweeper", sf::Style::Close);

        sf::Font font;
        if (!font.loadFromFile("files/font.ttf")) {
            cout << "Fail!" << endl;
        }

        sf::Text title("WELCOME TO MINESWEEPER!", font, 24);
        setText(title, welcomeWidth/2, welcomeHeight/2 - 150);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold | sf::Text::Underlined);

        sf::Text prompt("Enter your name: ", font, 20);
        setText(prompt, welcomeWidth/2, welcomeHeight/2 - 75);
        prompt.setFillColor(sf::Color::White);
        prompt.setStyle(sf::Text::Bold);


        string name;

        sf::Text input("", font, 18);
        setText(input, welcomeWidth/2, welcomeHeight/2 - 45);
        input.setFillColor(sf::Color::Yellow);
        input.setStyle(sf::Text::Bold);

        sf::Text inputText("", font, 18);
        inputText.setFont(font);
        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::TextEntered) {
                    char typedChar = (char)event.text.unicode;
                    if (isalpha(typedChar) && name.size() < 10) {
                        if (name.empty()) {
                            name += toupper(typedChar);
                        }
                        else {
                            name += tolower(typedChar);
                        }
                    }
                    else if (event.text.unicode == 8 && !name.empty()) {
                        name.pop_back();
                    }
                    else if (event.text.unicode == 13 && !name.empty()) {
                        cout << name << endl;
                        nameFinal = name;
                        window.close();
                        GenerateGameBoard(Game_Board, cols, rows, numMines);
                    }

                }
            }

            inputText.setString(name + ((name.size() < 10) ? "|" : ""));
            setText(inputText, welcomeWidth/2, welcomeHeight/2 - 45);

            window.clear(sf::Color::Blue);
            window.draw(title);
            window.draw(prompt);
            window.draw(inputText);
            window.display();
        }


    }

    bool playerHasWon(const vector<vector<Tile>>& tiles, int numMines) {
        int numUnrevealed = 0;
        //two seperate counts, one for mines, one for unrevealed tiles. When these equal, you win!
        for (int i = 0; i < tiles.size(); i++) {
            for (int j = 0; j < tiles.at(i).size(); j++) {
                if (!tiles[i][j].isRevealed) {
                    numUnrevealed +=1;
                }
            }
        }

        if(numUnrevealed == numMines){
            return true;
        }

        return false;
    }

    void revealAdjacentEmptyTiles(int x, int y, int cols, int rows, vector<vector<Tile>>& tiles, vector<vector<Flag>>& flags) {

        //mark numbers as revealed when they are shown
        if (tiles[y][x].value != 0) {
            tiles[y][x].isRevealed = true;
            return;
        }
        //mark 0 tiles as revealed
        tiles[y][x].isRevealed = true;


        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int k = x + i;
                int l = y + j;
                if (k >= 0 && k < cols && l >= 0 && l < rows && !tiles[l][k].isRevealed && !flags[l][k].isRevealed) {
                    tiles[l][k].changeSprite();
                    revealAdjacentEmptyTiles(k, l, cols, rows, tiles, flags);
                }
            }
        }
    }


    void GenerateGameBoard(Board &Game_Board, float cols, float rows, int numMines){

        sf::Clock clock;


        int gameHeight = (rows * 32) + 100;
        int gameWidth = (cols * 32);


        sf::RenderWindow window(sf::VideoMode(gameWidth, gameHeight), "Minesweeper", sf::Style::Close);



        float tileWidth = window.getSize().x / numCols;
        float tileHeight = (window.getSize().y - 100) / numRows;
        vector<vector<Tile>> tiles;
        vector<vector<Tile>> backgroundTiles;
        vector<vector<Flag>> flags;



        for (int i = 0; i < rows; ++i) {
            vector<Tile> rowTiles;
            vector<Tile> rowBackgroundTiles;
            vector<Flag> rowFlags;
            for (int j = 0; j < cols; ++j) {
                float x = j * tileWidth;
                float y = i * tileHeight;
                int value = grid[i][j];
                Tile tile(x, y, value);
                rowTiles.push_back(tile);

                Tile backgroundTile(x, y);
                rowBackgroundTiles.push_back(backgroundTile);

                Flag flag(x,y);
                rowFlags.push_back(flag);

            }
            tiles.push_back(rowTiles);
            backgroundTiles.push_back(rowBackgroundTiles);
            flags.push_back(rowFlags);

        }


        //SmileyFaceStuff
        sf::Texture smileytexture;
        smileytexture.loadFromFile("./files/images/face_happy.png");

        sf::Sprite face(smileytexture);
        float faceX = (cols/2.0f * 32) - 32;
        float faceY = 32 * (rows + 0.5);
        face.setPosition(faceX, faceY);

        sf::Texture sadFace;
        sadFace.loadFromFile("./files/images/face_lose.png");

        sf::Texture winFaceTexture;
        winFaceTexture.loadFromFile("./files/images/face_win.png");
        sf::Sprite winFace(winFaceTexture);
        winFace.setPosition(faceX, faceY);
        //hide
        winFace.setColor(sf::Color(255, 255, 255, 0));



        //bombRevealIcon
        sf::Texture bombTexture;
        bombTexture.loadFromFile("./files/images/debug.png");

        sf::Sprite debug(bombTexture);
        float debugX = (cols * 32) - 304;
        float debugY = 32 * (rows + 0.5);
        debug.setPosition(debugX, debugY);


        //Pause button
        sf::Texture pauseTexture;
        pauseTexture.loadFromFile("./files/images/pause.png");


        sf::Sprite pause(pauseTexture);
        float pauseX = (cols * 32) - 240;
        float pauseY = (32 * (rows + 0.5));
        pause.setPosition(pauseX, pauseY);

        //Play button
        sf::Texture playTexture;
        playTexture.loadFromFile("./files/images/play.png");

        sf::Sprite play(playTexture);
        float playX = (cols * 32) - 240;
        float playY = (32 * (rows + 0.5));
        pause.setPosition(playX, playY);


        //Leaderboard Button
        sf::Texture leaderboardTexture;
        leaderboardTexture.loadFromFile("./files/images/leaderboard.png");

        sf::Sprite leaderboard(leaderboardTexture);
        float leadX = (cols * 32) - 176;
        float leadY = (32 * (rows + 0.5));
        leaderboard.setPosition(leadX, leadY);

        //flag
        sf::Texture flagTexture;
        flagTexture.loadFromFile("./files/images/flag.png");
        sf::Sprite flag(flagTexture);

        //digits
        sf::Texture digitsTexture;
        digitsTexture.loadFromFile("./files/images/digits.png");

        vector<sf::Texture> digitTextures(11);

        for (int i = 0; i < 11; i++) {
            int offsetX = i * 21;
            sf::IntRect textureRect(offsetX, 0, 21, 32);
            digitTextures[i].loadFromImage(digitsTexture.copyToImage(), textureRect);
        }

        sf::Sprite bombCounter1;
        bombCounter1.setPosition(33, 32 * (rows + 0.5) + 16);
        sf::Sprite bombCounter2;
        bombCounter2.setPosition(33 + 21, 32 * (rows + 0.5) + 16);
        sf::Sprite bombCounter3;
        bombCounter3.setPosition(33 + 42, 32 * (rows + 0.5) + 16);

        sf::Sprite time1;
        time1.setPosition((cols * 32) - 97, 32 * (rows + 0.5) + 16);
        sf::Sprite time2;
        time2.setPosition((cols* 32) -76, 32 * (rows + 0.5) + 16);
        sf::Sprite time3;
        time3.setPosition((cols* 32) -54, 32 * (rows + 0.5) + 16);
        sf::Sprite time4;
        time4.setPosition((cols* 32) -33, 32 * (rows + 0.5) + 16);


        //need this for reset to work
        for (const auto& row : backgroundTiles) {
            for (const auto& backgroundTile : row) {
                backgroundTile.draw(window);
            }
        }


        float pausedSeconds = 0;


        sf::Clock PausedTime;
        float totalPaused = 0;


        while (window.isOpen()) {
            if(!paused) {

                sf::Time elapsedTime = clock.getElapsedTime();

                float totalSeconds = elapsedTime.asSeconds() - pausedSeconds;


                int minutes = static_cast<int>(totalSeconds) / 60;
                int seconds = static_cast<int>(totalSeconds) % 60;


                int digit1_minutes = minutes / 10;
                int digit2_minutes = minutes % 10;

                int digit1_seconds = seconds / 10;
                int digit2_seconds = seconds % 10;

                time1.setTexture(digitTextures[digit1_minutes]);
                time2.setTexture(digitTextures[digit2_minutes]);
                time3.setTexture(digitTextures[digit1_seconds]);
                time4.setTexture(digitTextures[digit2_seconds]);

                sf::Time pausedTime = PausedTime.restart();

            }
            else{

                sf::Time elapsedTime = PausedTime.getElapsedTime();
                pausedSeconds = elapsedTime.asSeconds();

            }



            sf::Event event;
            while (window.pollEvent(event)) {
                if(firstTime){
                    int valToDisplay = numMines - numFlags;
                    int digit1 = valToDisplay / 100;
                    int digit2 = (valToDisplay/10) % 10;
                    int digit3 = valToDisplay % 10;

                    if(valToDisplay < 0 && valToDisplay > -10){
                        bombCounter1.setTexture(digitTextures[10]);
                        bombCounter2.setTexture(digitTextures[digit2]);
                        bombCounter3.setTexture(digitTextures[digit3]);
                    }
                    else{
                        bombCounter1.setTexture(digitTextures[digit1]);
                        bombCounter2.setTexture(digitTextures[digit2]);
                        bombCounter3.setTexture(digitTextures[digit3]);
                    }

                    time1.setTexture(digitTextures[0]);
                    time2.setTexture(digitTextures[0]);
                    time3.setTexture(digitTextures[0]);
                    time4.setTexture(digitTextures[0]);

                    firstTime = false;
                }
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                else if (event.type == sf::Event::MouseButtonPressed) {



                    if (event.mouseButton.button == sf::Mouse::Left) {


                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));


                        int tileX = mousePos.x / tileWidth;
                        int tileY = mousePos.y / tileHeight;

                        float Xcor = mousePos.x / tileWidth;
                        float Ycor = mousePos.y / tileHeight;


                        //button handling
                        if (face.getGlobalBounds().contains(mousePosition)) {
                            window.close();
                            Board newBoard(rows,cols,numMines);
                            int** gridNew = newBoard.Initialize();
                            this->grid = gridNew;
                            win = false;
                            lose = false;
                            debugvar = false;
                            paused = false;
                            leaderboardVar = false;
                            firstTime = true;
                            numFlags = 0;
                            GenerateGameBoard(newBoard, cols,  rows, numMines);

                        }

                        if (debug.getGlobalBounds().contains(mousePosition) && !paused && !lose && !win) {

                            if (!debugvar && !(win || lose)) {
                                for (int i = 0; i < tiles.size(); i++) {
                                    for (int j = 0; j < tiles.at(i).size(); j++) {
                                        if (tiles[i][j].value == -1) {
                                            tiles[i][j].changeSprite();
                                        }
                                    }
                                }
                                debugvar = true;
                            }
                            else if (debugvar && !(win || lose)){
                                for(int i = 0; i < tiles.size(); i++){
                                    for(int j = 0; j < tiles.at(i).size();j++){
                                        if(tiles[i][j].value == -1){
                                            tiles[i][j].revertSprite();
                                        }
                                    }
                                }
                                debugvar = false;
                            }
                        }

                        if (pause.getGlobalBounds().contains(mousePosition) && !lose && !win) {
                            if(!paused){
                                paused = true;
                            }
                            else{
                                paused = false;
                            }


                        }

                        if (leaderboard.getGlobalBounds().contains(mousePosition)) {
                            leaderboardVar = true;
                            GenerateLeaderBoard(cols, rows, pause);
                            pause.setTexture(playTexture);

                        }

                        if (tileX >= 0 && tileX < cols && tileY >= 0 && tileY < rows && !paused && !lose && !win) {
                            if(!flags[tileY][tileX].isRevealed){
                                tiles[tileY][tileX].changeSprite();
                                tiles[tileY][tileX].isRevealed = true;
                            }
                            if(tiles[tileY][tileX].value == 0 && !flags[tileY][tileX].isRevealed){

                                tiles[tileY][tileX].isRevealed = true;
                                revealAdjacentEmptyTiles(tileX,tileY,cols, rows, tiles, flags);
                            }
                            if(tiles[tileY][tileX].value == -1){
                                cout << "You lose!" << endl;
                                lose = true;
                                paused = true;
                                checked = false;
                                face.setTexture(sadFace);
                                //reveal mines
                                for(int i = 0; i < tiles.size(); i++){
                                    for(int j = 0; j < tiles.at(i).size(); j++){
                                        if(tiles[i][j].value == -1){
                                            tiles[i][j].changeSprite();
                                            tiles[i][j].drawNumber(window);
                                        }
                                    }
                                }
                            }
                            if(playerHasWon(tiles,numMines)){
                                cout << "You Win!" << endl;
                                win = true;
                                paused = true;
                                winFace.setColor(sf::Color(255, 255, 255, 255));
                                face.setColor(sf::Color(255, 255, 255, 0));

                                sf::Time finalTime = clock.getElapsedTime();
                                finalSeconds =  finalTime.asSeconds();


                                for(int i = 0; i < tiles.size(); i++){
                                    for(int j = 0; j < tiles.at(i).size(); j++){
                                        if(!tiles[i][j].isRevealed){
                                            flags[i][j].reveal();
                                        }
                                    }
                                }
                                //win = false;


                                //DO NOT GET RID OF THESE LINES!!!!
                                sf::Texture happyFace;
                                happyFace.loadFromFile("./files/images/face_win.png");
                                face.setTexture(happyFace);
                            }

                        }
                    }
                    else if (event.mouseButton.button == sf::Mouse::Right) {

                        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                        int tileX = mousePos.x / tileWidth;
                        int tileY = mousePos.y / tileHeight;




                        if (tileX >= 0 && tileX < cols && tileY >= 0 && tileY < rows && !paused && !lose && !win) {
                            if (flags[tileY][tileX].isRevealed == false &&  tiles[tileY][tileX].isRevealed == false) {
                                flags[tileY][tileX].reveal();
                                flags[tileY][tileX].isRevealed = true;
                                numFlags += 1;

                            }
                            else if(flags[tileY][tileX].isRevealed == true){
                                flags[tileY][tileX].hide();
                                flags[tileY][tileX].isRevealed = false;
                                numFlags -= 1;

                            }

                        }
                    }

                    int valToDisplay = numMines - numFlags;
                    int digit1 = valToDisplay / 100;
                    int digit2 = (valToDisplay/10) % 10;
                    int digit3 = valToDisplay % 10;

                    if(valToDisplay < 0 && valToDisplay > -10){
                        bombCounter1.setTexture(digitTextures[10]);
                        bombCounter2.setTexture(digitTextures[-valToDisplay % 10]);
                        bombCounter3.setTexture(digitTextures[digit3]);
                    }
                    else if(valToDisplay <= -10){
                        bombCounter1.setTexture(digitTextures[10]);
                        bombCounter2.setTexture(digitTextures[-digit2]);
                        bombCounter3.setTexture(digitTextures[-digit3]);


                    }
                    else{
                        bombCounter1.setTexture(digitTextures[digit1]);
                        bombCounter2.setTexture(digitTextures[digit2]);
                        bombCounter3.setTexture(digitTextures[digit3]);
                    }


                }



            }

            window.clear(sf::Color::White);
            window.draw(face);
            window.draw(debug);
            window.draw(leaderboard);

            if(paused == false && !leaderboardVar){
                pause.setTexture(pauseTexture);
            }
            else if(paused == false && leaderboardVar){
                pause.setTexture(pauseTexture);
            }

            else{
                pause.setTexture(playTexture);
            }
            window.draw(pause);
            window.draw(winFace);
            window.draw(bombCounter1);
            window.draw(bombCounter2);
            window.draw(bombCounter3);
            window.draw(time1);
            window.draw(time2);
            window.draw(time3);
            window.draw(time4);




            //display sprites
            for (int i = 0; i < backgroundTiles.size(); ++i) {
                for (int j = 0; j < backgroundTiles[i].size(); ++j) {
                    sf::Texture backgroundTexture;
                    backgroundTexture.loadFromFile("./files/images/tile_revealed.png");

                    backgroundTiles[i][j].sprite.setTexture(backgroundTexture);
                    backgroundTiles[i][j].draw(window);
                    tiles[i][j].drawNumber(window);
                    tiles[i][j].draw(window);
                    flags[i][j].sprite.setTexture(flagTexture);
                    flags[i][j].draw(window);

                }
            }

            window.display();
        }


    }



    void GenerateLeaderBoard(int cols, int rows, sf::Sprite pause){



        sf::Font font;
        font.loadFromFile("files/font.ttf");

        sf::Texture pauseTexture;
        pauseTexture.loadFromFile("./files/images/pause.png");

        sf::Texture playTexture;
        playTexture.loadFromFile("./files/images/play.png");

        if(!paused){
            paused = true;
            pause.setTexture(playTexture);
        }

        if(win && !checked){
            int minutes = static_cast<int>(finalSeconds) / 60;
            int seconds = static_cast<int>(finalSeconds) % 60;


            int digit1_minutes = minutes / 10;
            int digit2_minutes = minutes % 10;

            int digit1_seconds = seconds / 10;
            int digit2_seconds = seconds % 10;

            string timeString = to_string(digit1_minutes) + to_string(digit2_minutes) + ":" + to_string(digit1_seconds) + to_string(digit2_seconds);
            timeString += "," + nameFinal;
            cout << timeString << endl;

            ofstream leaderboardFile("./files/leaderboard.txt", ios_base::app);
            leaderboardFile << timeString << "\n";

            checked = true;


        }

        vector<Player> leaderboardList;

        string line;
        ifstream file("./files/leaderboard.txt");
        while (getline(file, line)) {
            stringstream ss(line);
            Player player;
            getline(ss, player.time, ',');
            getline(ss, player.name);
            leaderboardList.push_back(player);
        }

        //add asterisk to display
        for(int i = 0; i < leaderboardList.size(); i++){
            if(leaderboardList.at(i).name == nameFinal){
                leaderboardList.at(i).name += "*";
            }
        }

        sort(leaderboardList.begin(), leaderboardList.end(), comparePlayers);

        int leadHeight = (rows * 16) + 50;
        int leadWidth = (cols * 16);


        //window
        sf::RenderWindow window(sf::VideoMode(leadWidth, leadHeight), "Minesweeper", sf::Style::Close);



        sf::Text title("LEADERBOARD", font, 20);
        setText(title, leadWidth/2, leadHeight/2 - 120);
        title.setFillColor(sf::Color::White);
        title.setStyle(sf::Text::Bold | sf::Text::Underlined);

        string player1String = "1.    " + leaderboardList.at(0).time + "   " + leaderboardList.at(0).name;
        sf::Text player1(player1String, font, 18);
        setText(player1, leadWidth/2, leadHeight/2 - 80);
        player1.setFillColor(sf::Color::White);
        player1.setStyle(sf::Text::Bold);

        string player2String = "2.    " + leaderboardList.at(1).time + "   " + leaderboardList.at(1).name;
        sf::Text player2(player2String, font, 18);
        setText(player2, leadWidth/2, leadHeight/2 - 40);
        player2.setFillColor(sf::Color::White);
        player2.setStyle(sf::Text::Bold);

        string player3String = "3.    " + leaderboardList.at(2).time + "   " + leaderboardList.at(2).name;
        sf::Text player3(player3String, font, 18);
        setText(player3, leadWidth/2, leadHeight/2);
        player3.setFillColor(sf::Color::White);
        player3.setStyle(sf::Text::Bold);

        string player4String = "4.    " + leaderboardList.at(3).time + "   " + leaderboardList.at(3).name;
        sf::Text player4(player4String, font, 18);
        setText(player4, leadWidth/2, leadHeight/2 + 40);
        player4.setFillColor(sf::Color::White);
        player4.setStyle(sf::Text::Bold);

        string player5String = "5.    " + leaderboardList.at(4).time + "   " + leaderboardList.at(4).name;
        sf::Text player5(player5String, font, 18);
        setText(player5, leadWidth/2, leadHeight/2 + 80);
        player5.setFillColor(sf::Color::White);
        player5.setStyle(sf::Text::Bold);



        while (window.isOpen()) {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    pause.setTexture(playTexture);
                    pause.setTexture(playTexture);
                    pause.setTexture(playTexture);

                    cout << "texture change" << endl;
                    window.close();
                }
            }



            window.clear(sf::Color::Blue);
            window.draw(player1);
            window.draw(player2);
            window.draw(player3);
            window.draw(player4);
            window.draw(player5);

            window.draw(title);
            window.display();
        }

    }


};
#endif //PROJECT3_BOARD_H
