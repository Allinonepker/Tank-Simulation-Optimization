#pragma once

namespace Tmpl8
{
	//forward declarations
	class Tank;
	class Rocket;
	class Smoke;
	class Particle_beam;

	class Game
	{
	public:
		void set_target(Surface* surface) { screen = surface; }
		void init();
		void shutdown();
		void smokeFunc();;
		void update(float deltaTime);
		void draw();
		void tick(float deltaTime);
		int partition(vector<Tank*>& values, int left, int right);
		void quicksort(vector<Tank*>& values, int left, int right, int depth);
		//void quicksort(vector<Tank*>& values, int left, int right);
		void measure_performance();
		void updateGrids();
		Tank*& find_closest_enemy(Tank& current_tank);
		ThreadPool pool(4);
		void collisionTanks();

		void particleBeam();

		void explosionsFunc();

		void rocketsFunc();

		void mouse_up(int button)
		{ /* implement if you want to detect mouse button presses */
		}

		void mouse_down(int button)
		{ /* implement if you want to detect mouse button presses */
		}

		void mouse_move(int x, int y)
		{ /* implement if you want to detect mouse movement */
		}

		void key_up(int key)
		{ /* implement if you want to handle keys */
		}

		void key_down(int key)
		{ /* implement if you want to handle keys */
		}

	private:
		Surface* screen;
		vector<Tank> tanks;
		vector<Rocket> rockets;
		vector<Smoke> smokes;
		vector<Explosion> explosions;
		vector<Particle_beam> particle_beams;
		Grid tank_grid[SCRWIDTH / COLSIZE][SCRHEIGHT / ROWSIZE];
		Font* frame_count_font;
		long long frame_count = 0;
		vector<pair<int, int>> redGrids;
		vector<pair<int, int>> blueGrids;
		void checkColors();
		vector<Tank*> tank_pointers_health;
		bool lock_update = false;
	};
}; // namespace Tmpl8