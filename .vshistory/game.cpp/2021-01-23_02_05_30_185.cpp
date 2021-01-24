#include "precomp.h" // include (only) this in every .cpp file

#define NUM_TANKS_BLUE 1000
#define NUM_TANKS_RED 1000

#define TANK_MAX_HEALTH 1000
#define ROCKET_HIT_VALUE 60
#define PARTICLE_BEAM_HIT_VALUE 50

#define TANK_MAX_SPEED 1.5

#define HEALTH_BARS_OFFSET_X 0
#define HEALTH_BAR_HEIGHT 70
#define HEALTH_BAR_WIDTH 1
#define HEALTH_BAR_SPACING 0

#define MAX_FRAMES 2000

//Global performance timer
#define REF_PERFORMANCE 73466 //UPDATE THIS WITH YOUR REFERENCE PERFORMANCE (see console after 2k frames)
static timer perf_timer;
static float duration;

//Load sprite files and initialize sprites
static Surface* background_img = new Surface("assets/Background_Grass.png");
static Surface* tank_red_img = new Surface("assets/Tank_Proj2.png");
static Surface* tank_blue_img = new Surface("assets/Tank_Blue_Proj2.png");
static Surface* rocket_red_img = new Surface("assets/Rocket_Proj2.png");
static Surface* rocket_blue_img = new Surface("assets/Rocket_Blue_Proj2.png");
static Surface* particle_beam_img = new Surface("assets/Particle_Beam.png");
static Surface* smoke_img = new Surface("assets/Smoke.png");
static Surface* explosion_img = new Surface("assets/Explosion.png");

static Sprite background(background_img, 1);
static Sprite tank_red(tank_red_img, 12);
static Sprite tank_blue(tank_blue_img, 12);
static Sprite rocket_red(rocket_red_img, 12);
static Sprite rocket_blue(rocket_blue_img, 12);
static Sprite smoke(smoke_img, 4);
static Sprite explosion(explosion_img, 9);
static Sprite particle_beam_sprite(particle_beam_img, 3);

const static vec2 tank_size(14, 18);
const static vec2 rocket_size(25, 24);

const static float tank_radius = 8.5f;
const static float rocket_radius = 10.f;

struct Timer
{
	std::chrono::time_point<std::chrono::steady_clock> start, end;
	std::chrono::duration<float> duration;

	Timer() {
		start = std::chrono::high_resolution_clock::now();
	}

	~Timer() {
		end = std::chrono::high_resolution_clock::now();
		duration = end - start;

		float ms = duration.count() * 1.000f;
		std::cout << "This operation took " << ms << "ms" << std::endl;
	}
};
// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::init()
{
	frame_count_font = new Font("assets/digital_small.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZ:?!=-0123456789.");

	tanks.reserve(NUM_TANKS_BLUE + NUM_TANKS_RED);

	uint rows = (uint)sqrt(NUM_TANKS_BLUE + NUM_TANKS_RED);
	uint max_rows = 12;

	float start_blue_x = tank_size.x + 10.0f;
	float start_blue_y = tank_size.y + 80.0f;

	float start_red_x = 980.0f;
	float start_red_y = 100.0f;

	float spacing = 15.0f;

	//Spawn blue tanks
	int id = 0;
	for (int i = 0; i < NUM_TANKS_BLUE; i++) {
		tanks.push_back(Tank(start_blue_x + ((i % max_rows) * spacing), start_blue_y + ((i / max_rows) * spacing), BLUE, &tank_blue, &smoke, 1200, 600, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED, id));
		++id;
	}
	//Spawn red tanks
	for (int i = 0; i < NUM_TANKS_RED; i++)
	{
		tanks.push_back(Tank(start_red_x + ((i % max_rows) * spacing), start_red_y + ((i / max_rows) * spacing), RED, &tank_red, &smoke, 80, 80, tank_radius, TANK_MAX_HEALTH, TANK_MAX_SPEED, id));
		++id;
	}

	for (Tank& tank : tanks) {
		int xGrid = ((int)tank.position.x / (SCRWIDTH / COLSIZE));
		int yGrid = ((int)tank.position.y / (SCRHEIGHT / ROWSIZE));
		tank.setXY(xGrid, yGrid);
		tank_grid[xGrid][yGrid].insertTank(&tank);
		tank_pointers_health.push_back(&tank);
	}

	particle_beams.push_back(Particle_beam(vec2(SCRWIDTH / 2, SCRHEIGHT / 2), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
	particle_beams.push_back(Particle_beam(vec2(80, 80), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
	particle_beams.push_back(Particle_beam(vec2(1200, 600), vec2(100, 50), &particle_beam_sprite, PARTICLE_BEAM_HIT_VALUE));
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::shutdown()
{
}

void Game::smokeFunc() {
	for (Smoke& smoke : smokes)
	{
		smoke.tick();
	}
}
//// -----------------------------------------------------------
//// Iterates through all tanks and returns the closest enemy tank for the given tank
//// -----------------------------------------------------------

Tank*& Game::find_closest_enemy(Tank& current_tank)
{
	int closest_grid_x(0);
	int closest_grid_y(0);

	float closest_distance = numeric_limits<float>::infinity();
	int xPosCurrentTank = current_tank.xGrid;
	int yPosCurrentTank = current_tank.yGrid;

	for (pair<int, int> pair : (current_tank.allignment == RED) ? blueGrids : redGrids) {
		float distance = sqrt((pow(abs(xPosCurrentTank - pair.first), 2) + (pow(abs(yPosCurrentTank - pair.second), 2))));
		if (distance < closest_distance) {
			closest_distance = distance;
			closest_grid_x = pair.first;
			closest_grid_y = pair.second;
		}
	}

	closest_distance = numeric_limits<float>::infinity();
	int closest_index = 0;
	int index = 0;

	for (Tank*& tank : tank_grid[closest_grid_x][closest_grid_y].tanks)
	{
		if (tank->active && tank->allignment != current_tank.allignment)
		{
			float sqr_dist = fabsf((tank->get_position() - current_tank.get_position()).sqr_length());
			if (sqr_dist < closest_distance)
			{
				closest_distance = sqr_dist;
				closest_index = index;
			}
		}
		index++;
	}

	return tank_grid[closest_grid_x][closest_grid_y].tanks[closest_index];
}

void Game::collisionTanks() {
	for (Tank& tank : tanks)
	{
		if (tank.active)
		{
			//Check tank collision and nudge tanks away from each other
			size_t x_grid = tank.xGrid;
			size_t y_grid = tank.yGrid;
			int x_bound_left, x_bound_right, y_bound_bottom, y_bound_top;

			(x_grid == 0) ? x_bound_left = 0, x_bound_right = x_grid + 1 : (x_grid == SCRWIDTH / COLSIZE - 1) ? x_bound_right = x_grid, x_bound_left = x_grid - 1 : x_bound_left = x_grid - 1,
				x_bound_right = x_grid + 1;

			(y_grid == 0) ? y_bound_bottom = 0, y_bound_top = y_grid + 1 : (y_grid == SCRHEIGHT / ROWSIZE - 1) ? y_bound_top = y_grid, y_bound_bottom = y_grid - 1 : y_bound_bottom = y_grid - 1,
				y_bound_top = y_grid + 1;

			for (size_t i = x_bound_left; i <= x_bound_right; i++) {
				for (size_t j = y_bound_bottom; j <= y_bound_top; j++) {
					for (Tank*& tanko : tank_grid[i][j].tanks) {
						if (tank.ID == tanko->ID) continue;

						vec2 dir = tank.get_position() - tanko->get_position();
						float dir_squared_len = dir.sqr_length();

						float col_squared_len = (tank.get_collision_radius() + tanko->get_collision_radius());
						col_squared_len *= col_squared_len;

						if (dir_squared_len < col_squared_len)
						{
							tank.push(dir.normalized(), 1.f);
						}
					}
				}
			}

			//Move tanks according to speed and nudges (see above) also reload
			tank.tick();

			//Shoot at closest target if reloaded
			if (tank.rocket_reloaded())
			{
				//Tank& target = find_closest_enemy(tank);
				Tank*& target = find_closest_enemy(tank);

				//rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));
				rockets.push_back(Rocket(tank.position, (target->get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

				tank.reload_rocket();
			}
		}
	}
}

void Game::particleBeam() {
	for (Particle_beam& particle_beam : particle_beams)
	{
		particle_beam.tick(tanks);

		////Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)

		size_t x_grid_min = ((abs((int)particle_beam.min_position.x)) / (SCRWIDTH / COLSIZE));
		size_t x_grid_max = ((abs((int)particle_beam.min_position.x) + particle_beam.max_position.x) / (SCRWIDTH / COLSIZE));

		size_t y_grid_min = ((abs((int)particle_beam.min_position.y)) / (SCRHEIGHT / ROWSIZE));
		size_t y_grid_max = ((abs((int)particle_beam.max_position.y) + particle_beam.max_position.y) / (SCRHEIGHT / ROWSIZE));

		//if (x_grid_min < 0 || x_grid_max > SCRWIDTH / COLSIZE - 1 || y_grid_min < 0 || y_grid_max > SCRHEIGHT / ROWSIZE - 1)
		//	continue;

		for (size_t i = x_grid_min; i <= x_grid_max; i++) {
			for (size_t j = y_grid_min; j <= y_grid_max; j++) {
				for (Tank*& tanko : tank_grid[i][j].tanks) {
					if (tanko->active && particle_beam.rectangle.intersects_circle(tanko->get_position(), tanko->get_collision_radius()))
					{
						if (tanko->hit(particle_beam.damage))
						{
							smokes.push_back(Smoke(smoke, tanko->position - vec2(0, 48)));
						}
					}
				}
			}
		}
	}
}

void Game::explosionsFunc() {
	//Update explosion sprites and remove when done with remove erase idiom
	for (Explosion& explosion : explosions)
	{
		explosion.tick();
	}

	explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());
}

void Game::rocketsFunc() {
	for (Rocket& rocket : rockets)
	{
		rocket.tick();
		bool mustbreak = false;

		int x_grid_rocket = ((abs((int)rocket.position.x)) / (SCRWIDTH / COLSIZE) >= (SCRWIDTH / COLSIZE) - 1) ? (SCRWIDTH / COLSIZE) - 1 : (abs((int)rocket.position.x)) / (SCRWIDTH / COLSIZE);
		int y_grid_rocket = ((abs((int)rocket.position.y)) / (SCRHEIGHT / ROWSIZE) >= (SCRHEIGHT / ROWSIZE) - 1) ? (SCRHEIGHT / ROWSIZE) - 1 : (abs((int)rocket.position.y)) / (SCRHEIGHT / ROWSIZE);
		int x_bound_left_rocket, x_bound_right_rocket, y_bound_bottom_rocket, y_bound_top_rocket;

		(x_grid_rocket == 0) ? x_bound_left_rocket = 0, x_bound_right_rocket = x_grid_rocket + 1 : (x_grid_rocket == SCRWIDTH / COLSIZE - 1) ? x_bound_right_rocket = x_grid_rocket, x_bound_left_rocket = x_grid_rocket - 1 : x_bound_left_rocket = x_grid_rocket - 1,
			x_bound_right_rocket = x_grid_rocket + 1;

		(y_grid_rocket == 0) ? y_bound_bottom_rocket = 0, y_bound_top_rocket = y_grid_rocket + 1 : (y_grid_rocket == SCRHEIGHT / ROWSIZE - 1) ? y_bound_top_rocket = y_grid_rocket, y_bound_bottom_rocket = y_grid_rocket - 1 : y_bound_bottom_rocket = y_grid_rocket - 1,
			y_bound_top_rocket = y_grid_rocket + 1;

		for (int i = x_bound_left_rocket; i <= x_bound_right_rocket; i++) {
			if (mustbreak)
				break;
			for (int j = y_bound_bottom_rocket; j <= y_bound_top_rocket; j++) {
				if (mustbreak)
					break;
				for (Tank*& tank : tank_grid[i][j].tanks) {
					if (tank->active && (tank->allignment != rocket.allignment) && rocket.intersects(tank->position, tank->collision_radius)) {
						explosions.push_back(Explosion(&explosion, tank->position));

						if (tank->hit(ROCKET_HIT_VALUE))
						{
							smokes.push_back(Smoke(smoke, tank->position - vec2(0, 48)));
						}

						rocket.active = false;
						mustbreak = true;
						break;
					}
				}
			}
		}

		//Remove exploded rockets with remove erase idiom
		rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());
	}
}

// -----------------------------------------------------------
// Update the game state:
// Move all objects
// Update sprite frames
// Collision detection
// Targeting etc..
// -----------------------------------------------------------
void Game::update(float deltaTime)
{
	{/*Timer timer;*/
		updateGrids();
	}
	{/*Timer timer;*/
		checkColors();
	}
	//thread C(thread(&Game::collisionTanks, this));
	//thread S(thread(&Game::smokeFunc, this));
	//thread R(thread(&Game::rocketsFunc, this));
	//thread P(thread(&Game::particleBeam, this));
	//thread E(thread(&Game::explosionsFunc, this));
	////collisionTanks();
	////smokeFunc();
	////rocketsFunc();
	////particleBeam();
	////explosionsFunc();
	//C.join();
	//S.join();
	//R.join();
	//P.join();
	//E.join();
	{
		//Timer timer;
		for (Tank& tank : tanks)
		{
			if (tank.active)
			{
				//Check tank collision and nudge tanks away from each other
				size_t x_grid = tank.xGrid;
				size_t y_grid = tank.yGrid;
				int x_bound_left, x_bound_right, y_bound_bottom, y_bound_top;

				(x_grid == 0) ? x_bound_left = 0, x_bound_right = x_grid + 1 : (x_grid == SCRWIDTH / COLSIZE - 1) ? x_bound_right = x_grid, x_bound_left = x_grid - 1 : x_bound_left = x_grid - 1,
					x_bound_right = x_grid + 1;

				(y_grid == 0) ? y_bound_bottom = 0, y_bound_top = y_grid + 1 : (y_grid == SCRHEIGHT / ROWSIZE - 1) ? y_bound_top = y_grid, y_bound_bottom = y_grid - 1 : y_bound_bottom = y_grid - 1,
					y_bound_top = y_grid + 1;

				for (size_t i = x_bound_left; i <= x_bound_right; i++) {
					for (size_t j = y_bound_bottom; j <= y_bound_top; j++) {
						for (Tank*& tanko : tank_grid[i][j].tanks) {
							if (tank.ID == tanko->ID) continue;

							vec2 dir = tank.get_position() - tanko->get_position();
							float dir_squared_len = dir.sqr_length();

							float col_squared_len = (tank.get_collision_radius() + tanko->get_collision_radius());
							col_squared_len *= col_squared_len;

							if (dir_squared_len < col_squared_len)
							{
								tank.push(dir.normalized(), 1.f);
							}
						}
					}
				}

				//Move tanks according to speed and nudges (see above) also reload
				tank.tick();

				//Shoot at closest target if reloaded
				if (tank.rocket_reloaded())
				{
					//Tank& target = find_closest_enemy(tank);
					Tank*& target = find_closest_enemy(tank);

					//rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));
					rockets.push_back(Rocket(tank.position, (target->get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

					tank.reload_rocket();
				}
			}
		}
	}
	//Update smoke plumes
	{/*Timer timer;*/
		for (Smoke& smoke : smokes)
		{
			smoke.tick();
		}
	}
	{/*Timer timer;*/
		for (Rocket& rocket : rockets)
		{
			rocket.tick();
			bool mustbreak = false;

			int x_grid_rocket = ((abs((int)rocket.position.x)) / (SCRWIDTH / COLSIZE) >= (SCRWIDTH / COLSIZE) - 1) ? (SCRWIDTH / COLSIZE) - 1 : (abs((int)rocket.position.x)) / (SCRWIDTH / COLSIZE);
			int y_grid_rocket = ((abs((int)rocket.position.y)) / (SCRHEIGHT / ROWSIZE) >= (SCRHEIGHT / ROWSIZE) - 1) ? (SCRHEIGHT / ROWSIZE) - 1 : (abs((int)rocket.position.y)) / (SCRHEIGHT / ROWSIZE);
			int x_bound_left_rocket, x_bound_right_rocket, y_bound_bottom_rocket, y_bound_top_rocket;

			(x_grid_rocket == 0) ? x_bound_left_rocket = 0, x_bound_right_rocket = x_grid_rocket + 1 : (x_grid_rocket == SCRWIDTH / COLSIZE - 1) ? x_bound_right_rocket = x_grid_rocket, x_bound_left_rocket = x_grid_rocket - 1 : x_bound_left_rocket = x_grid_rocket - 1,
				x_bound_right_rocket = x_grid_rocket + 1;

			(y_grid_rocket == 0) ? y_bound_bottom_rocket = 0, y_bound_top_rocket = y_grid_rocket + 1 : (y_grid_rocket == SCRHEIGHT / ROWSIZE - 1) ? y_bound_top_rocket = y_grid_rocket, y_bound_bottom_rocket = y_grid_rocket - 1 : y_bound_bottom_rocket = y_grid_rocket - 1,
				y_bound_top_rocket = y_grid_rocket + 1;

			for (int i = x_bound_left_rocket; i <= x_bound_right_rocket; i++) {
				if (mustbreak)
					break;
				for (int j = y_bound_bottom_rocket; j <= y_bound_top_rocket; j++) {
					if (mustbreak)
						break;
					for (Tank*& tank : tank_grid[i][j].tanks) {
						if (tank->active && (tank->allignment != rocket.allignment) && rocket.intersects(tank->position, tank->collision_radius)) {
							explosions.push_back(Explosion(&explosion, tank->position));

							if (tank->hit(ROCKET_HIT_VALUE))
							{
								smokes.push_back(Smoke(smoke, tank->position - vec2(0, 48)));
							}

							rocket.active = false;
							mustbreak = true;
							break;
						}
					}
				}
			}

			//Remove exploded rockets with remove erase idiom
			rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());
		}
	}
	//Update particle beams
	{Timer timer;
	for (Particle_beam& particle_beam : particle_beams)
	{
		particle_beam.tick(tanks);

		////Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)

		size_t x_grid_min = ((abs((int)particle_beam.min_position.x)) / (SCRWIDTH / COLSIZE));
		size_t x_grid_max = ((abs((int)particle_beam.min_position.x) + particle_beam.max_position.x) / (SCRWIDTH / COLSIZE));

		size_t y_grid_min = ((abs((int)particle_beam.min_position.y)) / (SCRHEIGHT / ROWSIZE));
		size_t y_grid_max = ((abs((int)particle_beam.max_position.y) + particle_beam.max_position.y) / (SCRHEIGHT / ROWSIZE));

		//if (x_grid_min < 0 || x_grid_max > SCRWIDTH / COLSIZE - 1 || y_grid_min < 0 || y_grid_max > SCRHEIGHT / ROWSIZE - 1)
		//	continue;

		for (size_t i = x_grid_min; i <= x_grid_max; i++) {
			for (size_t j = y_grid_min; j <= y_grid_max; j++) {
				for (Tank*& tanko : tank_grid[i][j].tanks) {
					if (tanko->active && particle_beam.rectangle.intersects_circle(tanko->get_position(), tanko->get_collision_radius()))
					{
						if (tanko->hit(particle_beam.damage))
						{
							smokes.push_back(Smoke(smoke, tanko->position - vec2(0, 48)));
						}
					}
				}
			}
		}
	}
	}
	//Update explosion sprites and remove when done with remove erase idiom
	{/*Timer timer;*/
	for (Explosion& explosion : explosions)
	{
		explosion.tick();
	}

	explosions.erase(std::remove_if(explosions.begin(), explosions.end(), [](const Explosion& explosion) { return explosion.done(); }), explosions.end());
	}
}
void Game::draw()
{
	// clear the graphics window
	screen->clear(0);

	//Draw background
	background.draw(screen, 0, 0);

	//Draw sprites
	for (int i = 0; i < NUM_TANKS_BLUE + NUM_TANKS_RED; i++)
	{
		tanks.at(i).draw(screen);

		vec2 tank_pos = tanks.at(i).get_position();
		// tread marks
		if ((tank_pos.x >= 0) && (tank_pos.x < SCRWIDTH) && (tank_pos.y >= 0) && (tank_pos.y < SCRHEIGHT))
			background.get_buffer()[(int)tank_pos.x + (int)tank_pos.y * SCRWIDTH] = sub_blend(background.get_buffer()[(int)tank_pos.x + (int)tank_pos.y * SCRWIDTH], 0x808080);
	}

	for (Rocket& rocket : rockets)
	{
		rocket.draw(screen);
	}

	for (Smoke& smoke : smokes)
	{
		smoke.draw(screen);
	}

	for (Particle_beam& particle_beam : particle_beams)
	{
		particle_beam.draw(screen);
	}

	for (Explosion& explosion : explosions)
	{
		explosion.draw(screen);
	}

	//Draw sorted health bars
	for (int t = 0; t < 2; t++)
	{
		const int NUM_TANKS = ((t < 1) ? NUM_TANKS_BLUE : NUM_TANKS_RED);
		const int begin = ((t < 1) ? 0 : NUM_TANKS_BLUE);

		quicksort(tank_pointers_health, begin, begin + NUM_TANKS - 1);

		for (int i = 0; i < NUM_TANKS; i++)
		{
			int health_bar_start_x = i * (HEALTH_BAR_WIDTH + HEALTH_BAR_SPACING) + HEALTH_BARS_OFFSET_X;
			int health_bar_start_y = (t < 1) ? 0 : (SCRHEIGHT - HEALTH_BAR_HEIGHT) - 1;
			int health_bar_end_x = health_bar_start_x + HEALTH_BAR_WIDTH;
			int health_bar_end_y = (t < 1) ? HEALTH_BAR_HEIGHT : SCRHEIGHT - 1;

			screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
			screen->bar(health_bar_start_x, health_bar_start_y + (int)((double)HEALTH_BAR_HEIGHT * (1 - ((double)tank_pointers_health.at(i)->health / (double)TANK_MAX_HEALTH))), health_bar_end_x, health_bar_end_y, GREENMASK);
		}
	}
}

void Tmpl8::Game::quicksort(vector<Tank*>& values, int left, int right) {
	if (left < right) {
		int pivotIndex = partition(values, left, right);
		//thread T([&] {quicksort(values, left, pivotIndex - 1); });
		quicksort(values, left, pivotIndex - 1);
		quicksort(values, pivotIndex, right);
	}
}int Tmpl8::Game::partition(vector<Tank*>& values, int left, int right) {
	int pivotIndex = left + (right - left) / 2;
	int pivotValue = values[pivotIndex]->health;
	int i = left, j = right;
	while (i <= j) {
		while (values[i]->health < pivotValue) {
			i++;
		}
		while (values[j]->health > pivotValue) {
			j--;
		}
		if (i <= j) {
			std::swap(values[i], values[j]);
			i++;
			j--;
		}
	}
	return i;
}

//void Tmpl8::Game::quicksortinit(vector<Tank>& values, vector<Tank*> tanki, int left, int right) {
//	if (left < right) {
//		int pivotIndex = partition(values, left, right);
//		quicksort(values, left, pivotIndex - 1);
//		quicksort(values, pivotIndex, right);
//	}
//}
// -----------------------------------------------------------
// When we reach MAX_FRAMES print the duration and speedup multiplier
// Updating REF_PERFORMANCE at the top of this file with the value
// on your machine gives you an idea of the speedup your optimizations give
// -----------------------------------------------------------
void Tmpl8::Game::measure_performance()
{
	char buffer[128];
	if (frame_count >= MAX_FRAMES)
	{
		if (!lock_update)
		{
			duration = perf_timer.elapsed();
			cout << "Duration was: " << duration << " (Replace REF_PERFORMANCE with this value)" << endl;
			lock_update = true;
		}

		frame_count--;
	}

	if (lock_update)
	{
		screen->bar(420, 170, 870, 430, 0x030000);
		int ms = (int)duration % 1000, sec = ((int)duration / 1000) % 60, min = ((int)duration / 60000);
		sprintf(buffer, "%02i:%02i:%03i", min, sec, ms);
		frame_count_font->centre(screen, buffer, 200);
		sprintf(buffer, "SPEEDUP: %4.1f", REF_PERFORMANCE / duration);
		frame_count_font->centre(screen, buffer, 340);
	}
}

void Tmpl8::Game::updateGrids()
{
	for (size_t i = 0; i < SCRWIDTH / COLSIZE; i++) {
		for (size_t j = 0; j < SCRHEIGHT / ROWSIZE; j++) {
			vector<Tank*>::iterator it;
			for (it = tank_grid[i][j].tanks.begin(); it != tank_grid[i][j].tanks.end(); ) {
				int xGrid = ((*it)->position.x / (SCRWIDTH / COLSIZE));
				int yGrid = ((*it)->position.y / (SCRHEIGHT / ROWSIZE));

				if ((*it)->xGrid != xGrid || (*it)->yGrid != yGrid) {
					(*it)->setXY(xGrid, yGrid);
					tank_grid[xGrid][yGrid].insertTank(*it);
					it = tank_grid[i][j].tanks.erase(it);
				}
				else {
					++it;
				}
			}
		}
	}
}

void Game::checkColors() {
	redGrids.clear();
	blueGrids.clear();
	for (int i = 0; i < SCRWIDTH / COLSIZE; i++) {
		for (int j = 0; j < SCRHEIGHT / ROWSIZE; j++) {
			bool blue = false;
			bool red = false;

			for (Tank*& tank : tank_grid[i][j].tanks) {
				if (!tank->active)
					continue;

				if (blue && red)
					break;

				if (tank->allignment == RED)
					red = true;
				if (tank->allignment == BLUE)
					blue = true;
			}

			if (red)
				redGrids.push_back(make_pair(i, j));

			if (blue)
				blueGrids.push_back(make_pair(i, j));
		}
	}
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::tick(float deltaTime)
{
	if (!lock_update)
	{
		update(deltaTime);
	}
	draw();

	measure_performance();

	// print something in the graphics window
	//screen->Print("hello world", 2, 2, 0xffffff);

	// print something to the text window
	//cout << "This goes to the console window." << std::endl;

	//Print frame count
	frame_count++;
	string frame_count_string = "FRAME: " + std::to_string(frame_count);
	frame_count_font->print(screen, frame_count_string.c_str(), 350, 580);
}