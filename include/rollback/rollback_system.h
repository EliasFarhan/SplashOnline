//
// Created by unite on 15.07.2024.
//

#ifndef SPLASHONLINE_ROLLBACK_SYSTEM_H
#define SPLASHONLINE_ROLLBACK_SYSTEM_H

namespace splash
{

    class RollbackInterface
    {
    public:
        virtual void Tick() = 0;
        virtual void Rollback() = 0;
    };

}


#endif //SPLASHONLINE_ROLLBACK_SYSTEM_H
