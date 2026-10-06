#pragma once

#include "TableItem.hpp"

#include <memory>
#include <vector>

// Таблица автомата: строки — входы x1..xN, столбцы — состояния s1..sM.
// Индексы в методах начинаются с нуля: Value(0, 0) — ячейка (x1, s1).
class Table {
public:
    using Row = std::vector<std::unique_ptr<TableItem>>;

    explicit Table(std::vector<Row> rows);

    int Value(int input, int state) const;
    const TableItem& Item(int input, int state) const;

    int InputCount() const;
    int StateCount() const;

private:
    std::vector<Row> items_;
};
