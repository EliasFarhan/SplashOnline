//
// Created by unite on 15.07.2024.
//

#ifndef SPLASHONLINE_ROLLBACK_SYSTEM_H
#define SPLASHONLINE_ROLLBACK_SYSTEM_H

#include <cstdint>

namespace splash
{

	template<typename T>
    class RollbackInterface
    {
    public:
        [[nodiscard]] virtual std::uint32_t CalculateChecksum() const = 0;
        virtual void RollbackFrom(const T& system) = 0;
    };

}


#endif //SPLASHONLINE_ROLLBACK_SYSTEM_H
