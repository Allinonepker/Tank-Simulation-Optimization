// C++ Implementation of Quad Tree 
#include <iostream> 
#include <cmath> 
using namespace std;

// Used to hold details of a point 
struct Point
{
    int x;
    int y;
    Point(int _x, int _y)
    {
        x = _x;
        y = _y;
    }
    Point()
    {
        x = 0;
        y = 0;
    }
};

// The objects that we want stored in the quadtree 
struct Node
{
    Point pos;
    int data;
    Node(Point _pos, int _data)
    {
        pos = _pos;
        data = _data;
    }
    Node()
    {
        data = 0;
    }
};

// The main quadtree class 
class Quad
{
    // Hold details of the boundary of this node 
    Point topLeft;
    Point botRight;

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

// Insert a node into the quadtree 

// Driver program 
int main()
{
    Quad center(Point(0, 0), Point(8, 8));
    Node a(Point(1, 1), 1);
    Node b(Point(2, 5), 2);
    Node c(Point(7, 6), 3);
    center.insert(&a);
    center.insert(&b);
    center.insert(&c);
    cout << "Node a: " <<
        center.search(Point(1, 1))->data << "\n";
    cout << "Node b: " <<
        center.search(Point(2, 5))->data << "\n";
    cout << "Node c: " <<
        center.search(Point(7, 6))->data << "\n";
    cout << "Non-existing node: "
        << center.search(Point(5, 5));
    return 0;
}