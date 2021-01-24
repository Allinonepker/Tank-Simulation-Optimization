#include "precomp.h"
#include "grid.h"

namespace Tmpl8
{
	void Grid::insertTank(Tank* tank)
	{
		tanks.push_back(tank);
	}
}