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
#include <iostream>
#include <vector>
#include <map>
#include <list> // TODO for an opt try to remove lists and only use vectors and queues
#include <set>
#include <algorithm>
#include <unordered_set>

#define COVERED -1
#define FLAGGED -2
#define NUMBERED -3
#define INVALID -4
typedef int Square;

using namespace std;

struct Coord {
    int x;
    int y;
    Coord(int xCoord, int yCoord) : x(xCoord), y(yCoord) {}
    string toString() const;
};

// BoardRepresentation Class
class BoardRep 
{
public:
    // variables of BoardRep
    const int rowSize;
    const int colSize;
    const int totalMines;
    int covered_sq_count;
    Square** board;
    list<Coord> all_covered_coords;

    // functions in BoardRep
    BoardRep(int _rowDimension, int _colDimension, int _totalMines);
    ~BoardRep();
    bool updateSquare(int col, int row, Square value);
    Square getSquare(int col, int row);
    bool isDone();
    bool withinBounds(int col, int row);
};

enum gameTile{
    NONE,
    BOMB, 
    SAFE
};

// AI Class
class MyAI : public Agent
{
public:
    MyAI ( int _rowDimension, int _colDimension, int _totalMines, int _agentX, int _agentY );
    ~MyAI();
    Action getAction ( int number ) override;

    void process_uncovered_coord(Coord& coord, int number);
    void add_neighbors(const Coord& coord, Square type, vector<Coord>& list);
    int count_neighbors(Coord& coord, Square type);
    vector<Coord> update_neighbors(Coord& coord, Square oldtype, Square newtype);

    void singlePointProcess(Coord& nextCoord);
    
    void enumerateFrontierStrategy();
    void process_recursive_mappings(vector<pair<Coord, gameTile>>& coord_mapping, int index, gameTile value);
    void add_consistent_mappings();

    vector<map<Coord, gameTile>> all_possible_mappings;
    list<Coord> frontier_covered_coords;
    
    vector<Coord> toUncoverVector;
    // unordered_set<Coord> toProcessSet;
    vector<Coord> toProcessVector;
    
    bool justPerfromedEnumeration = false;
    BoardRep* boardObj;
    Coord agentCoord = Coord(0,0);
    


};

#endif //MINE_SWEEPER_CPP_SHELL_MYAI_HPP
