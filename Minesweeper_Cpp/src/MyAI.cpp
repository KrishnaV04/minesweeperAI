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

int interleave_bits(int x, int y) {
    int z = 0;
    for (int i = 0; i < sizeof(int) * 4; ++i) {
        z |= (x & (1 << i)) << i | (y & (1 << i)) << (i + 1);
    }
    return z;
}

bool Coord::operator<(const Coord& other) const {
        return interleave_bits(x, y) < interleave_bits(other.x, other.y);
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

// Returns True if the coordinate is uncovered
bool BoardRep::isUncovered(Coord co) 
{
    return getSquare(co.x, co.y) >= 0;
}

// Returns True if both are covered or both are uncovered. Bounds checking is applied to b
bool BoardRep::matchingStatus(Coord a, Coord b)
{
    return isUncovered(a) == isUncovered(b);
}

// Returns a list of all adjacent Coordinates of opposite status
list<Coord> BoardRep::listOppositeNeighbors(Coord coord)
{
    list<Coord> opposite_neighbors;
    Coord neighbor {0, 0};
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            neighbor = Coord{i, j};
            if ((i != coord.x || j != coord.y) &&
                withinBounds(i, j) &&
                !matchingStatus(coord, neighbor))
            {
                opposite_neighbors.push_back(neighbor);
            }
        }
    }
    return opposite_neighbors;
}

// Returns a list of all adjacent Coordinates of opposite status
list<Coord> BoardRep::listMatchingNeighbors(Coord coord)
{
    list<Coord> matching_neighbors;
    Coord neighbor {0, 0};
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            neighbor = Coord{i, j};
            if ((i != coord.x || j != coord.y) &&
                withinBounds(i, j) &&
                matchingStatus(coord, neighbor))
            {
                matching_neighbors.push_back(neighbor);
            }
        }
    }
    return matching_neighbors;
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

void printCoordSet(std::set<Coord> s) {
    cout << "{ ";
    for (auto elem : s) {
        cout << "(" << elem.x + 1 << ", " << elem.y + 1 << "), ";
    }
    cout << " }" << std::endl;
}

Agent::Action MyAI::getAction(int number)
{   
    //1: Process Uncovered Coord
    process_uncovered_coord(agentCoord, number);

    //2: Check if Board is Complete
    if (boardObj->isDone()) {
        return {LEAVE,-1,-1};
    }

    while(!toUncoverVector.empty() || !toProcessVector.empty() || !justPerformedEnumeration) 
    {
        //3a: SimglePointProcess Strategy Implementation
        if(!toUncoverVector.empty())
        {
            Coord nextCoord = toUncoverVector.back();
            toUncoverVector.pop_back();
            if(boardObj->getSquare(nextCoord.x, nextCoord.y) == COVERED)
            {
                agentCoord = nextCoord;
                return {UNCOVER, agentCoord.x, agentCoord.y};
            }
            justPerformedEnumeration = false; // needed for #4
        }

        //3b: SimglePointProcess Strategy Implementation
        else if (!toProcessVector.empty())
        {
            Coord nextCoord = toProcessVector.back();
            toProcessVector.pop_back();
            singlePointProcess(nextCoord);

            justPerformedEnumeration = false; // needed for #4
        }

        //4: Enumerate Frontier Checking Strategy 
        else if (!justPerformedEnumeration)
        {
            if(boardObj->frontier_covered.size()) {
                enumerateFrontierStrategy();
            }
            // TODO frontier covered shouldn't have flagged mines
            //printCoordSet(boardObj->frontier_covered);
            justPerformedEnumeration = true;
        }
    }

    //5: Best Probability Strategy
    // if (boardObj->frontier_covered.size()) {
    //     Coord c = *boardObj->frontier_covered.begin();
    //     agentCoord = c;
    //     return {UNCOVER, c.x, c.y};
    // }

    // TODO STRATEGIES
    // - smart probability with frontier
    // - random with covered
    // - time strategy, look at documentation

    // TODO OPTIMIZATIONS
    // - ghost bug
    // - optimizations on covered_frontier !
    // - optimizations in enumeration !
    // - minimal optimizations

    return {LEAVE, -1, -1}; // temporarily as not implemented best prob strategy
}

void MyAI::enumerateFrontierStrategy() {
    vector<pair<Coord, gameTile>> covered_frontier_enumerate;
    for (const auto& coord : boardObj->frontier_covered) {
        covered_frontier_enumerate.emplace_back(coord, NONE);
    }
    for(const auto& p : covered_frontier_enumerate)
        boardObj->updateSquare(p.first.x, p.first.y, UNDEFINED);
    process_recursive_mappings(covered_frontier_enumerate, 0, BOMB);
    process_recursive_mappings(covered_frontier_enumerate, 0, SAFE);
    for(const auto& p : covered_frontier_enumerate)
        boardObj->updateSquare(p.first.x, p.first.y, COVERED);
    add_consistent_mappings();
}

void MyAI::process_recursive_mappings(vector<pair<Coord, gameTile>>& vector_to_enumerate, int index, gameTile value) {
    Coord& c = vector_to_enumerate[index].first;

    vector_to_enumerate[index].second = value;
    if (value == BOMB)
        boardObj->updateSquare(c.x, c.y, FLAGGED);
    else
        boardObj->updateSquare(c.x, c.y, COVERED);
    
    if (check_constraints(c)) {
        if (index == vector_to_enumerate.size() - 1) {
            all_possible_mappings.push_back(vector_to_enumerate);
        } else {
            process_recursive_mappings(vector_to_enumerate, index+1, BOMB);
            process_recursive_mappings(vector_to_enumerate, index+1, SAFE);
        }
    }
    vector_to_enumerate[index].second = NONE;
    boardObj->updateSquare(c.x, c.y, UNDEFINED);
}

bool MyAI::check_constraints(Coord& c) {
    vector<Coord> neighbors;
    int flagged_neighbors;
    int undefined_neighbors;
    int square_num;
    get_neighbors(c, NUMBERED, neighbors);
    for (const auto& coord : neighbors) {
        flagged_neighbors = count_neighbors(coord, FLAGGED);
        undefined_neighbors = count_neighbors(coord, UNDEFINED);
        square_num =  boardObj->getSquare(coord.x, coord.y);
        if (square_num > flagged_neighbors + undefined_neighbors ||
            flagged_neighbors > square_num || 
            (!undefined_neighbors && flagged_neighbors != square_num)
            )
            return false;
    }
    return true;
}

void MyAI::add_consistent_mappings() {
    vector<pair<Coord, gameTile>> cmap;

    // TODO weird but all_possible_mappings gets deleted after priting, else it doesn't
    // TODO combining consistent mappings doesn't seem to work

    // populate cmap    
    if(all_possible_mappings.size() == 0) {
        return;
    } else if (all_possible_mappings.size() == 1){
        cmap = all_possible_mappings[0];
    } else {
        const vector<pair<Coord, gameTile>>& first_map = all_possible_mappings[0];
        for(int i=0; i < first_map.size(); ++i)
        {
            bool all_equal = true;
            for(int j=1; j < all_possible_mappings.size(); ++j) {
                if (first_map[i].second != all_possible_mappings[j][i].second) {
                    all_equal = false;
                    break;
                }
            }
            if (all_equal) {
                cmap.push_back(first_map[i]);
            }
        }
    }

    // for consistent coords take following action
    for (auto& pair : cmap) {
    if(pair.second == BOMB){
            boardObj->updateSquare(pair.first.x, pair.first.y, FLAGGED);
            boardObj->frontier_covered.erase(pair.first);
            add_neighbors(pair.first, NUMBERED, toProcessVector);
        } else if (pair.second == SAFE){
            toUncoverVector.push_back(Coord{pair.first.x, pair.first.y});
        }
    }

    all_possible_mappings.clear();
}

void MyAI::process_uncovered_coord(Coord& coord, int number) {
    boardObj->updateSquare(coord.x, coord.y, number);

    // Changes 5/18: Updating the covered and uncovered frontiers
    boardObj->frontier_covered.erase(coord); // 1. Remove the uncovered coord from the covered frontier, if it exists
    vector<Coord> opposite_neighbors;
    get_neighbors(coord, COVERED, opposite_neighbors);
    //list<Coord> opposite_neighbors = boardObj->listOppositeNeighbors(coord); // 2. Find the covered neighbors of coord
    list<Coord> matching_neighbors = boardObj->listMatchingNeighbors(coord); // 2.5 Find the uncovered neighbors of coord
    list<Coord> coord2;
    if (opposite_neighbors.size()) { // 3. If the uncovered coord has covered neighbors, then it now belongs in the uncovered frontier
        // boardObj->frontier_uncovered.insert(coord);
        for (Coord covered_adj: opposite_neighbors) { // 4. Enter all the covered neighbors into the covered frontier
            boardObj->frontier_covered.insert(covered_adj);
        }
    }
    // for (Coord uncovered_adj : matching_neighbors) { // 5. Remove any uncovered neighbors of coord if they leave the uncovered frontier
    //     coord2 = boardObj->listOppositeNeighbors(uncovered_adj);
    //     if (!coord2.size()) {
    //         boardObj->frontier_uncovered.erase(uncovered_adj);
    //     }
    // }
    //printCoordSet(boardObj->frontier_uncovered); // Debug lines that show the frontiers after the prior iteration
    //printCoordSet(boardObj->frontier_covered);
    // end of changes 5/18
    
    if (number == 0) {
        add_neighbors(coord, COVERED, toUncoverVector);

    } else {
        toProcessVector.push_back(coord);
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
            boardObj->frontier_covered.erase(c);
        }
    }
}

void MyAI::add_neighbors(const Coord& coord, Square type, vector<Coord>& list)
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

int MyAI::count_neighbors(const Coord& coord, Square type) 
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

void MyAI::get_neighbors(Coord& coord, Square type, vector<Coord>& vector)
{
    for(int i = coord.x-1; i <= coord.x+1; ++i) {
        for(int j = coord.y-1; j <= coord.y+1; ++j) {
            if ((i != coord.x || j != coord.y) &&
                boardObj->getSquare(i, j) != INVALID &&
                ((boardObj->getSquare(i, j) == type) ||
                (type == NUMBERED && (boardObj->getSquare(i, j) >= 0)))
                )
            {
                vector.push_back(Coord(i, j));
            }
        }
    }
}