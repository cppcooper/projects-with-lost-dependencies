#ifndef GAME_H_
#define GAME_H_

#include "OGLheaders.h"

#include <atomic>
#include <mutex>

#include "input_mgr.h"

#include "texture_mgr.h"
#include "ffont.h"

#include "Shader_Manager.h"
#include "sprite_mgr.h"
#include "mesh_mgr.h"
#include "scene_nodes.h"

#include "map_editor.h"
#include "rover.h"

class game
{
private:
	Root_SceneNode SceneGraph;
	ShaderManager sManager;
	GLSLProgram* shader_3d = nullptr;
	GLSLProgram* shader_2d = nullptr;
	GLSLProgram* font_shader = nullptr;

	glm::vec3 player_straight_axis;
	glm::vec3 player_side_axis;
	std::mutex mouseMutex;

	Map_Editor* editor = nullptr;
	View_Port* view_port = nullptr;
	Tile_Map* map = nullptr;
	Rover* myrover = nullptr;

	sprite* G = nullptr;

protected:
	void Init_GFX();
	void Init_Input();
	void Rotate_Camera( float angle );
	void Rotate_Left();
	void Rotate_Right();
	void Move_Forward();
	void Move_Backward();
	void Move_Left();
	void Move_Right();

public:
	~game()
	{
		if ( editor )
		{
			editor->Save();
		}
		delete editor;
		delete map;
		delete view_port;
		delete myrover;
	}
	void Init();
	void Deinit();
	void Update();
	void Draw();
	void DoInput( int& key, int& scancode, int& action, int& mods );
	void SetCursor( double x, double y );
	void Set_MouseClicks( int button, int actions, int mods );

	Texture_Manager txtr_mgr;
	Sprite_Manager sprite_mgr;
	s3d_mesh_mgr mesh_mgr;
	input_mgr Controls_Input_Mgr;
};

#endif
