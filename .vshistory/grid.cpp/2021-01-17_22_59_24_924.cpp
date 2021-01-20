#include "precomp.h"
#include "grid.h"

namespace Tmpl8
{
	void Grid::addTank(Tank *tank)
	{
		tanks.push_back(tank);
	}

	void Grid::removeTank(int ID)
	{
		for (int i = 0; i < tanks.size(); i++) {
			if (tanks[i]->ID == ID)
				tanks.erase(tanks.begin() + i);
		}
	
		//auto it = find_if(tanks.begin(), tanks.end(), [&](unique_ptr<Tank> &tank) { return tank->ID == ID; });
	
		//if (it != tanks.end()) {
		//	auto retval = std::move(*it);
		//	tanks.erase(it);
		//}
	}

	void Grid::removeNull() {
		tanks.erase(std::remove(tanks.begin(), tanks.end(), nullptr),
			tanks.end());
	}

	void Grid::checkColors() {
		bool blue = false;
		bool red = false;

		for (unique_ptr<Tank> &tank : tanks) {
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