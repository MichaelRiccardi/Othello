#include <GLUT/GLUT.h>
#include <iostream>
#include <cstdlib>
#include <math.h>
#include "square.h"
#include "player.h"
#include <string>
#include "AI.h"
#include <unistd.h>
#include <chrono>

/* The large amount of static variables and methods are due
 to the fact that they will not change depending on the instance
 of the Board class. This also eases the "object" communication
 between the glut methods and the Board methods and variables.*/

using namespace std;
class Board{
public:
    void initializeBoard(int argc, char** argv);
    static Square* square[8][8];
    static void SetSquare();
    static bool turn;
    static Player* player;
    static AI* ai;
    static void drawBoard(int x, int y);
    static void drawPiece(int x, int y);
    static bool isValidMove(int playerType, int x, int y);
    static void makeMove(int playerType, int x, int y);
    static int** getValidMoves(int playerType);
    static int numberOfValidMoves(int playerType);
    static int examineMove(int playerType, int x, int y);
    static int getPieces(int Player_color);
    static bool gameOver();
private:
    static int move(int playerType, int x, int y, bool makeMove);
};

Square* Board::square[8][8]; // Initializer for static variable
Player* Board::player = new Player();
AI* Board::ai = new AI();
bool Board::turn = true;

void render(); // Function called upon to draw board
void reshape(int w, int h); // Function called on for adjustments in size
void renderString(); // Draws score to board and instructions to board
void keyboard(unsigned char c, int x, int y); // Allows program to recognize keyboard inputs
void mouse(int button, int state, int x, int y); // Allows program to recognize mouse inputs

// Basic glut functions to draw board follows
void Board::initializeBoard(int argc, char** argv){
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(800, 700);
    glutCreateWindow("UF Othello");
    
    glutReshapeFunc(reshape); // Adjust window for changes in size
    glutDisplayFunc(render); // Draws components of window
    glutKeyboardFunc(keyboard); // Incorporates keyboard activity into game
    glutMouseFunc(mouse); // Incorporates mouse activity into game
    glutMainLoop(); // Reruns glut commands continuously
}

void keyboard(unsigned char c, int x, int y){
    if(c == 27){ // Exit if ESC is pressed
        exit(0);
    }
    if(c == 32){ // Restart if SPACE key is pressed
        Board::SetSquare();
        render();
    }
}

void mouse(int button, int state, int x_m, int y_m){
    int x = (x_m / 100);
    int y = floor(y_m / (700/9)) - 1; // floor function rounds down
    if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
        if(Board::turn && Board::isValidMove(Board::player->Player_color, x, y)){
            Board::player->PlayerMakeMove(x, y);
            Board::turn = false;
        }
    }
}

void render(){
    if(!(Board::gameOver())){
        glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
        for(int x = 0; x <= 7; x++){
            for(int y = 0; y <= 7; y++){
                Board::drawBoard(x,y);
                if((Board::isValidMove(Board::player->Player_color, x, y))&&(Board::turn == true)) //shows valid moves for player
                {
                    glColor3f(0,1,0); //green
                }
                Board::drawPiece(x,y);
            }
        }
        glColor3f(1, .5, 0); // Orange
        renderString();
        glutSwapBuffers();
        if(!Board::turn){
            sleep(1);
            Board::ai->makeMoveAI();
            Board::turn = true;
        }
    }
    else{
        cout << "White has " << Board::getPieces(WHITE) << " points." << endl;
        cout << "Black has " << Board::getPieces(BLACK) << " points." << endl;
        cout << "Game Over" << endl;
        sleep(2);
        exit(0);
    }
}

void reshape(int w, int h){
    if(h == 0){
        h = 1;
    }
    float aspectRatio = (float)w/h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, w, h);
    gluPerspective(45, aspectRatio, 0, 1000);
    glMatrixMode(GL_MODELVIEW);
}

void renderString(){
    int BlackPieces[2] = {Board::getPieces(BLACK) / 10 + 48, (Board::getPieces(BLACK) + 48 - ((BlackPieces[0] - 48) * 10))}; // Black score tenth digit and single digit plus 48 to get number from ASCII table
    int WhitePieces[2] = {Board::getPieces(WHITE) / 10 + 48, (Board::getPieces(WHITE) + 48 - ((WhitePieces[0] - 48) * 10))}; // Black score tenth digit and single digit plus 48 to get number from ASCII table
    char B [14]= {'B','l','a','c','k','\0','S','c','o','r','e',':',BlackPieces[0],BlackPieces[1]};
    char W [14]= {'W','h','i','t','e','\0','S','c','o','r','e',':',WhitePieces[0],WhitePieces[1]};
    char R [16]= {'S','P','A','C','E','\0','t','o','\0','R','e','s','t','a','r','t'};
    char Q [11]= {'E','S','C','\0','t','o','\0','Q','u','i','t'};
    glRasterPos2f(.5, .85);
    for(int i = 0; i < 14; i++){ // Draw Black Score
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,B[i]);
    }
    glRasterPos2f(.5, .9);
    for(int i = 0; i < 14; i++){ // Draw White Score
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,W[i]);
    }
    glRasterPos2f(-.75, .9);
    for(int i = 0; i < 16; i++){ // Instruction to Restart
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,R[i]);
    }
    glRasterPos2f(-.75, .85);
    for(int i = 0; i < 11; i++){ // Draw Instruction to Quit
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15,Q[i]);
    }
}

// initializes each square to specified coordinates on the board
void Board::SetSquare(){
    for(int x = 0; x <= 7; x++){
        for(int y = 0; y <= 7; y++){
            square[x][y] = new Square();
        }
    }
    square[3][3]->value = BLACK;
    square[3][4]->value = WHITE;
    square[4][3]->value = WHITE;
    square[4][4]->value = BLACK;
}

void Board::drawBoard(int x, int y){
    glBegin(GL_QUADS); // Draw square
    if(((x == 0 || x % 2 == 0) && (y % 2 != 0)) || ((x % 2 != 0) && (y == 0 || y % 2 == 0))){
        glColor3f(0, 0, 1); // Blue
    }
    else{
        glColor3f(1, .5, 0); // Orange
    }
    // Four corners of each square with calculations made to evenly space them out across the window
    glVertex2f(-1 + (x * .25), .78 - (y * .22));
    glVertex2f(-1 + (x * .25), .56 - (y * .22));
    glVertex2f(-.75 + (x * .25), .56 - (y * .22));
    glVertex2f(-.75 + (x * .25), .78 - (y * .22));
    glEnd();
}

void Board::drawPiece(int x, int y){
    //// Shadow
    float cxS = -.865 + (x * .25); // Center x
    float cyS = .66 - (y * .22); // Center y
    float rS = .09;
    if(square[x][y]->value == BLACK || square[x][y]->value == WHITE){
        if(((x == 0 || x % 2 == 0) && (y % 2 != 0)) || ((x % 2 != 0) && (y == 0 || y % 2 == 0))){
            glColor3f(0.137255, 0.137255, 0.556863); // Blue
        }
        else{
            glColor3f(0.52, 0.37, 0.26); // Orange
        }
        
    }
    glLineWidth(500);
    glBegin(GL_POLYGON);
    
    for (int i = 0; i < 10; i++)   {
        float theta = 2.0f * 3.1415926f * float(i) / float(10); // Get the current angle
        float x = rS * cosf(theta); // Calculate the x component
        float y = rS * sinf(theta); // Calculate the y component
        glVertex2f(x + cxS, y + cyS); // Output vertex
    }
    glEnd();
    ////
    
    
    float cx = -.875 + (x * .25); // Center x
    float cy = .67 - (y * .22); // Center y
    float r = .09;
    glPolygonMode(GL_FRONT, GL_FILL);
    if(square[x][y]->value == BLACK){ // Assign black color to piece if value == 1
        glColor3f(0, 0, 0);
    }
    else if(square[x][y]->value == WHITE){ // Assign white color to piece if value == 2
        glColor3f(1, 1, 1);
    }
    glLineWidth(500);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 10; i++)   {
        float theta = 2.0f * 3.1415926f * float(i) / float(10); // Get the current angle
        float x = r * cosf(theta); // Calculate the x component
        float y = r * sinf(theta); // Calculate the y component
        glVertex2f(x + cx, y + cy); // Output vertex
    }
    glEnd();
}


int Board::move(int playerType, int x, int y, bool makeMove)
{
    int opponentType;
    if(playerType == BLACK)
    {
        opponentType = WHITE;
    }
    else
    {
        opponentType = BLACK;
    }

    if(x >= 0 && x < 8 && y >= 0 && y < 8){
        if(square[x][y]->value != BLANK) // if square isn’t blank, this isn’t a valid move
            {
                return 0; // so return zero tiles flipped (indicating an invalid move)
        }
    }
    
    // 8 directions: left, right, up, down, up-left, up-right, down-left, down-right
    const int NUM_OF_DIRECTIONS = 8;
    int stepX [NUM_OF_DIRECTIONS] = { -1, 1, 0, 0, -1, 1, -1, 1 };
    int stepY [NUM_OF_DIRECTIONS] = { 0, 0, -1, 1, -1, -1, 1, 1 };
    //cout << Board::square[x][y]->value << endl;
    
    int numFlipped = 0;
    
    for(int h = 0; h < NUM_OF_DIRECTIONS; h++)
    {
        int i = x + stepX[h];
        int j = y + stepY[h];
        int numToFlip = 0;
        // cycle in given direction of new piece while the tiles are all opponent tiles (and not reached end of the board)
        while(i >= 0 && i < 8 && j >= 0 && j < 8 && square[i][j]->value == opponentType)
        {
            i += stepX[h];
            j += stepY[h];
            numToFlip++;
        }
        if(i != 8 && i != -1 && j != 8 && j != -1){
            if(square[i][j]->value == playerType && numToFlip != 0)
            {
                // go back and flip all of those tiles
                int i2 = x;
                int j2 = y;
                while(numToFlip >= 0)
                {
                    if(makeMove){
                        square[i2][j2]->value = playerType;
                        i2 += stepX[h];
                        j2 += stepY[h];
                    }
                    numFlipped++;
                    numToFlip--;
                }
                // if blank, take no action
            }
        }
    }
    if(makeMove && numFlipped > 0)
    {
        glutPostRedisplay(); // redraws the window after each move
    }
    return numFlipped;
}

/* Returns true if it is a valid move to place a piece of playerType at Square (x,y), false otherwise. */
bool Board::isValidMove(int playerType, int x, int y)
{
    return (move(playerType,x,y,false) > 0); // true if number of pieces moved would be greater than zero
}

/* Places a piece of playerType at Square (x,y) if it is a valid move, and flips opponent pieces accordingly. Otherwise does nothing. The Player should check that the move is valid before calling this method. */
void Board::makeMove(int playerType, int x, int y)
{
    move(playerType,x,y,true);
}

/* Returns the number of pieces that WOULD be flipped by placing a piece of playerType at Square (x,y). Does NOT actually make a move or flip any pieces. */
int Board::examineMove(int playerType, int x, int y)
{
    return move(playerType,x,y,false);
}

int Board::getPieces(int Player_color)
{
    //returns the number of pieces of a certain color, or score
    int pieces = 0;
    
    for(int x = 0; x < 8; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if(square[x][y]->value == Player_color)
            {
                pieces++;
            }
        }
    }
    
    return pieces;
}

Player::Player(){
    Player_color = 0;
}

void Player::setType(int color)
{
    Player_color = color;
}

void Player::PlayerMakeMove(int x, int y)
{
    //showValidMove();
    Board::makeMove(Player_color,x,y);
}

AI::AI()
{
    maxPoints = 0;
}

void AI::setType(int Color){
    playerType = Color;
}

void AI::makeMoveAI() // Determines the best move for the AI to make
{
    int points;
    maxPoints = 0;
    int x;
    int y;
    
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(Board::isValidMove(playerType,i,j))
            {
                x = i;
                y = j;
                
                maxValue(Board::examineMove(playerType, x, y), x, y); // Finds the max value
            }
        }
    }
    
    Board::makeMove(playerType, bestX, bestY);  // AI move
}

void AI::maxValue(int points, int x, int y)  // Compares points to the maximum value to determine maximum points at a certain position
{
    if(points > maxPoints)
    {
        maxPoints = points;
        bestX = x;
        bestY = y;
    }
}

bool Board::gameOver()
{
    bool moveLeft = true;
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            if(Board::isValidMove(WHITE , i, j))
            {
                moveLeft = false;
            }
            if(Board::isValidMove(BLACK , i, j))
            {
                moveLeft = false;
            }
        }
    }
    
    return moveLeft;
}