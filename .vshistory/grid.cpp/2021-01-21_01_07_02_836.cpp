#include "precomp.h"
#include "grid.h"

namespace Tmpl8
{
	void Grid::addTank(Tank *tank)
	{
		tanks.push_back(tank);
	}
}