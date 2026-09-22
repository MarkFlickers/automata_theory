#include "StructSynthesis.hpp"

#include <stdexcept>
#include <ostream>
#include <utility>

StructSynthesis::StructSynthesis(Table transitions, Table outputs)
    : transitions_(std::move(transitions)),
      outputs_(std::move(outputs))
{
    if (transitions_.InputCount() != outputs_.InputCount() ||
        transitions_.StateCount() != outputs_.StateCount())
    {
        throw std::invalid_argument("Transition and output tables must have the same size");
    }
}

void StructSynthesis::EncodeStates()
{
    // TODO [1]: заполнить поля из StructSynthesis.hpp.
    //  1. Получить inputCount_ / stateCount_ из размеров transitions_.
    //  2. По outputs_ определить outputCount_ (здесь номера y1..y5 без пропусков).
    //     kUndefinedValue = 0 означает '-', его не считать выходом.
    //  3. Определить минимальное число бит для кодирования входов и состояний.
    //  4. Заполнить stateCodes_: прямой код состояния s — двоичная запись s - 1.
    //  5. Проверить, что определённые значения f лежат в 1..stateCount_.
    //
    // Индексы Table::Value(input, state) начинаются с 0, номера в файлах — с 1.
    // Ориентир для варианта 1: 7 входов, 8 состояний, 5 выходов, 3 D-триггера.
    // После реализации удали throw ниже. PrintEncoding() уже покажет результат.
    throw std::logic_error("TODO [1]: implement EncodeStates in src/StructSynthesis.cpp");
}

void StructSynthesis::PrintEncoding(std::ostream& output) const
{
    output << "Inputs: " << inputCount_ << ", states: " << stateCount_
           << ", outputs: " << outputCount_ << '\n'
           << "Input bits: " << inputBitCount_ << ", D flip-flops: " << stateBitCount_ << '\n';
    for (std::size_t index = 0; index < stateCodes_.size(); ++index) {
        output << "  s" << index + 1 << " -> " << stateCodes_[index] << '\n';
    }
}

void StructSynthesis::BuildSdnf()
{
    // TODO [2]: заполнить functions_, предварительно очистив старое содержимое.
    //  1. Создать функции Q0..Q(r-1), затем y1..yM (см. README).
    //  2. Для каждой функции создать шкалу длиной 2^(inputBitCount_ + stateBitCount_).
    //  3. Обойти пары (вход, текущее состояние), найти позицию в шкале.
    //  4. Для Qj взять бит j кода СЛЕДУЮЩЕГО состояния из f.
    //  5. Для yj поставить 1, если g == j, иначе 0; при g == '-' оставить '-'.
    //     При f == '-' оставить '-' только в Q-функциях: f и g независимы.
    //  6. Неиспользуемые коды входов оставить как '-'.
    // Пример добавления готовой шкалы: functions_.push_back({"Q0", scale, {}}).
    //
    // Формат шкалы — как scale.txt в ЛР1: строка из '0', '1', '-' длиной 2^n,
    // где n — число переменных. Символ с номером i — значение функции на наборе,
    // двоичная запись i которого совпадает с импликантой в output.txt
    // (крайняя левая переменная — старший бит).
    // Единый порядок для BuildSdnf и Check: [биты входа][биты текущего состояния].
    // Для варианта 1: a2 a1 a0 q2 q1 q0; код входа x — двоичная запись x - 1.
    // Ориентир: 8 функций, по 64 символа; набор x1,s1 — позиция 0 шкалы.
    // После реализации удали throw ниже; main покажет имена и длины шкал.
    throw std::logic_error("TODO [2]: implement BuildSdnf in src/StructSynthesis.cpp");
}

void StructSynthesis::Minimize(const Lab1Runner &lab1)
{
    if (functions_.empty())
    {
        throw std::logic_error("No SDNF to minimize: stage 2 (BuildSdnf) produced no functions");
    }

    for (LogicFunction &function : functions_)
    {
        function.mdnf = lab1.Minimize(function.name, function.scale);
    }
}

CheckWord StructSynthesis::Check(const CheckWord &expected) const
{
    // TODO [3]: моделировать автомат только по functions_[i].mdnf.
    //  1. Взять из expected входы и ТОЛЬКО первое состояние.
    //  2. Составить набор [код входа][код текущего состояния], как в BuildSdnf.
    //  3. Вычислить все Qj и yj на ОДНОМ наборе, до смены состояния.
    //     Импликанта истинна, если все её '0'/'1' совпадают с набором;
    //     '-' в импликанте пропускается. Функция — ИЛИ её импликант.
    //     Пустой список импликант — 0; импликанта из одних '-' — 1.
    //  4. Собрать код следующего состояния из Qj, перевести его в номер.
    //  5. По активной yj получить номер выхода j и сохранить шаг.
    //     Если активных yj нет или их несколько, сохранить kUndefinedValue.
    //     main примет его лишь там, где ожидается '-', иначе проверка не пройдёт.
    //  6. Вернуть X из k входов, S из k+1 состояний и Y из k выходов.
    // Не брать следующие состояния из expected или transitions_: это обойдёт
    // проверку синтеза. Ожидаемые S и Y сравнит main, '-' там означает любое значение.
    // Неопределённые выходы после минимизации могут дать неоднозначный набор yj;
    // правило его представления в CheckWord описано в README.
    // После реализации замени throw возвратом своего CheckWord.
    static_cast<void>(expected);
    throw std::logic_error("TODO [3]: implement Check in src/StructSynthesis.cpp");
}

const std::vector<LogicFunction> &StructSynthesis::Functions() const
{
    return functions_;
}
