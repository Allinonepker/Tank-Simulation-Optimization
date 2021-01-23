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

    // Contains details of node 
    Node* n;

    // Children of this tree 
    Quad* topLeftTree;
    Quad* topRightTree;
    Quad* botLeftTree;
    Quad* botRightTree;

public:
    Quad()
    {
        topLeft = Point(0, 0);
        botRight = Point(0, 0);
        n = NULL;
        topLeftTree = NULL;
        topRightTree = NULL;
        botLeftTree = NULL;
        botRightTree = NULL;
    }
    Quad(Point topL, Point botR)
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