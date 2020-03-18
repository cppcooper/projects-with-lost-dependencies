#ifndef _THEBUILDER_H_
#define _THEBUILDER_H_

#include "cell_manager.h"
#include "directions.h"
#include <tools_logger.h>
#include "debug_funcs.h"

#include <ctime>
#include <stack>
#include <iostream>

using namespace logger;

typedef std::pair<int, int> maze_specs;
typedef std::stack<coords> maze_path;

class thebuilder
{
	private:
		unsigned char**			maze = nullptr;
		cell_mgr						Cartographer;
		maze_specs					size;
		coords							position;
		maze_path					current_path;
		std::list<maze_path>	paths;

	protected:
		inline direction get_directions();
		inline void build_up();
		inline void build_down();
		inline void build_left();
		inline void build_right();
		inline void build_path();
		inline void tour();
		inline bool backtrack();

	public:
		unsigned char **generate(maze_specs& gen_size);
};

extern inline void draw(unsigned char **maze, maze_specs size, coords cursor);

#endif