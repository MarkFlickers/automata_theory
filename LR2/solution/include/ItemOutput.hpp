#pragma once

#include "TableItem.hpp"

// Ячейка таблицы выходов g: номер выхода автомата.
class ItemOutput final : public TableItem {
public:
    explicit ItemOutput(int output);

    int Value() const override;
    bool IsDefined() const override;

private:
    int output_;
};
