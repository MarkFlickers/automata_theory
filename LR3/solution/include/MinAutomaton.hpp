#pragma once

#include "SMaxComp.hpp"
#include "Table.hpp"

#include <iosfwd>
#include <vector>

// Фаза 4: минимизированный автомат Мили.
// Состояние Ak — k-й класс отсортированного SMaxComp (нумерация с 1).
// В таблице переходов хранится номер k класса-приёмника, в таблице выходов — номер выхода.
class MinAutomaton {
public:
    // classes должен быть уже отсортирован: SMaxComp::Sort().
    MinAutomaton(const SMaxComp& classes, const Table& transitions, const Table& outputs);

    const Table& Transitions() const;
    const Table& Outputs() const;

    // Таблицы f и g рядом, столбцы A1, A2, ...
    void Print(std::ostream& output) const;

private:
    Table transitions_;
    Table outputs_;

    // Table не имеет конструктора по умолчанию, поэтому таблицы строятся
    // этими функциями прямо в списке инициализации конструктора.
    static Table BuildTransitions(
        const std::vector<std::vector<int>>& classes, const Table& transitions);
    static Table BuildOutputs(
        const std::vector<std::vector<int>>& classes, const Table& outputs);
};
