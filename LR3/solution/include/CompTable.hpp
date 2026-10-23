#pragma once

#include "SCouple.hpp"

#include <iosfwd>
#include <string>
#include <vector>

// Ячейка таблицы совместимости для одной пары состояний.
//   compatible == false              -> пара несовместима ('x')
//   compatible && conditions.empty() -> совместима безусловно ('v')
//   compatible && !conditions.empty() -> совместима при условии совместимости этих пар
struct CompCell {
    bool compatible = true;
    std::vector<SCouple> conditions;
};

// Абстрактная треугольная таблица совместимости: по ячейке на каждую пару (si, sj), i < j.
//
//         s1   s2   s3   s4
//   s2    .
//   s3    .    .
//   s4    .    .    .
//   s5    .    .    .    .
//
// Хранилище, доступ и печать общие. Наследники различаются тем, как заполняют
// ячейки (конструктор) и как печатают одну ячейку (FormatCell).
class CompTable {
public:
    virtual ~CompTable() = default;

    int StateCount() const;
    bool IsCompatible(const SCouple& pair) const;
    const std::vector<SCouple>& Conditions(const SCouple& pair) const;

    // Рисует треугольник; текст каждой ячейки даёт FormatCell наследника.
    void Print(std::ostream& output) const;

protected:
    // Все ячейки создаются совместимыми и без условий.
    explicit CompTable(int stateCount);

    // Копирование для наследников: FCompTable(pre) : CompTable(pre) копирует все ячейки.
    CompTable(const CompTable&) = default;
    CompTable& operator=(const CompTable&) = default;

    // Доступ к ячейке пары; изменять ячейки могут только наследники.
    CompCell& Cell(const SCouple& pair);
    const CompCell& Cell(const SCouple& pair) const;

    // Текст одной ячейки для Print(). Чисто виртуальная: делает класс абстрактным.
    virtual std::string FormatCell(const CompCell& cell) const = 0;

private:
    int stateCount_;
    // cells_[j - 2][i - 1] — ячейка пары (si, sj), i < j. В строке j ровно j - 1 ячейка.
    std::vector<std::vector<CompCell>> cells_;
};
