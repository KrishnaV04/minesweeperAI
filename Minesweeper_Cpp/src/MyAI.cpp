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

string Coord::toString() const {
        return "(" + std::to_string(x+1) + ", " + std::to_string(y+1) + ")";
}

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
    covered_sq_count = rowSize * colSize;
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
    // checks to make sure board had a covered square and then is getting uncovered
    else if (board[row][col] == COVERED && value >= 0) {
        covered_sq_count -= 1;
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
    return covered_sq_count <= totalMines;
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
    //1: Process Uncovered Coord
    process_uncovered_coord(agentCoord, number);

    //2: Check if Board is Complete
    if (boardObj->isDone()) {
        return {LEAVE,-1,-1};
    }

    //3: SimglePointProcess Strategy Implementation
    while(!toUncoverVector.empty() || !toProcessVector.empty()) 
    {
        if(!toUncoverVector.empty())
        {
            Coord nextCoord = toUncoverVector.back();
            toUncoverVector.pop_back();
            if(boardObj->getSquare(nextCoord.x, nextCoord.y) == COVERED)
            {
                agentCoord = nextCoord;
                return {UNCOVER, agentCoord.x, agentCoord.y};
            }
            
        }
        else if (!toProcessVector.empty())
        {
            Coord nextCoord = toProcessVector.back();
            toProcessVector.pop_back();
            toProcessSet.erase(nextCoord);
            singlePointProcess(nextCoord);
        }
    }

    //4: Local Model Checking Strategy 

    //5: Best Probability Strategy
    return {LEAVE, -1, -1}; // temporarily as not implemented best prob strategy
}

void MyAI::process_uncovered_coord(Coord& coord, int number) {
    boardObj->updateSquare(coord.x, coord.y, number);
    
    if (number == 0) {
        add_neighbors(coord, COVERED, toUncoverVector);

    } else {
        if (toProcessSet.find(coord) != toProcessSet.end())
        {
            toProcessVector.push_back(coord);
            toProcessSet.insert(coord);
        }
        add_neighbors(coord, NUMBERED, toProcessVector);
    }
}

void MyAI::singlePointProcess(Coord& nextCoord) {
    int covered_neighbors = count_neighbors(nextCoord, COVERED);
    int flagged_neighbors = count_neighbors(nextCoord, FLAGGED);
    int square_num =  boardObj->getSquare(nextCoord.x, nextCoord.y);

    if (flagged_neighbors == square_num && covered_neighbors) {
        add_neighbors(nextCoord, COVERED, toUncoverVector);
    }
    else if (covered_neighbors + flagged_neighbors == square_num) {
        vector<Coord> updated_coords = update_neighbors(nextCoord, COVERED, FLAGGED);
        for (Coord& c : updated_coords) {
            add_neighbors(c, NUMBERED, toProcessVector);
        }
    }
}

void MyAI::add_neighbors(Coord& coord, Square type, vector<Coord>& list)
{   
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            if ((i != coord.x || j != coord.y) &&
                boardObj->getSquare(i, j) != INVALID &&
                ((type == NUMBERED && boardObj->getSquare(i, j) >= 0) || 
                (boardObj->getSquare(i, j) == type)))
            {
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

vector<Coord> MyAI::update_neighbors(Coord& coord, Square oldtype, Square newtype)
{
    vector<Coord> updated;
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            if ((i != coord.x || j != coord.y) &&
                boardObj->getSquare(i, j) != INVALID &&
                (boardObj->getSquare(i, j) == oldtype))
            {
                boardObj->updateSquare(i, j, newtype);
                updated.push_back(Coord(i, j));
            }
        }
    }
    return updated;
}