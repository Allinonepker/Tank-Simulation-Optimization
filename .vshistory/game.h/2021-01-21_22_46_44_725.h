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
    void update(float deltaTime);
    void draw();
    void tick(float deltaTime);
    void insertion_sort_tanks_health(const std::vector<Tank>& original, std::vector<const Tank*>& sorted_tanks, int begin, int end);
    void quick_sort_tanks_health(const std::vector<Tank>& original, int l, int r);
    void merge(const std::vector<Tank>& original, int l, int r);
    vector<Tank*> quick_sort_tanks_health(const std::vector<Tank>& original, vector<Tank*> sorted_tanks, int l, int u);
    vector<Tank*> quick_sort_tanks_health(const std::vector<Tank>& original, std::vector<const Tank*>& sorted_tanks, int l, int u);
    int partition(vector<Tank&>, int l, int u);
    void measure_performance();
    void updateGrids();
    Tank*& find_closest_enemy(Tank& current_tank);
    //Tank& find_closest_enemy(Tank& current_tank);

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
    bool lock_update = false;
};

}; // namespace Tmpl8