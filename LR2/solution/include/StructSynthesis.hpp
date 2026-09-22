#pragma once

#include "CheckWord.hpp"
#include "Lab1Runner.hpp"
#include "LogicFunction.hpp"
#include "Table.hpp"

#include <vector>
#include <iosfwd>
#include <string>

// Структурный синтез автомата Мили: таблицы f и g -> МДНФ функций
// возбуждения D-триггеров и выходов автомата.
class StructSynthesis {
public:
    StructSynthesis(Table transitions, Table outputs);

    // Этап 1: количество выходов, состояний, D-триггеров; кодирование состояний.
    void EncodeStates();

    // Готовая диагностика: показывает заполненные тобой поля этапа 1.
    void PrintEncoding(std::ostream& output) const;

    // Этап 2: СДНФ функций возбуждения D-триггеров и выходов -> functions_.
    void BuildSdnf();

    // Этап 3: минимизация каждой СДНФ программой из ЛР1.
    void Minimize(const Lab1Runner& lab1);

    // Проверка: работа автомата по полученным МДНФ на входах проверочного слова.
    CheckWord Check(const CheckWord& expected) const;

    const std::vector<LogicFunction>& Functions() const;

private:
    Table transitions_;  // f: номер следующего состояния
    Table outputs_;      // g: номер выхода

    // TODO [1]: заполнить эти поля в EncodeStates(). Соглашения см. README.
    int inputCount_ = 0;       // Количество строк таблицы (7 для варианта 1).
    int stateCount_ = 0;       // Количество столбцов (8).
    int outputCount_ = 0;      // Количество выходных сигналов y1..yM (5).
    int inputBitCount_ = 0;    // Биты двоичного кода входа (3).
    int stateBitCount_ = 0;    // Число D-триггеров (3).
    // stateCodes_[s - 1] — код состояния s, старший бит слева:
    // s1 -> "000", ..., s8 -> "111". Номер состояния не равен его коду!
    std::vector<std::string> stateCodes_;

    // TODO [2]: заполнить в BuildSdnf(). Порядок: Q0..Q(r-1), y1..yM.
    // Q0 отвечает за младший бит СЛЕДУЮЩЕГО состояния; yj — отдельный выход.
    // После Minimize() поле mdnf каждой функции заполнится автоматически.
    std::vector<LogicFunction> functions_;
};
