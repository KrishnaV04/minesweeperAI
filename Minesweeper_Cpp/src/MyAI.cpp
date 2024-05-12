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
    Square* column;
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

    if (board[row][col] < 0)
    {
        covered_squares -= 1;
    }
    board[row][col] = value;
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

bool BoardRep::isDone()
{
    return covered_squares <= totalMines;
}

// Start of myAI class, which contains core functionality
MyAI::MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY ) : Agent()
{
    BoardRep* boardObj = new BoardRep(_rowDimension, _colDimension, _totalMines);
    agentX = _agentX;
    agentY = _agentY;
};

MyAI::~MyAI() {
    delete boardObj;
}

Agent::Action MyAI::getAction(int number)
{
    boardObj->updateSquare(agentX, agentY, number);

    if (boardObj->isDone()) {
        return {LEAVE,-1,-1};
    }

    Coord currentCoord = Coord(agentX, agentY);

    if (number == 0)
    {
        add_neighbors(currentCoord, COVERED, toUncoverList);

    } else if (number == 1)
    {
        toProcessList.push_back(currentCoord);
        
    } else {
        return {LEAVE,-1,-1}; // temporarily
    }

    while(!toUncoverList.empty() || !toProcessList.empty()) 
    {
        if(!toUncoverList.empty())
        {
            
            Coord nextCoord = toUncoverList.front();
            toUncoverList.pop_front();
            agentX = nextCoord.x;
            agentY = nextCoord.y;
            return {UNCOVER, agentX, agentY};
        }
        else if (!toProcessList.empty())
        {
            Coord nextCoord = toProcessList.front();
            toProcessList.pop_front();
            int covered_neighbors = count_neighbors(nextCoord, COVERED);
            int flagged_neighbors = count_neighbors(nextCoord, FLAGGED);
            int square_num =  *boardObj->getSquare(nextCoord.x, nextCoord.y);

            // process actions
            if (covered_neighbors == 0) {
                continue; // next loop as no action can be done
            }
            else if (covered_neighbors + flagged_neighbors == square_num) {
                list<Coord> updated_coords = update_neighbors(nextCoord, COVERED, FLAGGED);
                for (Coord& c : updated_coords) {
                    add_neighbors(c, NUMBERED, toProcessList);
                }
            }
            else if (flagged_neighbors == square_num) {
                add_neighbors(nextCoord, COVERED, toUncoverList);
            }
        }
    }

    cout << "Exhausted Definite Cases" << endl;
    return {LEAVE, -1, -1}; // temporarily as exhausted rest 
}


void MyAI::add_neighbors(Coord& coord, Square type, list<Coord>& list)
{   
    // very ugly :(
    if(type == NUMBERED) {
        for(int i = coord.x-1; i <= coord.x+1; ++i) {
            for(int j = coord.y-1; j <= coord.y+1; --j) {
                if (boardObj->getSquare(i, j) && *boardObj->getSquare(i, j) >= 0) {
                    list.push_back(Coord(i, j));
                }
            }
        }

    } else {
        for(int i = coord.x-1; i <= coord.x+1; ++i) {
            for(int j = coord.y-1; j <= coord.y+1; --j) {
                if (boardObj->getSquare(i, j) && *boardObj->getSquare(i, j) == type) {
                    list.push_back(Coord(i, j));
                }
            }
        }
    }
}
