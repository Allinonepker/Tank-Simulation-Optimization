#pragma once

namespace Tmpl8
{
    class Grid
    {
    public:
        Grid() {};
        void addTank(Tank*);
        vector<Tank*> tanks;
        
    };
} // namespace Tmpl8