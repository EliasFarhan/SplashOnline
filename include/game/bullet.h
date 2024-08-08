//
// Created by unite on 09.07.2024.
//

#ifndef SPLASHONLINE_BULLET_H
#define SPLASHONLINE_BULLET_H

namespace splash
{
struct Bullet
{

};

class GameSystems;
class BulletManager
{
public:
	explicit BulletManager(GameSystems* gameSystems);
private:
	GameSystems* gameSystems_ = nullptr;
};
}
#endif //SPLASHONLINE_BULLET_H
