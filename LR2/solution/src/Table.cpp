#include "Table.hpp"

#include <stdexcept>
#include <utility>

Table::Table(std::vector<Row> rows)
    : items_(std::move(rows))
{
    if (items_.empty() || items_.front().empty())
    {
        throw std::invalid_argument("Table cannot be empty");
    }

    for (const Row &row : items_)
    {
        if (row.size() != items_.front().size())
        {
            throw std::invalid_argument("All table rows must have the same length");
        }
    }
}

int Table::Value(int input, int state) const
{
    return Item(input, state).Value();
}

const TableItem &Table::Item(int input, int state) const
{
    if (input < 0 || state < 0)
    {
        throw std::out_of_range("Table index cannot be negative");
    }

    return *items_.at(static_cast<std::size_t>(input)).at(static_cast<std::size_t>(state));
}

int Table::InputCount() const
{
    return static_cast<int>(items_.size());
}

int Table::StateCount() const
{
    return static_cast<int>(items_.front().size());
}
