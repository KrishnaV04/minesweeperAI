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

// constructor for BoardRep
BoardRep::BoardRep(int _rowDimension, int _colDimension, int _totalMines)
    :rowSize(_rowDimension), colSize(_colDimension), totalMines(_totalMines)
{   
    board = new Square*[rowSize];
    for (int i = 0; i < rowSize; ++i) {
        board[i] = new Square[colSize];
        for (int j = 0; j < colSize; ++j) {
            board[i][j] = COVERED;
        }
    }
    covered_squares = rowSize * colSize;
}

// destructor for BoardRep
BoardRep::~BoardRep()
{
        for (int i = 0; i < rowSize; ++i) {
            delete[] board[i];
        }
        delete[] board;
}

// Returns true only if provided row and col are within bounds.
bool BoardRep::withinBounds(int col, int row)
{
    
    return (row >= 0) && (row < rowSize) && (col >= 0) && (col < colSize);
}

// Marks the square at (row, col) as uncovered and stores its value. Returns false if square out of bounds
bool BoardRep::updateSquare(int col, int row, Square value)
{
    if (!withinBounds(col, row)) {
        return false;
    }
    if (board[row][col] < 0 && value > 0)
    {
        covered_squares -= 1;
    }
    board[row][col] = value;
    return true;
}

// Returns a pointer to the square at (row, col), returning nullptr if out of bounds
Square BoardRep::getSquare(int col, int row) 
{
    if (!withinBounds(col, row)) {
        return INVALID;
    }
    return board[row][col];
}

// returns true if isDone
bool BoardRep::isDone()
{
    return covered_squares <= totalMines;
}

// Start of myAI class, which contains core functionality
MyAI::MyAI (int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY) : Agent()
{
    boardObj = new BoardRep(_rowDimension, _colDimension, _totalMines);
    agentCoord = Coord(_agentX, _agentY);
};

MyAI::~MyAI() {
    delete boardObj;
}

Agent::Action MyAI::getAction(int number)
{
    boardObj->updateSquare(agentCoord.x, agentCoord.y, number);
    if (boardObj->isDone()) {
        return {LEAVE,-1,-1};
    }

    Coord currentCoord = Coord(agentCoord.x, agentCoord.y);
    if (number == 0) {
        add_neighbors(currentCoord, COVERED, toUncoverList);

    } else { 
        toProcessList.push_back(currentCoord);
    }

    while(!toUncoverList.empty() || !toProcessList.empty()) 
    {
        if(!toUncoverList.empty())
        {
            Coord nextCoord = toUncoverList.front();
            toUncoverList.pop_front();
            if(boardObj->getSquare(nextCoord.x, nextCoord.y) == COVERED)
            {
                agentCoord = nextCoord;
                return {UNCOVER, agentCoord.x, agentCoord.y};
            }
            
        }
        else if (!toProcessList.empty())
        {
            Coord nextCoord = toProcessList.front();
            toProcessList.pop_front();
            processCoord(nextCoord);
        }
    }

    // cout << "Exhausted Definite Cases: ";
    // cout << boardObj->covered_squares << endl;
    return {LEAVE, -1, -1}; // temporarily as exhausted rest 
}

void MyAI::processCoord(Coord nextCoord) {
    int covered_neighbors = count_neighbors(nextCoord, COVERED);
    int flagged_neighbors = count_neighbors(nextCoord, FLAGGED);
    int square_num =  boardObj->getSquare(nextCoord.x, nextCoord.y);

    if (flagged_neighbors == square_num && covered_neighbors) {
        add_neighbors(nextCoord, COVERED, toUncoverList);
    }
    else if (covered_neighbors + flagged_neighbors == square_num) {
        list<Coord> updated_coords = update_neighbors(nextCoord, COVERED, FLAGGED);
        for (Coord& c : updated_coords) {
            add_neighbors(c, NUMBERED, toProcessList);
        }
    }
}

void MyAI::add_neighbors(Coord& coord, Square type, list<Coord>& list)
{   
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            if ((i != coord.x || j != coord.y) &&
                boardObj->getSquare(i, j) != INVALID &&
                ((type == NUMBERED && boardObj->getSquare(i, j) >= 0) || 
                (boardObj->getSquare(i, j) == type)))
            {
                // cout << "added: " << i << ", " << j << endl;
                list.push_back(Coord(i, j));
            }
        }
    }
}

int MyAI::count_neighbors(Coord& coord, Square type) 
{
    int sum = 0;
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            if ((i != coord.x || j != coord.y) &&
                boardObj->getSquare(i, j) != INVALID &&
                ((type == NUMBERED && boardObj->getSquare(i, j) >= 0) || 
                (boardObj->getSquare(i, j) == type)))
            {
                ++sum;
            }
        }
    }
    return sum;
}

list<Coord> MyAI::update_neighbors(Coord& coord, Square oldtype, Square newtype)
{
    list<Coord> updated;
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            if ((i != coord.x || j != coord.y) &&
                boardObj->getSquare(i, j) != INVALID &&
                ((oldtype == NUMBERED && boardObj->getSquare(i, j) >= 0) || 
                (boardObj->getSquare(i, j) == oldtype)))
            {
                boardObj->updateSquare(i, j, newtype);
                updated.push_back(Coord(i, j));
            }
        }
    }
    return updated;
}