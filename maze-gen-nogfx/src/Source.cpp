#include "thebuilder.h"
#include <tools_logger.h>
#include "debug_funcs.h"

using namespace logger;

void main()
{
	logger::LogFile_Manager::RegisterLog( "mazebuilder.log", false );

	thebuilder bob;
	maze_specs size;
	size.first = 50;
	size.second = 20;
	LOGFILE_1(_INFO) << "Maze sizes set" << coords_ToString(size);
	unsigned char **maze = bob.generate(size);
	draw(maze, size, std::make_pair(0,0));
	system("pause");
}

inline void draw(unsigned char **maze, maze_specs size, coords cursor)
{
	system("cls");
	std::stringstream buffer;
	for ( int y = size.second - 1; y >= 0; --y )
	{
		for ( int x = 0; x < size.first; ++x )
		{
			if ( (x == cursor.first) && (y == cursor.second) )
			{
				buffer << "^";
			}
			else
			{
				switch ( maze[x][y] )
				{
					case 0:
						buffer << " ";
						break;
					case 1:
						buffer << "#";
						break;
					default:
						buffer << "X";
						break;
				}
			}
		}
		buffer << std::endl;
	}
	std::cout << buffer.str();
	//char c;
	//std::cin >> c;
}