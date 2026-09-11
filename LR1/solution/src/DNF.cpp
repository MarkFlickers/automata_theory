#include "DNF.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

DNF::DNF(std::string scale) {
    scale = NormalizeScale(std::move(scale));
    ValidateScale(scale);
    variableCount_ = DetermineVariableCount(scale.size());
    ParseScale(scale);
}

void DNF::Minimize() {
    const ImplicantList sourceImplicants = data_;
    const ImplicantList requiredMinterms = ExtractRequiredMinterms(sourceImplicants);
    const ImplicantList primeImplicants = BuildPrimeImplicants(sourceImplicants);
    const CoverageTable coverageTable =
        BuildCoverageTable(primeImplicants, requiredMinterms);
    const IndexList selectedIndices =
        SelectMinimumCover(coverageTable, primeImplicants);

    data_ = CollectSelectedImplicants(primeImplicants, selectedIndices);
}

DNF::ImplicantList DNF::ExtractRequiredMinterms(
    const ImplicantList& source) const {
    static_cast<void>(source);

    // TODO: Вернуть только импликанты, соответствующие единицам исходной шкалы.
    return {};
}

DNF::ImplicantList DNF::BuildPrimeImplicants(
    ImplicantList currentLevel) const {
    static_cast<void>(currentLevel);

    // TODO: Последовательно выполнять склейки до получения всех простых импликант.
    return {};
}

DNF::ImplicantList DNF::BuildNextCombinationLevel(
    ImplicantList& currentLevel) const {
    static_cast<void>(currentLevel);

    // TODO: Склеить все допустимые пары текущего уровня через Impl::Patch.
    return {};
}

void DNF::AppendUncombinedImplicants(
    const ImplicantList& currentLevel,
    ImplicantList& primeImplicants) const {
    static_cast<void>(currentLevel);
    static_cast<void>(primeImplicants);

    // TODO: Добавить несклеенные импликанты текущего уровня в список простых.
}

void DNF::AppendUniqueImplicant(
    ImplicantList& destination,
    const Impl& candidate) const {
    static_cast<void>(destination);
    static_cast<void>(candidate);

    // TODO: Добавить импликанту, если пары номер-маска ещё нет в списке.
}

DNF::CoverageTable DNF::BuildCoverageTable(
    const ImplicantList& primeImplicants,
    const ImplicantList& requiredMinterms) const {
    static_cast<void>(primeImplicants);
    static_cast<void>(requiredMinterms);

    // TODO: Построить таблицу покрытия простыми импликантами обязательных минтермов.
    return {};
}

bool DNF::Covers(const Impl& primeImplicant, const Impl& minterm) const {
    static_cast<void>(primeImplicant);
    static_cast<void>(minterm);

    // TODO: Реализовать условие покрытия N & (~P) == Nт.
    return false;
}

DNF::IndexList DNF::SelectMinimumCover(
    const CoverageTable& coverageTable,
    const ImplicantList& primeImplicants) const {
    static_cast<void>(coverageTable);
    static_cast<void>(primeImplicants);

    // TODO: Объединить обязательные импликанты с минимальным дополнительным покрытием.
    return {};
}

DNF::IndexList DNF::FindEssentialPrimeIndices(
    const CoverageTable& coverageTable) const {
    static_cast<void>(coverageTable);

    // TODO: Найти строки, которые единственными покрывают хотя бы один столбец.
    return {};
}

DNF::IndexList DNF::FindMinimumAdditionalCover(
    const CoverageTable& coverageTable,
    const ImplicantList& primeImplicants,
    const IndexList& essentialPrimeIndices) const {
    static_cast<void>(coverageTable);
    static_cast<void>(primeImplicants);
    static_cast<void>(essentialPrimeIndices);

    // TODO: Найти наименьший набор строк для ещё не покрытых столбцов.
    return {};
}

DNF::ImplicantList DNF::CollectSelectedImplicants(
    const ImplicantList& primeImplicants,
    const IndexList& selectedIndices) const {
    static_cast<void>(primeImplicants);
    static_cast<void>(selectedIndices);

    // TODO: Сформировать итоговую МДНФ по выбранным индексам простых импликант.
    return {};
}

void DNF::Print(std::ostream& output) const {
    for (const Impl& impl : data_) {
        if (impl.isDontCare_) {
            continue;
        }

        PrintImpl(output, impl);
        output << '\n';
    }
}

Impl& DNF::GetImpl(int index) {
    if (index < 0) {
        throw std::out_of_range("Implicant index cannot be negative");
    }

    return data_.at(static_cast<std::size_t>(index));
}

const Impl& DNF::GetImpl(int index) const {
    if (index < 0) {
        throw std::out_of_range("Implicant index cannot be negative");
    }

    return data_.at(static_cast<std::size_t>(index));
}

std::string DNF::NormalizeScale(std::string scale) {
    scale.erase(
        std::remove_if(
            scale.begin(),
            scale.end(),
            [](unsigned char symbol) { return std::isspace(symbol) != 0; }),
        scale.end());

    return scale;
}

void DNF::ValidateScale(const std::string& scale) {
    if (scale.empty()) {
        throw std::invalid_argument("Scale cannot be empty");
    }

    const bool hasInvalidSymbol = std::any_of(
        scale.begin(),
        scale.end(),
        [](char symbol) { return symbol != '0' && symbol != '1' && symbol != '-'; });

    if (hasInvalidSymbol) {
        throw std::invalid_argument("Scale may contain only '0', '1', and '-'");
    }

    if ((scale.size() & (scale.size() - 1)) != 0) {
        throw std::invalid_argument("Scale length must be a power of two");
    }

    if (scale.size() > static_cast<std::size_t>(std::numeric_limits<int>::max())) {
        throw std::invalid_argument("Scale is too large");
    }
}

int DNF::DetermineVariableCount(std::size_t scaleLength) {
    int variableCount = 0;

    while (scaleLength > 1) {
        scaleLength /= 2;
        ++variableCount;
    }

    return variableCount;
}

void DNF::ParseScale(const std::string& scale) {
    data_.reserve(scale.size());

    for (std::size_t index = 0; index < scale.size(); ++index) {
        const char state = scale[index];
        if (state == '0') {
            continue;
        }

        data_.push_back(Impl(static_cast<int>(index), state == '-'));
    }
}

void DNF::PrintImpl(std::ostream& output, const Impl& impl) const {
    const unsigned int number = static_cast<unsigned int>(impl.number_);
    const unsigned int mask = static_cast<unsigned int>(impl.mask_);

    for (int bit = variableCount_ - 1; bit >= 0; --bit) {
        const unsigned int bitMask = 1U << bit;

        if ((mask & bitMask) != 0U) {
            output << '-';
        } else if ((number & bitMask) != 0U) {
            output << '1';
        } else {
            output << '0';
        }
    }
}
