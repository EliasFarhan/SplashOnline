//
// Created by unite on 15.07.2024.
//

#ifndef SPLASHONLINE_SYSTEM_H
#define SPLASHONLINE_SYSTEM_H

namespace splash
{

class SystemInterface
{
public:
	virtual ~SystemInterface() = default;
	virtual void Begin() = 0;
	virtual void End() = 0;
	virtual void Update(float dt) = 0;

	[[nodiscard]] virtual int GetSystemIndex() const = 0;
	virtual void SetSystemIndex(int index) = 0;
};

}

#endif //SPLASHONLINE_SYSTEM_H
