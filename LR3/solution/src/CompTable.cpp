#include "CompTable.hpp"

#include <algorithm>
#include <cstddef>
#include <iomanip>
#include <ostream>
#include <stdexcept>
#include <utility>

CompTable::CompTable(int stateCount)
    : stateCount_(stateCount)
{
    if (stateCount_ < 2)
    {
        throw std::invalid_argument("Compatibility table needs at least two states");
    }

    for (int state = 2; state <= stateCount_; ++state)
    {
        cells_.emplace_back(static_cast<std::size_t>(state - 1));
    }
}

int CompTable::StateCount() const
{
    return stateCount_;
}

bool CompTable::IsCompatible(const SCouple& pair) const
{
    return Cell(pair).compatible;
}

const std::vector<SCouple>& CompTable::Conditions(const SCouple& pair) const
{
    return Cell(pair).conditions;
}

CompCell& CompTable::Cell(const SCouple& pair)
{
    return const_cast<CompCell&>(std::as_const(*this).Cell(pair));
}

const CompCell& CompTable::Cell(const SCouple& pair) const
{
    if (pair.First() < 1 || pair.Second() > stateCount_)
    {
        throw std::out_of_range(
            "Pair " + pair.ToString() + " is outside states 1.." + std::to_string(stateCount_));
    }

    return cells_[static_cast<std::size_t>(pair.Second() - 2)]
                 [static_cast<std::size_t>(pair.First() - 1)];
}

void CompTable::Print(std::ostream& output) const
{
    const int labelWidth = static_cast<int>(("s" + std::to_string(stateCount_)).size());

    // Ширина столбца si — максимум из заголовка и текстов ячеек под ним
    std::vector<int> widths;
    for (int column = 1; column < stateCount_; ++column)
    {
        std::size_t width = ("s" + std::to_string(column)).size();
        for (int row = column + 1; row <= stateCount_; ++row)
        {
            width = std::max(width, FormatCell(Cell(SCouple(column, row))).size());
        }
        widths.push_back(static_cast<int>(width));
    }

    output << std::left << std::setw(labelWidth) << "";
    for (int column = 1; column < stateCount_; ++column)
    {
        output << "  " << std::setw(widths[static_cast<std::size_t>(column - 1)])
               << "s" + std::to_string(column);
    }
    output << '\n';

    for (int row = 2; row <= stateCount_; ++row)
    {
        output << std::setw(labelWidth) << "s" + std::to_string(row);
        for (int column = 1; column < row; ++column)
        {
            output << "  " << std::setw(widths[static_cast<std::size_t>(column - 1)])
                   << FormatCell(Cell(SCouple(column, row)));
        }
        output << '\n';
    }

    output << std::right;
}
