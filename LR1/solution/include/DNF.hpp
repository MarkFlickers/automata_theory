#pragma once

#include "Impl.hpp"

#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>
#include <algorithm>

class DNF {
public:
    explicit DNF(std::string scale);
    ~DNF() = default;

    void Minimize();
    void Print(std::ostream& output) const;
    Impl& GetImpl(int index);
    const Impl& GetImpl(int index) const;

private:
    using ImplicantList = std::vector<Impl>;
    using CoverageTable = std::vector<std::vector<bool>>;
    using IndexList = std::vector<std::size_t>;

    ImplicantList data_;
    int variableCount_ = 0;

    static std::string NormalizeScale(std::string scale);
    static void ValidateScale(const std::string& scale);
    static int DetermineVariableCount(std::size_t scaleLength);

    void ParseScale(const std::string& scale);
    void PrintImpl(std::ostream& output, const Impl& impl) const;

    ImplicantList ExtractRequiredMinterms(const ImplicantList& source) const;
    ImplicantList BuildPrimeImplicants(ImplicantList currentLevel) const;
    ImplicantList BuildNextCombinationLevel(ImplicantList& currentLevel) const;
    void AppendUncombinedImplicants(
        const ImplicantList& currentLevel,
        ImplicantList& primeImplicants) const;
    void AppendUniqueImplicant(ImplicantList& destination, const Impl& candidate) const;

    CoverageTable BuildCoverageTable(
        const ImplicantList& primeImplicants,
        const ImplicantList& requiredMinterms) const;
    bool Covers(const Impl& primeImplicant, const Impl& minterm) const;
    bool are_columns_covered_by_rows(const CoverageTable &coverageTable, const IndexList &selectedColumns, const IndexList &selectedRows) const;
    IndexList SelectMinimumCover(
        const CoverageTable& coverageTable,
        const ImplicantList& primeImplicants) const;
    IndexList FindEssentialPrimeIndices(const CoverageTable& coverageTable) const;
    IndexList FindMinimumAdditionalCover(
        const CoverageTable& coverageTable,
        const ImplicantList& primeImplicants,
        const IndexList& essentialPrimeIndices) const;
    ImplicantList CollectSelectedImplicants(
        const ImplicantList& primeImplicants,
        const IndexList& selectedIndices) const;
};

template <typename T>
void push_back_if_unique(std::vector<T>& vec, const T& value) {
    // Если элемент НЕ найден, добавляем его
    if (std::find(vec.begin(), vec.end(), value) == vec.end()) {
        vec.push_back(value);
    }
}