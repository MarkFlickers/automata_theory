#include "ItemState.hpp"

ItemState::ItemState(int nextState)
    : nextState_(nextState)
{
}

int ItemState::Value() const
{
    return nextState_;
}

bool ItemState::IsDefined() const
{
    return nextState_ != kUndefinedValue;
}
