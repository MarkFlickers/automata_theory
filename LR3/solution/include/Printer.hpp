#pragma once

#include "Table.hpp"

#include <iosfwd>
#include <string>

// Проверяет, что f и g описывают один автомат: размеры совпадают,
// определённые переходы f ссылаются на существующие состояния 1..StateCount().
void ValidateAutomaton(const Table& transitions, const Table& outputs);

// Печатает таблицы f и g рядом, как в методичке:
//
//          f                 g
//      s1 s2 s3 ...      s1 s2 s3 ...
//  x1   2  3  3 ...       1  1  1 ...
//
// statePrefix — подпись столбцов и значений f: "s" даёт s1, s2, ...
// Значения f печатаются числом без префикса, kUndefinedValue — как '-'.
void PrintAutomaton(
    std::ostream& output,
    const Table& transitions,
    const Table& outputs,
    const std::string& statePrefix = "s");
