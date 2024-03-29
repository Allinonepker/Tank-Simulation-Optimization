#include "precomp.h" // include (only) this in every .cpp file

#define NUM_TANKS_BLUE 500
#define NUM_TANKS_RED 500

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
		//Tank* tankptr = &tank;
		tank_grid[xGrid][yGrid].addTank(make_unique<Tank>(tank));
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

//// -----------------------------------------------------------
//// Iterates through all tanks and returns the closest enemy tank for the given tank
//// -----------------------------------------------------------
//Tank& Game::find_closest_enemy(Tank& current_tank)
//{
//	float closest_distance = numeric_limits<float>::infinity();
//	int closest_index = 0;
//
//	for (int i = 0; i < tanks.size(); i++)
//	{
//		if (tanks.at(i).allignment != current_tank.allignment && tanks.at(i).active)
//		{
//			float sqr_dist = fabsf((tanks.at(i).get_position() - current_tank.get_position()).sqr_length());
//			if (sqr_dist < closest_distance)
//			{
//				closest_distance = sqr_dist;
//				closest_index = i;
//			}
//		}
//	}
//
//	return tanks.at(closest_index);
//}


unique_ptr<Tank>& Game::find_closest_enemy(Tank& current_tank)
{
	//Timer timer;
	int closest_grid_x(0);
	int closest_grid_y(0);

	float closest_distance = numeric_limits<float>::infinity();
	int x = (int)current_tank.position.x / (SCRWIDTH / COLSIZE);
	int y = (int)current_tank.position.y / (SCRHEIGHT / ROWSIZE);

	for (int i = 0; i < (int)SCRWIDTH / COLSIZE; i++) {
		for (int j = 0; j < (int)SCRHEIGHT / ROWSIZE; j++) {
			if (current_tank.allignment == RED) {
				if (tank_grid[i][j].hasBlue) {
					float distance = sqrt((pow(abs(x - i), 2) + (pow(abs(y - j), 2))));
					if (distance < closest_distance) {
						closest_distance = distance;
						closest_grid_x = i;
						closest_grid_y = j;
					}
				}
			}

			else if (current_tank.allignment == BLUE) {
				if (tank_grid[i][j].hasRed) {
					float distance = sqrt((pow(abs(x - i), 2) + (pow(abs(y - j), 2))));
					if (distance < closest_distance) {
						closest_distance = distance;
						closest_grid_x = i;
						closest_grid_y = j;
					}
				}
			}
		}
	}

	closest_distance = numeric_limits<float>::infinity();

	unique_ptr<Tank>* closest_tank = nullptr;

	for (unique_ptr<Tank>& tank : tank_grid[closest_grid_x][closest_grid_y].tanks)
	{
		if (tank->active && tank->allignment != current_tank.allignment)
		{
			float sqr_dist = fabsf((tank->get_position() - current_tank.get_position()).sqr_length());
			if (sqr_dist < closest_distance)
			{
				closest_distance = sqr_dist;
				closest_tank = &tank;
			}
		}
	}

	return *closest_tank;
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
	//cout << tank_grid[2][2].tanks[0] << endl;
	//////Timer timer;
	updateGrids();

	for (int i = 0; i < SCRWIDTH / COLSIZE; i++) {
		for (int j = 0; j < SCRHEIGHT / ROWSIZE; j++) {
			tank_grid[i][j].checkColors();
			tank_grid[i][j].removeNull();
		}
	}

	//Update tanks
	for (Tank& tank : tanks)
	{
		if (tank.active)
		{
			for (Tank& o_tank : tanks)
			{
				if (&tank == &o_tank) continue;

				vec2 dir = tank.get_position() - o_tank.get_position();
				float dir_squared_len = dir.sqr_length();

				float col_squared_len = (tank.get_collision_radius() + o_tank.get_collision_radius());
				col_squared_len *= col_squared_len;

				if (dir_squared_len < col_squared_len)
				{
					tank.push(dir.normalized(), 1.f);
				}
			}

			/*      Timer timer;*/
					//Check tank collision and nudge tanks away from each other
					/*int x_grid = tank.xGrid;
					int y_grid = tank.yGrid;
					int x_bound_left, x_bound_right, y_bound_bottom, y_bound_top;

					(x_grid == 0) ? x_bound_left = 0, x_bound_right = x_grid + 1 : (x_grid == SCRWIDTH / COLSIZE - 1) ? x_bound_right = x_grid, x_bound_left = x_grid - 1 : x_bound_left = x_grid - 1,
						x_bound_right = x_grid + 1;

					(y_grid == 0) ? y_bound_bottom = 0, y_bound_top = y_grid + 1 : (y_grid == SCRHEIGHT / ROWSIZE - 1) ? y_bound_top = y_grid, y_bound_bottom = y_grid - 1 : y_bound_bottom = y_grid - 1,
						y_bound_top = y_grid + 1;

					for (int i = x_bound_left; i <= x_bound_right; i++) {
						for (int j = y_bound_bottom; j <= y_bound_top; j++) {
							for (unique_ptr<Tank> &tanko : tank_grid[i][j].tanks) {
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
					}*/

					//{
			for (Tank& o_tank : tanks)
			{
				if (&tank == &o_tank) continue;

				vec2 dir = tank.get_position() - o_tank.get_position();
				float dir_squared_len = dir.sqr_length();

				float col_squared_len = (tank.get_collision_radius() + o_tank.get_collision_radius());
				col_squared_len *= col_squared_len;

				if (dir_squared_len < col_squared_len)
				{
					tank.push(dir.normalized(), 1.f);
				}
			}

			//Move tanks according to speed and nudges (see above) also reload
			tank.tick();

			//Shoot at closest target if reloaded
			if (tank.rocket_reloaded())
			{
				//Tank& target = find_closest_enemy(tank);
				unique_ptr<Tank>& target = find_closest_enemy(tank);

				//rockets.push_back(Rocket(tank.position, (target.get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));
				rockets.push_back(Rocket(tank.position, (target->get_position() - tank.position).normalized() * 3, rocket_radius, tank.allignment, ((tank.allignment == RED) ? &rocket_red : &rocket_blue)));

				tank.reload_rocket();
			}
		}
	}

	//Update smoke plumes
	for (Smoke& smoke : smokes)
	{
		smoke.tick();
	}

	for (Rocket& rocket : rockets)
	{
		rocket.tick();
		bool mustbreak = false;

		int x_grid_rocket = ((abs((int)rocket.position.x)) / (SCRWIDTH / COLSIZE) >= 64) ? 63 : (abs((int)rocket.position.x)) / (SCRWIDTH / COLSIZE);
		int y_grid_rocket = ((abs((int)rocket.position.y)) / (SCRHEIGHT / ROWSIZE) >= 36) ? 35 : (abs((int)rocket.position.y)) / (SCRHEIGHT / ROWSIZE);
		int x_bound_left_rocket, x_bound_right_rocket, y_bound_bottom_rocket, y_bound_top_rocket;

		(x_grid_rocket == 0) ? x_bound_left_rocket = 0, x_bound_right_rocket = x_grid_rocket + 1 : (x_grid_rocket == SCRWIDTH / COLSIZE - 1) ? x_bound_right_rocket = x_grid_rocket, x_bound_left_rocket = x_grid_rocket - 1 : x_bound_left_rocket = x_grid_rocket - 1,
			x_bound_right_rocket = x_grid_rocket + 1;

		(y_grid_rocket == 0) ? y_bound_bottom_rocket = 0, y_bound_top_rocket = y_grid_rocket + 1 : (y_grid_rocket == SCRHEIGHT / ROWSIZE - 1) ? y_bound_top_rocket = y_grid_rocket, y_bound_bottom_rocket = y_grid_rocket - 1 : y_bound_bottom_rocket = y_grid_rocket - 1,
			y_bound_top_rocket = y_grid_rocket + 1;

		for (int i = x_bound_left_rocket; i <= x_bound_right_rocket; i++) {
			if (i == 64)
				continue;

			if (mustbreak)
				break;
			for (int j = y_bound_bottom_rocket; j <= y_bound_top_rocket; j++) {
				if (j == 36)
					continue;
				if (mustbreak)
					break;
				for (unique_ptr<Tank>& tank : tank_grid[i][j].tanks) {
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

			//Check if rocket collides with enemy tank, spawn explosion and if tank is destroyed spawn a smoke plume

			//for (Tank& tank : tanks)
			//{
			//	if (tank.active && (tank.allignment != rocket.allignment) && rocket.intersects(tank.position, tank.collision_radius))
			//	{
			//		explosions.push_back(Explosion(&explosion, tank.position));

			//		if (tank.hit(ROCKET_HIT_VALUE))
			//		{
			//			smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
			//		}

			//		rocket.active = false;
			//		break;
			//	}
			//}
		}

		//Remove exploded rockets with remove erase idiom
		rockets.erase(std::remove_if(rockets.begin(), rockets.end(), [](const Rocket& rocket) { return !rocket.active; }), rockets.end());

		//Update particle beams
		for (Particle_beam& particle_beam : particle_beams)
		{
			particle_beam.tick(tanks);

			////Damage all tanks within the damage window of the beam (the window is an axis-aligned bounding box)
			//for (Tank& tank : tanks)
			//{
			//    if (tank.active && particle_beam.rectangle.intersects_circle(tank.get_position(), tank.get_collision_radius()))
			//    {
			//        if (tank.hit(particle_beam.damage))
			//        {
			//            smokes.push_back(Smoke(smoke, tank.position - vec2(0, 48)));
			//        }
			//    }
			//}

			int x_grid_min = ((abs((int)particle_beam.min_position.x)) / (SCRWIDTH / COLSIZE));
			int x_grid_max = ((abs((int)particle_beam.min_position.x) + particle_beam.max_position.x) / (SCRWIDTH / COLSIZE));

			int y_grid_min = ((abs((int)particle_beam.min_position.y)) / (SCRHEIGHT / ROWSIZE));
			int y_grid_max = ((abs((int)particle_beam.max_position.y) + particle_beam.max_position.y) / (SCRHEIGHT / ROWSIZE));

			for (int i = x_grid_min; i <= x_grid_max; i++) {
				for (int j = y_grid_min; j <= y_grid_max; j++) {
					for (unique_ptr<Tank>& tanko : tank_grid[i][j].tanks) {
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

		//Update explosion sprites and remove when done with remove erase idiom
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
		std::vector<const Tank*> sorted_tanks;
		insertion_sort_tanks_health(tanks, sorted_tanks, begin, begin + NUM_TANKS);

		for (int i = 0; i < NUM_TANKS; i++)
		{
			int health_bar_start_x = i * (HEALTH_BAR_WIDTH + HEALTH_BAR_SPACING) + HEALTH_BARS_OFFSET_X;
			int health_bar_start_y = (t < 1) ? 0 : (SCRHEIGHT - HEALTH_BAR_HEIGHT) - 1;
			int health_bar_end_x = health_bar_start_x + HEALTH_BAR_WIDTH;
			int health_bar_end_y = (t < 1) ? HEALTH_BAR_HEIGHT : SCRHEIGHT - 1;

			screen->bar(health_bar_start_x, health_bar_start_y, health_bar_end_x, health_bar_end_y, REDMASK);
			screen->bar(health_bar_start_x, health_bar_start_y + (int)((double)HEALTH_BAR_HEIGHT * (1 - ((double)sorted_tanks.at(i)->health / (double)TANK_MAX_HEALTH))), health_bar_end_x, health_bar_end_y, GREENMASK);
		}
	}
}

// -----------------------------------------------------------
// Sort tanks by health value using insertion sort
// -----------------------------------------------------------
void Tmpl8::Game::insertion_sort_tanks_health(const std::vector<Tank>& original, std::vector<const Tank*>& sorted_tanks, int begin, int end)
{
	const int NUM_TANKS = end - begin;
	sorted_tanks.reserve(NUM_TANKS);
	sorted_tanks.emplace_back(&original.at(begin));

	for (int i = begin + 1; i < (begin + NUM_TANKS); i++)
	{
		const Tank& current_tank = original.at(i);

		for (int s = (int)sorted_tanks.size() - 1; s >= 0; s--)
		{
			const Tank* current_checking_tank = sorted_tanks.at(s);

			if ((current_checking_tank->compare_health(current_tank) <= 0))
			{
				sorted_tanks.insert(1 + sorted_tanks.begin() + s, &current_tank);
				break;
			}

			if (s == 0)
			{
				sorted_tanks.insert(sorted_tanks.begin(), &current_tank);
				break;
			}
		}
	}
}

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
	//int id = 0;
	//for (Tank& tank : tanks) {
	//    int xGrid = ((int)tank.position.x / (SCRWIDTH / COLSIZE));
	//    int yGrid = ((int)tank.position.y / (SCRHEIGHT / ROWSIZE));
	//    if (tank.xGrid != xGrid || tank.yGrid != yGrid) {
	//        tank.setXY(xGrid, yGrid);
	//        tank_grid[xGrid][yGrid].addTank(make_unique<Tank>(tank));
	//		tank_grid[tank.xGrid][tank.yGrid].removeTank(id);
	//    }
	//}


	for (int i = 0; i < SCRWIDTH / COLSIZE; i++) {
		for (int j = 0; j < SCRHEIGHT / ROWSIZE; j++) {
			int count = 0;
			for ( Tank *tank : tank_grid[i][j].tanks) {
				int xGrid = ((int)tank->position.x / (SCRWIDTH / COLSIZE));
				int yGrid = ((int)tank->position.y / (SCRHEIGHT / ROWSIZE));
				if (tank->xGrid != xGrid || tank->yGrid != yGrid) {
					tank->setXY(xGrid, yGrid);
					tank_grid[xGrid][yGrid].tanks.push_back(move(tank));
					tank_grid[tank->xGrid][tank->yGrid].tanks.erase(tank_grid[tank->xGrid][tank->yGrid].tanks.begin() + count);
				}
				count++;
			}
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