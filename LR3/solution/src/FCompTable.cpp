#include "FCompTable.hpp"

FCompTable::FCompTable(const PreCompTable& preTable)
    : CompTable(preTable)  // копия всех ячеек таблицы предварительной совместимости
{
    // Проходы повторяются, пока на очередном проходе что-то вычёркивается.
    // Цепочки условий не обходим: за один проход смотрим только прямые
    // условия ячейки, распространение идёт через повторные проходы.
    bool removeOccurred = false;
    do
    {
        removeOccurred = false;

        // i, j — номера состояний (с 1): Table здесь не используется
        for (int i = 1; i <= StateCount(); ++i)
        {
            for (int j = i + 1; j <= StateCount(); ++j)
            {
                CompCell& cell = Cell(SCouple(i, j));

                // Уже вычеркнута или безусловно совместима: менять нечего
                if (!cell.compatible || cell.conditions.empty())
                {
                    continue;
                }

                for (const SCouple& condition : cell.conditions)
                {
                    if (!IsCompatible(condition))
                    {
                        cell.compatible = false;
                        removeOccurred = true;
                        break;
                    }
                }
            }
        }
    } while (removeOccurred);
}

std::string FCompTable::FormatCell(const CompCell& cell) const
{
    return cell.compatible ? "v" : "x";
}
