#pragma once

#include <string>
#include <vector>

// Функция комбинационной схемы: функция возбуждения D-триггера или выход автомата.
struct LogicFunction {
    std::string name;               // заголовок в output.txt, например "Q0" или "y1"
    std::string scale;              // СДНФ в виде шкалы для ЛР1: '0', '1', '-', длина 2^n
    std::vector<std::string> mdnf;  // МДНФ от ЛР1: импликанты вида "--10"
};
