#include "SMaxComp.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <map>
#include <ostream>
#include <utility>

namespace {

// Убирает классы, которые содержатся в другом классе.
// Из двух одинаковых классов остаётся первый.
void RemoveSubsumed(std::vector<std::vector<int>>& classes)
{
    std::vector<std::vector<int>> kept;

    for (std::size_t index = 0; index < classes.size(); ++index)
    {
        const std::vector<int>& candidate = classes[index];
        bool subsumed = false;

        for (std::size_t other = 0; other < classes.size() && !subsumed; ++other)
        {
            const std::vector<int>& container = classes[other];
            if (other == index ||
                !std::includes(container.begin(), container.end(), candidate.begin(), candidate.end()))
            {
                continue;
            }

            // Строгое подмножество лишнее всегда, копия — только если оригинал раньше
            subsumed = candidate.size() < container.size() || other < index;
        }

        if (!subsumed)
        {
            kept.push_back(candidate);
        }
    }

    classes = std::move(kept);
}

}  // namespace

SMaxComp::SMaxComp(const FCompTable& table)
{
    const int stateCount = table.StateCount();
    classes_.push_back({stateCount});

    // Столбцы справа налево: после шага i в classes_ лежат
    // классы максимальной совместимости среди состояний i..n
    for (int i = stateCount - 1; i >= 1; --i)
    {
        // Si — состояния правее i, совместимые с i (по возрастанию)
        std::vector<int> compatibleRight;
        for (int j = i + 1; j <= stateCount; ++j)
        {
            if (table.IsCompatible(SCouple(i, j)))
            {
                compatibleRight.push_back(j);
            }
        }

        // Новые классы копятся отдельно: добавлять в classes_ во время
        // обхода classes_ нельзя
        std::vector<std::vector<int>> newClasses;
        for (const std::vector<int>& existing : classes_)
        {
            std::vector<int> intersection;
            std::set_intersection(
                existing.begin(), existing.end(),
                compatibleRight.begin(), compatibleRight.end(),
                std::back_inserter(intersection));

            if (!intersection.empty())
            {
                // i меньше всех j > i: вставка в начало сохраняет сортировку
                intersection.insert(intersection.begin(), i);
                newClasses.push_back(std::move(intersection));
            }
        }

        // {i} останется, только если i не вошло ни в один класс выше
        newClasses.push_back({i});

        classes_.insert(classes_.end(), newClasses.begin(), newClasses.end());
        RemoveSubsumed(classes_);
    }
}

void SMaxComp::Sort()
{
    // Сколько классов содержит каждое состояние — считается до перестановок
    std::map<int, int> occurrences;
    for (const std::vector<int>& compClass : classes_)
    {
        for (int state : compClass)
        {
            ++occurrences[state];
        }
    }

    // Признак «есть уникальное состояние» едет вместе со своим классом
    std::vector<std::pair<bool, std::vector<int>>> keyed;
    for (std::vector<int>& compClass : classes_)
    {
        const bool hasUnique = std::any_of(compClass.begin(), compClass.end(),
            [&occurrences](int state) { return occurrences[state] == 1; });
        keyed.emplace_back(hasUnique, std::move(compClass));
    }

    // Один компаратор с тремя ключами: следующий ключ решает только при равенстве предыдущих
    std::sort(keyed.begin(), keyed.end(), [](const auto& left, const auto& right) {
        if (left.first != right.first)
        {
            return left.first;  // класс с уникальным состоянием раньше
        }
        if (left.second.size() != right.second.size())
        {
            return left.second.size() > right.second.size();  // больший раньше
        }
        return left.second < right.second;  // лексикографически
    });

    classes_.clear();
    for (auto& [hasUnique, compClass] : keyed)
    {
        classes_.push_back(std::move(compClass));
    }
}

const std::vector<std::vector<int>>& SMaxComp::Classes() const
{
    return classes_;
}

void SMaxComp::Print(std::ostream& output) const
{
    for (std::size_t index = 0; index < classes_.size(); ++index)
    {
        output << "A" << index + 1 << " = {";
        for (std::size_t position = 0; position < classes_[index].size(); ++position)
        {
            output << (position > 0 ? ", " : "") << "s" << classes_[index][position];
        }
        output << "}\n";
    }
}
