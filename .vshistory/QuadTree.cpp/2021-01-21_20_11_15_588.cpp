#include "precomp.h"

void Quad::insert(Tank* tank)
{
    if (tank == NULL)
        return;

    // Current quad cannot contain it 
    if (!inBoundary(tank->position))
        return;

    if ((topLeft.first + botRight.first) / 2 >= tank->position.x)
    {
        // Indicates topLeftTree 
        if ((topLeft.second + botRight.second) / 2 >= tank->position.y)
        {
            if (topLeftTree == NULL)
                topLeftTree = new Quad(
                    make_pair(topLeft.first, topLeft.second),
                    make_pair((topLeft.first + botRight.first) / 2,
                        (topLeft.second + botRight.second) / 2));
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
        if ((topLeft.y + botRight.y) / 2 >= tank->position.y)
        {
            if (topRightTree == NULL)
                topRightTree = new Quad(
                    Point((topLeft.x + botRight.x) / 2,
                        topLeft.y),
                    Point(botRight.x,
                        (topLeft.y + botRight.y) / 2));
            topRightTree->insert(tank);
        }

        // Indicates botRightTree 
        else
        {
            if (botRightTree == NULL)
                botRightTree = new Quad(
                    Point((topLeft.x + botRight.x) / 2,
                        (topLeft.y + botRight.y) / 2),
                    Point(botRight.x, botRight.y));
            botRightTree->insert(tank);
        }
    }
}

// Find a node in a quadtree 
Tank* Quad::search(Tank* tank)
{
    // Current quad cannot contain it 
    if (!inBoundary(tank->position))
        return NULL;

    // We are at a quad of unit length 
    // We cannot subdivide this quad further 
    if (tank != NULL)
        return tank;

    if ((topLeft.x + botRight.x) / 2 >= tank->position.x)
    {
        // Indicates topLeftTree 
        if ((topLeft.y + botRight.y) / 2 >= tank->position.y)
        {
            if (topLeftTree == NULL)
                return NULL;
            return topLeftTree->search(tank);
        }

        // Indicates botLeftTree 
        else
        {
            if (botLeftTree == NULL)
                return NULL;
            return botLeftTree->search(tank);
        }
    }
    else
    {
        // Indicates topRightTree 
        if ((topLeft.y + botRight.y) / 2 >= tank->position.y)
        {
            if (topRightTree == NULL)
                return NULL;
            return topRightTree->search(tank);
        }

        // Indicates botRightTree 
        else
        {
            if (botRightTree == NULL)
                return NULL;
            return botRightTree->search(tank);
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