#include "SCouple.hpp"

#include <stdexcept>
#include <utility>

SCouple::SCouple(int first, int second)
    : first_(first),
      second_(second)
{
    if (first_ == second_)
    {
        throw std::invalid_argument(
            "SCouple of identical states s" + std::to_string(first_) + " is trivial");
    }

    if (first_ > second_)
    {
        std::swap(first_, second_);
    }
}

int SCouple::First() const
{
    return first_;
}

int SCouple::Second() const
{
    return second_;
}

std::string SCouple::ToString() const
{
    return "(" + std::to_string(first_) + "," + std::to_string(second_) + ")";
}
