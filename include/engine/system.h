//
// Created by unite on 15.07.2024.
//

#ifndef SPLASHONLINE_SYSTEM_H
#define SPLASHONLINE_SYSTEM_H

namespace splash
{

    class System
    {
    public:
        virtual void Begin() = 0;
        virtual void End() = 0;
        virtual void Update(float dt) = 0;
    };

}

#endif //SPLASHONLINE_SYSTEM_H
