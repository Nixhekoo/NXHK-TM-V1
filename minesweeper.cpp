#include <iostream>
#include "NXHK\NXHKSYSTEM.hpp"
#include <cmath>
#include <vector>
#include <windows.h>
#include <ctime>
#include <conio.h>

// Variables
const int GridWidth = 20;
const int GridHeight = 10;
int MouseHeight = 0;
int MouseWidth = 0;
int BombAmmount = (GridWidth * GridHeight) / 100; // The more, the easier it is
int Grid[GridHeight][GridWidth];
char Map[GridHeight][GridWidth];
int Flags[GridHeight][GridWidth];
bool Alive = true;
// Grid
int GridWall = 2;
int GridBomb = 1;
int GridSpace = 0;
// Map
char MapWall = '#';
char MapBomb = 'X';
char MapSpace = '-';

// Initializes grid and creates bombs
void initializeGame(){
    for(int h = 0; h < GridHeight; h++){
        for(int w = 0; w < GridWidth; w++){
            Grid[h][w] = GridSpace;  // Initialize all cells to GridSpace
            Map[h][w] = MapWall;
            Flags[h][w] = 0;
        }
    }

    // Create Bombs
    int BombsOnMap = 0;
    while(BombsOnMap != BombAmmount){
        int BombPlacerHeight = rand() % GridHeight;
        int BombPlacerWidth = rand() % GridWidth;
        if (Grid[BombPlacerHeight][BombPlacerWidth] != GridBomb) {
            Grid[BombPlacerHeight][BombPlacerWidth] = GridBomb;  // Place bombs
            BombsOnMap++;
        }
    }
}

// Checks for bombs in nearby cells
int checkForBombs(int HEIGHT, int WIDTH) {
    int BombCounter = 0;
    for (int dh = -1; dh <= 1; ++dh) {
        for (int dw = -1; dw <= 1; ++dw) {
            int nh = HEIGHT + dh;
            int nw = WIDTH + dw;
            if (nh >= 0 && nh < GridHeight && nw >= 0 && nw < GridWidth) {
                if (Grid[nh][nw] == GridBomb) {
                    ++BombCounter;
                }
            }
        }
    }
    return BombCounter;
}

// Check for victory
bool victoryQuestion(){
    int openedCells = 0;
    for(int h = 0; h < GridHeight; h++){
        for(int w = 0; w < GridWidth; w++){
            if(Map[h][w] != MapWall && Map[h][w] != MapBomb){
                openedCells++;
            }
        }
    }
    return openedCells == (GridWidth * GridHeight) - BombAmmount;
}

// Recursively reveals empty cells and their neighbors
void revealCells(int HEIGHT, int WIDTH) {
    // Base condition to stop recursion if cell is already revealed or flagged
    if (Map[HEIGHT][WIDTH] != MapWall || Flags[HEIGHT][WIDTH] == 1) {
        return;
    }

    int BombsAmmount = checkForBombs(HEIGHT, WIDTH);
    if (BombsAmmount == 0) {
        Map[HEIGHT][WIDTH] = MapSpace;  // Reveal the cell

        // Recursively reveal all neighboring cells
        for (int dh = -1; dh <= 1; ++dh) {
            for (int dw = -1; dw <= 1; ++dw) {
                int nh = HEIGHT + dh;
                int nw = WIDTH + dw;
                if (nh >= 0 && nh < GridHeight && nw >= 0 && nw < GridWidth) {
                    revealCells(nh, nw);
                }
            }
        }
    } else {
        Map[HEIGHT][WIDTH] = '0' + BombsAmmount;  // Reveal the cell with bomb count
    }
}

// Updates Map
void updateMap(int HEIGHT, int WIDTH) {
    if (Grid[HEIGHT][WIDTH] == GridBomb) {
        Alive = false;  // Player hit a bomb
    } else {
        revealCells(HEIGHT, WIDTH);  // Reveal the cell and its neighbors
    }
}

// Displays and Colors the Map
void displayMap(){
    // Loops through Pixels on Map
    for(int h = 0; h < GridHeight; h++){
        for(int w = 0; w < GridWidth; w++){
            // Sets colors to specific icons on map
            if(Flags[h][w] == 0){
                if(Grid[h][w] == 0){
                    NXHK_SET_CONSOLE_COLOR(8);
                }
                if(Map[h][w] == MapWall){
                    NXHK_SET_CONSOLE_COLOR(3);
                }
                if(h == MouseHeight && w == MouseWidth){
                    if(Map[h][w] == MapWall){
                        NXHK_SET_CONSOLE_COLOR(1);
                    } else {
                        NXHK_SET_CONSOLE_COLOR(7);
                    }
                }
            } else {
                if(h == MouseHeight && w == MouseWidth){
                    NXHK_SET_CONSOLE_COLOR(BACKGROUND_GREEN | 7);
                }
            }
            std::cout << Map[h][w];
            NXHK_SET_CONSOLE_COLOR(BACKGROUND_GREEN | 8);
        }
        if(h != GridHeight - 1){
            std::cout << std::endl;
        }
    }
}

int main(){
    srand(time(NULL));
    initializeGame();
    while(Alive){
        NXHK_RESET_CONSOLE_CURSOR_POSITION();
        MouseWidth = round((NXHK_GET_MOUSE_POSITION_X() * GridWidth) / NXHK_GET_SCREEN_SIZE_X());
        MouseHeight = round((NXHK_GET_MOUSE_POSITION_Y() * GridHeight) / NXHK_GET_SCREEN_SIZE_Y());
        if(_kbhit()){
            char KeyPress = _getch();
            if(KeyPress == 'q'){
                if(Flags[MouseHeight][MouseWidth] == 1){
                    Flags[MouseHeight][MouseWidth] = 0;
                } else {
                    Flags[MouseHeight][MouseWidth] = 1;
                }
            } else if(KeyPress == 'e'){
                updateMap(MouseHeight, MouseWidth);
            }
        }
        displayMap();
        if(victoryQuestion()){
            Alive = false;
            std::cout << "YOU WON!" << std::endl;
        }
    }
    std::cout << "Game Over!" << std::endl;
    return 0;
}
