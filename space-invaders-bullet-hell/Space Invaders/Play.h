#ifndef _PLAY_H_
#define _PLAY_H_

#include "screen_objs.h"
#include "invasion_mgr.h"
#include "STL.h"

class bullet;

class player : public ScreenObject
{
private:
	float xincrements = 200;
	bool alive = true;

protected:
public:
	player();
	virtual ~player()
	{
		if ( playerbullet )
		{
			delete playerbullet;
		}
	}

	void deleteSelf() final override { delete this; }
	void Death(); 
	void unDeath();
	float GetX() const { return x; }
	float GetY() const { return y; }
	void MoveLeft( double& seconds );
	void MoveRight( double& seconds );
	void Sync(){}
	void Draw();
	bullet* playerbullet = nullptr;
};

#endif