#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <ctime>
#include <GL/glut.h>
#include <string>
#include <sstream>
#include <fstream>
using namespace std;

// RGB Colors Array
float colors[][3] = {
    0.95f, 0.95f, 0.95f,    // gray
    1.0f, 1.0f, 1.0f,       // white
    1.0f, 0.0f, 0.0f,       // red
    1.0f, 0.90f, 0.0f,      // yellow
    0.2f, 0.6f, 1.0f,       // blue
    0.0f, 0.8f, 0.4f,       // green
    1.0f, 0.49f, 0.0f,      // orange
    1.0f, 0.0f, 1.0f,       // magenta
};

float colors2[][3] = {
    0.95f, 0.95f, 0.95f,    // gray
    1.0f, 1.0f, 1.0f,       // white
    0.6f, 0.0f, 0.0f,       // red
    0.74f, 0.67f, 0.07f,      // yellow
    0.2f, 0.1f, 0.7f,       // blue
    0.0f, 0.5f, 0.4f,       // green
    1.0f, 0.8f, 0.0f,      // orange
    0.65f, 0.0f, 1.0f,       // magenta
};

// variables for board, shape and score
int board[10][25] = { {0} };
int curr_shape[4][2] = { { -1 } };
int currShapeType = 0;
int currShapeColor = 0;
int direction = 1;
int tempPoints[4][2];
int score = 0;
int oldScore = 0;
int highScore = 0;
string scoreStr = "Score: 0";
string highScoreStr = "High Score: 0";

// All helper functions
void drawSquare(float x, float y, float size, int cIndex );
void newShape();
void drawBoxes();
void clearMovingShape();
void moveShapeLeft();
void moveShapeRight();
void moveShapeDown();
void rotateLineShape();
void rotateLShape();
void rotateSShape();
void rotateTShape();
void rotateShape();
void rotationUpdate();
void removeCompleteLines();
void gameFinishCheck();
void displayGameName();
void displayScore();
void displayText(int x, int y, const string& str);
void displayHighScore();
void saveHighScore();
void getHighScore();

// main display function
void display() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black
    glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer

    gameFinishCheck();
    displayGameName();
    displayScore();
    displayHighScore();
    moveShapeDown();
    drawBoxes();

    glFlush();  // Render now
}

// function to handle arrow keys
void NonPrintableKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_DOWN)
        glutPostRedisplay();
    else {
        if (key == GLUT_KEY_LEFT)
            moveShapeLeft();
        else if (key == GLUT_KEY_RIGHT)
            moveShapeRight();
        else if (key == GLUT_KEY_UP)
            rotateShape();
        drawBoxes();
        glFlush();  // Render now
    }
}

// function to execute after every specified time
void Timer(int m) {
    glutPostRedisplay();
    // once again we tell the library to call our Timer function after next 800
    glutTimerFunc(800, Timer, 0);
}


// main function
int main(int argc, char** argv) {
    srand(time(NULL));
    getHighScore();
    int width = 480, height = 480;

    glutInit(&argc, argv);                 // Initialize GLUT
    glutInitDisplayMode(GLUT_RGBA);
    glutInitWindowSize(width, height);   // Set the window's initial width & height
    glutInitWindowPosition(400, 180); // Position the window's initial top-left corner
    glutCreateWindow("Tetris Game"); // Create a window with the given title

    // set viewpoint
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    newShape();
    glutDisplayFunc(display); // Register display callback handler for window re-paint
    glutSpecialFunc(NonPrintableKeys); // telling library which function to call for non-printable ASCII characters
    glutTimerFunc(5 / 5, Timer, 0);

    glutMainLoop();           // Enter the infinitely event-processing loop
    return 0;
}

// Draw Square: makes a sqaure at specified position
void drawSquare(float x, float y, float size, int cIndex ) {
    glBegin(GL_QUADS);              // 4 vertices will form a quad
    glColor3f( colors[cIndex][0], colors[cIndex][1], colors[cIndex][2]);
    glVertex2f(x + size, y + size);
    glVertex2f(x, y + size);
    glVertex2f(x, y);
    glColor3f(colors2[cIndex][0], colors2[cIndex][1], colors2[cIndex][2]);
    glVertex2f(x + size, y);
    glEnd();
}

// New Shape: creates a new random tetris shape
void newShape() {
    int randNum = rand() % 5;
    currShapeType = randNum;
    currShapeColor = rand() % 6 + 2;
    direction = 1;

    // straight line
    if (randNum == 0) {
        // x coordinates
        curr_shape[0][0] = rand() % 7;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0] + 2;
        curr_shape[3][0] = curr_shape[0][0] + 3;
        // y coordinates
        curr_shape[0][1] = curr_shape[1][1] = curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // box shape
    else if (randNum == 1) {
        // x corrdinates
        curr_shape[0][0] = rand() % 9;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0];
        curr_shape[3][0] = curr_shape[1][0];
        // y coordinates
        curr_shape[0][1] = curr_shape[1][1] = 22;
        curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // L shape
    else if (randNum == 2) {
        // x coordinates
        curr_shape[0][0] = rand() % 8;
        curr_shape[1][0] = curr_shape[0][0];
        curr_shape[2][0] = curr_shape[0][0] + 1;
        curr_shape[3][0] = curr_shape[1][0] + 2;
        // y coordinates
        curr_shape[0][1] = 22;
        curr_shape[1][1] = curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // S shape
    else if (randNum == 3) {
        //  x coordinates
        curr_shape[0][0] = rand() % 8 + 1;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0];
        curr_shape[3][0] = curr_shape[0][0] - 1;
        // y coordinates
        curr_shape[0][1] = curr_shape[1][1] = 22;
        curr_shape[2][1] = curr_shape[3][1] = 21;
    }

    // T shape
    else if (randNum == 4) {
        // x coordinates
        curr_shape[0][0] = rand() % 8;
        curr_shape[1][0] = curr_shape[0][0] + 1;
        curr_shape[2][0] = curr_shape[0][0] + 2;
        curr_shape[3][0] = curr_shape[1][0];
        // y coordinates
        curr_shape[0][1] = curr_shape[1][1] = curr_shape[2][1] = 22;
        curr_shape[3][1] = 21;
    }
}

// Clear Moving Shape: remove the old shape position data from board
void clearMovingShape() {
    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 20; b++) {
            if (board[a][b] == 1)
                board[a][b] = 0;
        }
    }
}

// Move Shape Left: move shape to left if space is available
void moveShapeLeft() {
    // check if shape can be moved to left or not
    int canMove = 1;
    for (int a = 0; a < 4; a++) {
        if (curr_shape[a][0] == 0 || (board[curr_shape[a][0] - 1][curr_shape[a][1]] > 1))
            canMove = 0;
    }
    // if shape can be moved set new coordinates for shape
    if (canMove) {
        for (int a = 0; a < 4; a++)
            board[curr_shape[a][0]][curr_shape[a][1]] = 0;
        for (int a = 0; a < 4; a++)
            curr_shape[a][0] -= 1;
    }
}

// Move Shape Right: move shape to right if position is available
void moveShapeRight() {
    // check if shape can be moved to right or not
    int canMove = 1;
    for (int a = 0; a < 4; a++) {
        if (curr_shape[a][0] == 9 || (board[curr_shape[a][0] + 1][curr_shape[a][1]] > 1))
            canMove = 0;
    }
    // if shape can be moved set new coordinates for shape
    if (canMove) {
        for (int a = 0; a < 4; a++)
            board[curr_shape[a][0]][curr_shape[a][1]] = 0;
        for (int a = 0; a < 4; a++)
            curr_shape[a][0] += 1;
    }
}

// Rotate Line Shape: rotate the shape if is is a line
void rotateLineShape() {
    if (direction == 1 || direction == 3) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[3][0] = tempPoints[0][0];
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 3;
    }
    else if (direction == 2 || direction == 4) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0] + 2;
        tempPoints[3][0] = tempPoints[0][0] + 3;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[3][1] = tempPoints[0][1];
    }
}

// Rotate L Shape: rotate the shape if it is L shape
void rotateLShape() {
    if (direction == 1) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0];
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1];
        tempPoints[2][1] = tempPoints[0][1] - 1;
        tempPoints[3][1] = tempPoints[0][1] - 2;
    }
    else if (direction == 2) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0] + 2;
        tempPoints[3][0] = tempPoints[0][0] + 2;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[0][1];
        tempPoints[3][1] = tempPoints[2][1] - 1;
    }
    else if (direction == 3) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] + 1;
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] - 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 2;
    }
    else if (direction == 4) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] - 1;
        tempPoints[1][0] = tempPoints[0][0];
        tempPoints[2][0] = tempPoints[0][0] + 1;
        tempPoints[3][0] = tempPoints[0][0] + 2;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[3][1] = tempPoints[0][1] - 1;
    }
}

// Rotate S Shape: rotate the shape if it is S shape
void rotateSShape() {
    if (direction == 1 || direction == 3) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] - 1;
        tempPoints[1][0] = tempPoints[0][0];
        tempPoints[2][0] = tempPoints[0][0] + 1;
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[0][1] - 1;
        tempPoints[3][1] = tempPoints[0][1] - 2;
    }
    else if (direction == 2 || direction == 4) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] + 1;
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] - 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1];
        tempPoints[2][1] = tempPoints[0][1] - 1;
        tempPoints[3][1] = tempPoints[2][1];
    }
}

// Rotate T Shape: rotate the shape if it is T
void rotateTShape() {
    if (direction == 1) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] + 1;
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] - 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 1;
    }
    else if (direction == 2) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] - 1;
        tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[3][1] = tempPoints[0][1] - 1;
    }
    else if (direction == 3) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[0][0] = tempPoints[0][0] - 1;
        tempPoints[1][0] = tempPoints[2][0] = tempPoints[0][0];
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[0][1] - 1;
        tempPoints[2][1] = tempPoints[0][1] - 2;
        tempPoints[3][1] = tempPoints[0][1] - 1;
    }
    else if (direction == 4) {
        // new x coordinates
        tempPoints[0][0] = curr_shape[0][0];
        tempPoints[1][0] = tempPoints[0][0] + 1;
        tempPoints[2][0] = tempPoints[0][0] + 2;
        tempPoints[3][0] = tempPoints[0][0] + 1;
        // new y coordinates
        tempPoints[0][1] = curr_shape[0][1];
        tempPoints[1][1] = tempPoints[2][1] = tempPoints[0][1];
        tempPoints[3][1] = tempPoints[0][1] - 1;
    }
}

// Remove Complete Lines: remove the rows which are completely filled
void removeCompleteLines() {
    int count = 0;
    for (int a = 0; a < 20; a++) {
        // check if the current row is completely filled 
        count = 0;
        for (int b = 0; b < 10; b++)
        {
            if ((board[b][a] != 1) && (board[b][a] > 1)) {
                count++;
            }
        }
        // if row is completely filled then remove this row
        if (count == 10) {
            for (int i = a; i < 20; i++) {
                for (int b = 0; b < 10; b++) {
                    if (board[b][i + 1] != 1)
                        board[b][i] = board[b][i + 1];
                    else
                        board[b][i] = 0;
                }
            }
            a--;
            score++;    // increase score by 1
        }
    }
}

// Draw Boxes: draws boxes according to values in board
void drawBoxes() {
    clearMovingShape();

    for (int a = 0; a < 4; a++) {
        board[curr_shape[a][0]][curr_shape[a][1]] = 1;
    }

    for (int a = 0; a < 10; a++) {
        for (int b = 0; b < 20; b++) {
            drawSquare(a * 24, b * 24, 23, board[a][b] );
            if (board[a][b] == 1) {
                drawSquare(a * 24, b * 24, 23, currShapeColor );
            }
        }
    }
}

// Rotate Shape: execute the appropriate rotate function according to shape
void rotateShape() {
    // if shape is box do nothing
    if (currShapeType == 1) {
    }
    // rotate other shapes
    else {
        if (currShapeType == 0)
            rotateLineShape();
        else if (currShapeType == 2)
            rotateLShape();
        else if (currShapeType == 3)
            rotateSShape();
        else if (currShapeType == 4)
            rotateTShape();
        rotationUpdate();
    }
}

// Move Shape Down: move shape one step down if space is available
void moveShapeDown() {
    // check if shape can be moved down or not
    int stop = 0;
    for (int a = 0; a < 4; a++) {
        if (curr_shape[a][1] == 0 || (board[curr_shape[a][0]][curr_shape[a][1] - 1] > 1))
            stop = 1;
    }
    // if shape can be moved down move it one step down
    if (stop == 0) {
        for (int a = 0; a < 4; a++) {
            curr_shape[a][1] -= 1;
        }
    }
    // else check for complete lines and make a new shape
    else {
        for (int a = 0; a < 4; a++) {
            board[curr_shape[a][0]][curr_shape[a][1]] = currShapeColor;
        }
        removeCompleteLines();
        newShape();
    }
}

// Rotation Update: rotate the shape if space is available
void rotationUpdate() {
    // check if shape can be rotated or not
    int otherShape = 0;
    int i = -1, j = -1;
    for (int a = 0; a < 4; a++) {
        i = tempPoints[a][0];
        j = tempPoints[a][1];
        if (i >= 0 && i <= 9 && j >= 0)
        {
            if (board[tempPoints[a][0]][tempPoints[a][1]] > 1)
                otherShape = 1;
        }
        else
            otherShape = 1;

    }
    // if shape can be rotated then set the new coordinates for shape
    if (otherShape == 0) {
        for (int a = 0; a < 4; a++) {
            curr_shape[a][0] = tempPoints[a][0];
            curr_shape[a][1] = tempPoints[a][1];
        }
        if (direction >= 4)
            direction = 1;
        else
            direction++;
    }
}

// Game Finish Check: check if game is finished if all rows filled up to top
void gameFinishCheck() {
    // check if rows are filled upto top
    int complete = 0;
    for (int a = 0; a < 10; a++) {
        if (board[a][19] > 1)
            complete = 1;
    }
    if (complete) {
        // if current score is greater than high score then save this score
        if (score > highScore) {
            highScore = score;
            saveHighScore();
        }
        // clear current score and board
        score = 0;
        for (int a = 0; a < 10; a++) {
            for (int b = 0; b < 25; b++) {
                board[a][b] = 0;
            }
        }
        newShape();
    }
}

// Display Game Name: display game's name on screen
void displayGameName() {
    displayText(320, 400, "Tetris");
}

// Display Score: displays current score on screen
void displayScore() {
    // if score is updated then update the score string
    if (score != oldScore) {
        oldScore = score;
        stringstream s;
        s << score;
        string tmpStr = "";
        s >> tmpStr;
        scoreStr = "Score: " + tmpStr;
    }
    displayText(300, 350, scoreStr);
}

// Display Text: display the text on specified position
void displayText(int x, int y, const string& str)
{
    GLvoid* font_style = GLUT_BITMAP_TIMES_ROMAN_24;
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos3f(x, y, 1);
    //  Draw the characters one by one
    for (int a = 0; a < str.size(); a++)
        glutBitmapCharacter(font_style, str[a]);
}

// Display High Score: displays high score on screen
void displayHighScore() {
    displayText(280, 300, highScoreStr);
}

// Save High Score: saves high score in text file
void saveHighScore() {
    fstream file;
    file.open("High_Score.txt", ios::out | ios::trunc);
    file << score;
    file.close();

    file.open("High_Score.txt");
    highScoreStr = "";
    file >> highScoreStr;
    file.close();
    highScoreStr = "High Score: " + highScoreStr;
}

// Get High Score: read high score from text file
void getHighScore() {
    fstream file;
    file.open("High_Score.txt");
    file >> highScore;
    file.close();

    file.open("High_Score.txt");
    file >> highScoreStr;
    file.close();
    highScoreStr = "High Score: " + highScoreStr;
}