/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#include "flags.hpp"

#include <stdexcept>
#include <ostream>

std::ostream &io::operator<<(std::ostream &os, flags fl)
{
    if (fl.test(flags::empty))
    {
        os << "[empty]";
    }
    if (fl.test(flags::in))
    {
        os << "[in]";
    }
    if (fl.test(flags::out))
    {
        os << "[out]";
    }
    if (fl.test(flags::error))
    {
        os << "[error]";
    }

    return os;
}
