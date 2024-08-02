#ifndef SPLASHONLINE_PLAYER_RENDERER_H_
#define SPLASHONLINE_PLAYER_RENDERER_H_

namespace splash
{

class PlayerRenderer
{
public:
	void Begin();
	void End();
	void Update(float dt);
	void Draw();

private:
};

}

#endif