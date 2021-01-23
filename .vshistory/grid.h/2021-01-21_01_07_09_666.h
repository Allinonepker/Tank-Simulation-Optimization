#pragma once

namespace Tmpl8
{
    class Grid
    {
    public:
        Grid() : hasBlue(false), hasRed(false) {};

        bool hasBlue;
        bool hasRed;
        void addTank(Tank*);
        vector<Tank*> tanks;
        
    };
} // namespace Tmpl8