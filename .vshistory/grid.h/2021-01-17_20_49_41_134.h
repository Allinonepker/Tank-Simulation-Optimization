#pragma once

namespace Tmpl8
{
    class Grid
    {
    public:
        Grid() : hasBlue(false), hasRed(false) {};

        bool hasBlue;
        bool hasRed;
        void addTank(unique_ptr<Tank>);
        void removeTank(int ID);
        void removeNull();
        void checkColors();
        vector<unique_ptr<Tank>> tanks;
        
    };
} // namespace Tmpl8