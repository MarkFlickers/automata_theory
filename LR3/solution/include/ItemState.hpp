#pragma once

#include "TableItem.hpp"

// Ячейка таблицы переходов f: номер следующего состояния.
class ItemState final : public TableItem {
public:
    explicit ItemState(int nextState);

    int Value() const override;
    bool IsDefined() const override;

private:
    int nextState_;
};
