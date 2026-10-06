#include "ItemOutput.hpp"

ItemOutput::ItemOutput(int output)
    : output_(output)
{
}

int ItemOutput::Value() const
{
    return output_;
}

bool ItemOutput::IsDefined() const
{
    return output_ != kUndefinedValue;
}
