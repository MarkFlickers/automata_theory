#include "MinAutomaton.hpp"

#include "ItemOutput.hpp"
#include "ItemState.hpp"
#include "Printer.hpp"

#include <algorithm>
#include <cstddef>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

namespace {

// Номер (с 1) класса-приёмника, содержащего next целиком: сначала текущий
// класс, затем все остальные в порядке сортировки. next отсортирован, без повторов.
int FindReceiver(
    const std::vector<std::vector<int>>& classes,
    std::size_t current,
    const std::vector<int>& next)
{
    const auto contains = [&next](const std::vector<int>& candidate) {
        return std::includes(candidate.begin(), candidate.end(), next.begin(), next.end());
    };

    if (contains(classes[current]))
    {
        return static_cast<int>(current) + 1;
    }

    for (std::size_t index = 0; index < classes.size(); ++index)
    {
        if (index != current && contains(classes[index]))
        {
            return static_cast<int>(index) + 1;
        }
    }

    throw std::logic_error(
        "No compatibility class contains the next states of A" + std::to_string(current + 1));
}

}  // namespace

MinAutomaton::MinAutomaton(const SMaxComp& classes, const Table& transitions, const Table& outputs)
    : transitions_(BuildTransitions(classes.Classes(), transitions)),
      outputs_(BuildOutputs(classes.Classes(), outputs))
{
}

const Table& MinAutomaton::Transitions() const
{
    return transitions_;
}

const Table& MinAutomaton::Outputs() const
{
    return outputs_;
}

void MinAutomaton::Print(std::ostream& output) const
{
    PrintAutomaton(output, transitions_, outputs_, "A");
}

Table MinAutomaton::BuildTransitions(
    const std::vector<std::vector<int>>& classes, const Table& transitions)
{
    // Table собирается по строкам, а строка — вход: снаружи цикл по входам
    std::vector<Table::Row> rows;

    for (int input = 0; input < transitions.InputCount(); ++input)
    {
        Table::Row row;

        for (std::size_t current = 0; current < classes.size(); ++current)
        {
            // next = f(x, Ak): определённые переходы состояний класса (номера с 1)
            std::vector<int> next;
            for (int state : classes[current])
            {
                const TableItem& item = transitions.Item(input, state - 1);
                if (item.IsDefined())
                {
                    next.push_back(item.Value());
                }
            }

            // std::includes требует сортировки и считает повторы: {3,3} не входит в {2,3}
            std::sort(next.begin(), next.end());
            next.erase(std::unique(next.begin(), next.end()), next.end());

            const int receiver =
                next.empty() ? kUndefinedValue : FindReceiver(classes, current, next);
            row.push_back(std::make_unique<ItemState>(receiver));
        }

        rows.push_back(std::move(row));
    }

    return Table(std::move(rows));
}

Table MinAutomaton::BuildOutputs(
    const std::vector<std::vector<int>>& classes, const Table& outputs)
{
    std::vector<Table::Row> rows;

    for (int input = 0; input < outputs.InputCount(); ++input)
    {
        Table::Row row;

        for (std::size_t current = 0; current < classes.size(); ++current)
        {
            // Единственный определённый выход класса; '-', если не задан ни у кого
            int output = kUndefinedValue;
            for (int state : classes[current])
            {
                const TableItem& item = outputs.Item(input, state - 1);
                if (!item.IsDefined())
                {
                    continue;
                }

                // Класс совместим, значит совместим и по выходам
                if (output != kUndefinedValue && output != item.Value())
                {
                    throw std::logic_error(
                        "Class A" + std::to_string(current + 1) +
                        " has different outputs on x" + std::to_string(input + 1));
                }
                output = item.Value();
            }

            row.push_back(std::make_unique<ItemOutput>(output));
        }

        rows.push_back(std::move(row));
    }

    return Table(std::move(rows));
}
