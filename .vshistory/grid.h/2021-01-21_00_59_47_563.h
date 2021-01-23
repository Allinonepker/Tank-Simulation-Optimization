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
        void checkColors();
        vector<Tank*> tanks;
        int xArray;
        int yArray;
        
    };
} // namespace Tmpl8