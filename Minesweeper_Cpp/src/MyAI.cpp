// ======================================================================
// FILE:        MyAI.cpp
//
// AUTHOR:      Jian Li
//
// DESCRIPTION: This file contains your agent class, which you will
//              implement. You are responsible for implementing the
//              'getAction' function and any helper methods you feel you
//              need.
//
// NOTES:       - If you are having trouble understanding how the shell
//                works, look at the other parts of the code, as well as
//                the documentation.
//
//              - You are only allowed to make changes to this portion of
//                the code. Any changes to other portions of the code will
//                be lost when the tournament runs your code.
// ======================================================================

#include "MyAI.hpp"

struct Square {
    bool covered = true;
    bool flagged = false;
    short number = -1;
};

class BoardRep 
{
public:
    const int rowSize;
    const int colSize;
    Square** board;

    BoardRep(int _rowDimension, int _colDimension);
    ~BoardRep() {
        for (int i = 0; i < rowSize; ++i) {
            delete[] board[i];
        }
        delete[] board;
    }
    bool updateSquare(int row, int col, int value);
    Square* getSquare(int row, int col);


private:
    bool withinBounds(int row, int col);
};

BoardRep::BoardRep(int _rowDimension, int _colDimension)
    : rowSize(_rowDimension), colSize(_colDimension)
{
    board = new Square*[rowSize];
    Square* column;
    for (int i = 0; i < rowSize; ++i) {
        column = new Square[colSize];
        board[i] = column;
    }
}

bool BoardRep::withinBounds(int row, int col)
{
    return (row >= 0) && (row < rowSize) && (col >= 0) && (col < colSize);
}

// Marks the square at (row, col) as uncovered and stores its value. Returns false if square out of bounds
bool BoardRep::updateSquare(int row, int col, int value)
{
    if (!withinBounds(row, col)) {
        return false;
    }
    board[row][col].number = value;
    board[row][col].covered = false;
    return true;
}

// Returns a pointer to the square at (row, col), returning nullptr if out of bounds
Square* BoardRep::getSquare(int row, int col) 
{
    if (!withinBounds(row, col)) {
        return nullptr;
    }
    return &board[row][col];
}

/* Hi Krishna! Everything above this comment until the #include was added by me.
// As a hint, _agentX and _agentY seem to indicate the board position of the first uncovered square.
// Using updateSquare() on the board with the number passed as an argument to getAction(number) should
// put the first number on the board, giving you the information that you'll need to choose basic actions
*/ 

MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{

};

Agent::Action MyAI::getAction( int number )
{

    return {LEAVE,-1,-1};
}