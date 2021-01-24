#pragma once

namespace Tmpl8
{
	class Grid
	{
	public:
		Grid() {};
		void insertTank(Tank*);
		vector<Tank*> tanks;
	};
} // namespace Tmpl8