#include "Impl.hpp"
#include <bit>

Impl::Impl(int number)
    : Impl(number, false)
{
}

Impl::Impl(int number, bool isDontCare)
    : number_(number),
      index_(Count(number)),
      mask_(0),
      wasCombined_(false),
      isDontCare_(isDontCare)
{
}

unsigned int Impl::Count(unsigned int value)
{
    unsigned int retval = std::popcount(value);
    return retval;
}

Impl Impl::Patch(Impl &first, Impl &second)
{
    if (is_patch_allowed_for(first, second))
    {
        Impl newImpl = first;
        newImpl.mask_ |= second.number_ - first.number_;
        newImpl.wasCombined_ = false;
        first.wasCombined_ = true;
        second.wasCombined_ = true;
        newImpl.isDontCare_ = first.isDontCare_ && second.isDontCare_;
        return newImpl;
    }
    else
    {
        return Impl(-1);
    }
}

bool Impl::is_patch_allowed_for(const Impl &first, const Impl &second)
{
    if (first.number_ < second.number_ && first.mask_ == second.mask_ && second.index_ - first.index_ == 1 && Count(second.number_ - first.number_) == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}