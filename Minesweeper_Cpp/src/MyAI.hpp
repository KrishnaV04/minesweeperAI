// ======================================================================
// FILE:        MyAI.hpp
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

#ifndef MINE_SWEEPER_CPP_SHELL_MYAI_HPP
#define MINE_SWEEPER_CPP_SHELL_MYAI_HPP

#include "Agent.hpp"
#include <iostream> // temporary use
#include <vector>
#include <map>
#include <list>
#include <set>
#include <algorithm>

#define COVERED -1
#define FLAGGED -2
#define NUMBERED -3
typedef int Square;

using namespace std;

// BoardRepresentation Class
class BoardRep 
{
public:
    // variables of BoardRep
    const int rowSize;
    const int colSize;
    const int totalMines;
    int covered_squares;
    Square** board;

    // functions in BoardRep
    BoardRep(int _rowDimension, int _colDimension, int _totalMines);
    ~BoardRep();
    bool updateSquare(int row, int col, Square value);
    Square* getSquare(int row, int col);
    bool isDone();
    bool withinBounds(int row, int col);
    void flagSquared(int row, int col);

};

struct Coord {
    int x;
    int y;

    Coord(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
};

// AI Class
class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );
    ~MyAI();
    Action getAction ( int number ) override;

    // Adds neighbors of coordinates provided which have a certain type, to the provided list.
    void add_neighbors(Coord& coord, Square type, list<Coord>& list);
    int count_neighbors(Coord& coord, Square type);
    list<Coord> update_neighbors(Coord& coord, Square oldtype, Square newtype);

    list<Coord> toUncoverList;
    list<Coord> toProcessList;
    BoardRep* boardObj;
    int agentX;
    int agentY;

};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
