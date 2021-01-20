#include "precomp.h"
#include "grid.h"

namespace Tmpl8
{
	void Grid::addTank(Tank *tank)
	{
		tanks.push_back(tank);
	}

	void Grid::checkColors() {
		Timer timer;
		bool blue = false;
		bool red = false;

		for (Tank* tank : tanks) {
			if (!tank->active)
				continue;

			if (blue && red)
				break;

			if (tank->allignment == RED)
				red = true;
			if (tank->allignment == BLUE)
				blue = true;
		}

		if (!red)
			hasRed = false;
		else
			hasRed = true;

		if (!blue)
			hasBlue = false;
		else
			hasBlue = true;
	}
}