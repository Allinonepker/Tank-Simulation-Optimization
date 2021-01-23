#include "precomp.h"

void Quad::insert(Tank* tank)
{
    if (tank == NULL)
        return;

    // Current quad cannot contain it 
    if (!inBoundary(tank->position))
        return;

    // We are at a quad of unit area 
    // We cannot subdivide this quad further 
    if (abs(topLeft.x - botRight.x) <= 1 &&
        abs(topLeft.y - botRight.y) <= 1)
    {
        if (tank = NULL)
            //n = node;
        return;
    }

    if ((topLeft.x + botRight.x) / 2 >= tank->position.x)
    {
        // Indicates topLeftTree 
        if ((topLeft.y + botRight.y) / 2 >= tank->position.y)
        {
            if (topLeftTree == NULL)
                topLeftTree = new Quad(
                    Point(topLeft.x, topLeft.y),
                    Point((topLeft.x + botRight.x) / 2,
                        (topLeft.y + botRight.y) / 2));
            topLeftTree->insert(tank);
        }

        // Indicates botLeftTree 
        else
        {
            if (botLeftTree == NULL)
                botLeftTree = new Quad(
                    Point(topLeft.x,
                        (topLeft.y + botRight.y) / 2),
                    Point((topLeft.x + botRight.x) / 2,
                        botRight.y));
            botLeftTree->insert(tank);
        }
    }
    else
    {
        // Indicates topRightTree 
        if ((topLeft.y + botRight.y) / 2 >= node->pos.y)
        {
            if (topRightTree == NULL)
                topRightTree = new Quad(
                    Point((topLeft.x + botRight.x) / 2,
                        topLeft.y),
                    Point(botRight.x,
                        (topLeft.y + botRight.y) / 2));
            topRightTree->insert(node);
        }

        // Indicates botRightTree 
        else
        {
            if (botRightTree == NULL)
                botRightTree = new Quad(
                    Point((topLeft.x + botRight.x) / 2,
                        (topLeft.y + botRight.y) / 2),
                    Point(botRight.x, botRight.y));
            botRightTree->insert(node);
        }
    }
}

// Find a node in a quadtree 
Node* Quad::search(Point p)
{
    // Current quad cannot contain it 
    if (!inBoundary(p))
        return NULL;

    // We are at a quad of unit length 
    // We cannot subdivide this quad further 
    if (n != NULL)
        return n;

    if ((topLeft.x + botRight.x) / 2 >= p.x)
    {
        // Indicates topLeftTree 
        if ((topLeft.y + botRight.y) / 2 >= p.y)
        {
            if (topLeftTree == NULL)
                return NULL;
            return topLeftTree->search(p);
        }

        // Indicates botLeftTree 
        else
        {
            if (botLeftTree == NULL)
                return NULL;
            return botLeftTree->search(p);
        }
    }
    else
    {
        // Indicates topRightTree 
        if ((topLeft.y + botRight.y) / 2 >= p.y)
        {
            if (topRightTree == NULL)
                return NULL;
            return topRightTree->search(p);
        }

        // Indicates botRightTree 
        else
        {
            if (botRightTree == NULL)
                return NULL;
            return botRightTree->search(p);
        }
    }
};

// Check if current quadtree contains the point 
bool Quad::inBoundary(vec2 position)
{
    return (position.x >= topLeft.x &&
        position.x <= botRight.x &&
        position.y >= topLeft.y &&
        position.y <= botRight.y);
}
