// C++ Implementation of Quad Tree 
#include <iostream> 
#include <cmath> 
using namespace std;

// The main quadtree class 
class Quad
{
    // Hold details of the boundary of this node 
    pair<int, int> topLeft;
    pair<int, int> botRight;

    // Children of this tree 
    Quad* topLeftTree;
    Quad* topRightTree;
    Quad* botLeftTree;
    Quad* botRightTree;

public:
    Quad()
    {
        topLeft = make_pair(0, 720);
        botRight = make_pair(1280, 0);
        topLeftTree = NULL;
        topRightTree = NULL;
        botLeftTree = NULL;
        botRightTree = NULL;
    }
    Quad(pair<int, int> topL, pair<int, int> botR)
    {
        n = NULL;
        topLeftTree = NULL;
        topRightTree = NULL;
        botLeftTree = NULL;
        botRightTree = NULL;
        topLeft = topL;
        botRight = botR;
    }
    void insert(Tank* tank);
    Tank* search(Tank* tank);
    bool inBoundary(vec2 position);
};