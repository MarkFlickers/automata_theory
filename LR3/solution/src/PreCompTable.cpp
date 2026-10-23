#include "PreCompTable.hpp"

#include <algorithm>

namespace {

// Совместимы ли состояния по выходам: нет входа, на котором оба выхода
// определены и различны. Индексы состояний — с 0.
bool OutputsCompatible(const Table& outputs, int firstIndex, int secondIndex)
{
    for (int input = 0; input < outputs.InputCount(); ++input)
    {
        const TableItem& first = outputs.Item(input, firstIndex);
        const TableItem& second = outputs.Item(input, secondIndex);

        if (first.IsDefined() && second.IsDefined() && first.Value() != second.Value())
        {
            return false;
        }
    }

    return true;
}

}  // namespace

PreCompTable::PreCompTable(const Table& transitions, const Table& outputs)
    : CompTable(transitions.StateCount())
{
    // i, j — индексы столбцов (с 0); номера состояний в SCouple — i + 1, j + 1
    for (int i = 0; i < transitions.StateCount(); ++i)
    {
        for (int j = i + 1; j < transitions.StateCount(); ++j)
        {
            const SCouple pair(i + 1, j + 1);
            CompCell& cell = Cell(pair);

            if (!OutputsCompatible(outputs, i, j))
            {
                cell.compatible = false;
                continue;
            }

            for (int input = 0; input < transitions.InputCount(); ++input)
            {
                const TableItem& nextI = transitions.Item(input, i);
                const TableItem& nextJ = transitions.Item(input, j);

                // '-' можно доопределить соседним значением: условия нет
                if (!nextI.IsDefined() || !nextJ.IsDefined())
                {
                    continue;
                }

                // Оба перехода в одно состояние: тривиальная пара
                if (nextI.Value() == nextJ.Value())
                {
                    continue;
                }

                // Значения f — номера состояний (с 1), как и в SCouple.
                // SCouple нормализует порядок, поэтому (2,1) == (1,2).
                const SCouple condition(nextI.Value(), nextJ.Value());
                if (condition == pair)
                {
                    continue;
                }

                if (std::find(cell.conditions.begin(), cell.conditions.end(), condition) ==
                    cell.conditions.end())
                {
                    cell.conditions.push_back(condition);
                }
            }
        }
    }
}

std::string PreCompTable::FormatCell(const CompCell& cell) const
{
    if (!cell.compatible)
    {
        return "x";
    }

    if (cell.conditions.empty())
    {
        return "v";
    }

    std::string text;
    for (const SCouple& condition : cell.conditions)
    {
        text += condition.ToString();
    }

    return text;
}
