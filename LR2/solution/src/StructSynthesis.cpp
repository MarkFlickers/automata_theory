#include "StructSynthesis.hpp"

#include <stdexcept>
#include <ostream>
#include <utility>
#include <bit>
#include <bitset>
#include <math.h>

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

    inputCount_ = transitions_.InputCount();
    stateCount_ = transitions_.StateCount();
    outputCount_ = CalculateOutputsCount(outputs_);
    stateBitCount_ = CalculateBitCount(stateCount_);
    inputBitCount_ = CalculateBitCount(inputCount_);

    stateCodes_.clear();
    for(int stateNum = 0; stateNum < stateCount_; stateNum++)
    {
        const std::string fullCode = std::bitset<32>(stateNum).to_string();
        stateCodes_.push_back(fullCode.substr(32 - stateBitCount_));
    }

    for(int state = 0; state < transitions_.StateCount(); state++)
    {
        for(int input = 0; input < transitions_.InputCount(); input++)
        {
            const TableItem& item = transitions_.Item(input, state);
            if(item.IsDefined())
            {
                if((item.Value() < 1) || (item.Value() > stateCount_) )
                {
                    throw std::invalid_argument("Transiton table have transition to incorrect state");
                }
            }
        }
    }

}

int StructSynthesis::CalculateBitCount(unsigned int stateCount)
{
    int bitCount = std::bit_width(stateCount - 1);

    return bitCount;
}

int StructSynthesis::CalculateOutputsCount(const Table &outputs)
{
    int maxOutput = 0;
    for(int state = 0; state < outputs.StateCount(); state++)
    {
        for(int input = 0; input < outputs.InputCount(); input++)
        {
            const TableItem& item = outputs.Item(input, state);
            if(item.IsDefined())
            {
                maxOutput = maxOutput > item.Value() ? maxOutput : item.Value();
            }
        }
    }

    return maxOutput;
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

    PrefillFunctions();

    for(int input = 0; input < inputCount_; input++)
    {
        for(int state = 0; state < stateCount_; state++)
        {
            const TableItem& transitionItem = transitions_.Item(input, state);
            if(transitionItem.IsDefined())
            {
                for(int i = 0; i < stateBitCount_; i++)
                {
                    functions_[i].scale[input * stateCount_ + state] = stateCodes_[transitionItem.Value() - 1][stateBitCount_ - i - 1];
                }
            }

            const TableItem& outputItem = outputs_.Item(input, state);
            if(outputItem.IsDefined())
            {
                for(int i = stateBitCount_; i < (stateBitCount_ + outputCount_); i++)
                {
                    functions_[i].scale[input * stateCount_ + state] = outputItem.Value() == (i - stateBitCount_ + 1) ? '1' : '0';
                }
            }
        }
    }
}

void StructSynthesis::PrefillFunctions(void)
{
    std::string filler(pow(2, inputBitCount_ + stateBitCount_), '-');
    functions_.clear();

    for(int i = 0; i < stateBitCount_; i++)
    {
        std::string name("Q" + std::to_string(i));
        LogicFunction Q = {
            .name = name,
            .scale = filler,
            .mdnf = {},
        };
        functions_.push_back(Q);
    }

    for(int i = 1; i < outputCount_ + 1; i++)
    {
        std::string name("y" + std::to_string(i));
        LogicFunction y = {
            .name = name,
            .scale = filler,
            .mdnf = {},
        };
        functions_.push_back(y);
    }

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

    CheckWord obtained = {
        .inputs = expected.inputs,
        .states = {},
        .outputs = {},
    };

    int currentState = expected.states[0];
    for(unsigned int i = 0; i < expected.inputs.size(); i++)
    {
        int currentInput = expected.inputs[i];
        const std::string fullStateCode = std::bitset<32>(currentState - 1).to_string();
        std::string stateCode = fullStateCode.substr(32 - stateBitCount_);
        const std::string fullInputCode = std::bitset<32>(currentInput - 1).to_string();
        std::string inputCode = fullInputCode.substr(32 - inputBitCount_);
        std::string inputSet = inputCode + stateCode;

        int newState = 0;
        for(int functionNum = 0; functionNum < stateBitCount_; functionNum++)
        {
            bool dnfResult = calculateDNF(functions_[functionNum].mdnf, inputSet);
            newState |= static_cast<int>(dnfResult) << functionNum;
        }
        newState += 1;

        int outputNum = kUndefinedValue;
        int activeCount = 0;
        for(int functionNum = stateBitCount_; functionNum < stateBitCount_ + outputCount_; functionNum++)
        {
            bool dnfResult = calculateDNF(functions_[functionNum].mdnf, inputSet);
            if(dnfResult == true)
            {
                activeCount++;
                outputNum = functionNum - stateBitCount_ + 1 ;
            }
        }
        if (activeCount != 1)
        {
            outputNum = kUndefinedValue;
        }

        obtained.outputs.push_back(outputNum);
        obtained.states.push_back(currentState);
        currentState = newState;
    }
    obtained.states.push_back(currentState);

    return obtained;
}

bool StructSynthesis::calculateDNF(std::vector<std::string> dnf, std::string inputs) const
{
    bool result = 0;

    for(auto implicant : dnf)
    {
        bool isImplicantTrue = true;
        if(implicant.size() != inputs.size())
        {
            throw std::invalid_argument("implicant length does not match input length");
        }
        for(std::size_t i = 0; i < implicant.size(); i++)
        {
            auto dnfLiteral = implicant[i];
            auto inputLiteral = inputs[i];
            if(dnfLiteral == '-')
            {
                continue;
            }
            if(inputLiteral != dnfLiteral)
            {
                isImplicantTrue = false;
            }
        }

        if(isImplicantTrue == true)
        {
            result = 1;
            break;
        }
    }

    return result;
}

const std::vector<LogicFunction> &StructSynthesis::Functions() const
{
    return functions_;
}
