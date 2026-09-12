#include "DNF.hpp"

#include <algorithm>
#include <cctype>
#include <limits>
#include <ostream>
#include <stdexcept>
#include <utility>

DNF::DNF(std::string scale)
{
    scale = NormalizeScale(std::move(scale));
    ValidateScale(scale);
    variableCount_ = DetermineVariableCount(scale.size());
    ParseScale(scale);
}

void DNF::Minimize()
{
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
    const ImplicantList &source) const
{
    ImplicantList requiredMinterms = ImplicantList();
    for (auto implicant : source)
    {
        if (implicant.isDontCare_ == true)
        {
            continue;
        }
        else
        {
            requiredMinterms.push_back(implicant);
        }
    }
    return requiredMinterms;
}

DNF::ImplicantList DNF::BuildPrimeImplicants(
    ImplicantList currentLevel) const
{
    ImplicantList primeImplicants = ImplicantList();
    ImplicantList nextLevel = ImplicantList();

    bool is_combinable = true;
    while (is_combinable)
    {
        nextLevel = BuildNextCombinationLevel(currentLevel);
        AppendUncombinedImplicants(currentLevel, primeImplicants);
        if (nextLevel.size() == 0)
        {
            is_combinable = false;
            break;
        }
        currentLevel = nextLevel;
    }
    return primeImplicants;
}

DNF::ImplicantList DNF::BuildNextCombinationLevel(
    ImplicantList &currentLevel) const
{
    ImplicantList newLevel = ImplicantList();

    size_t levelSize = currentLevel.size();
    for (size_t i = 0; i < levelSize; i++)
    {
        for (size_t j = i + 1; j < levelSize; j++)
        {
            Impl patchedImpl = Impl::Patch(currentLevel[i], currentLevel[j]);
            if (patchedImpl.number_ != -1)
            {
                AppendUniqueImplicant(newLevel, patchedImpl);
            }
        }
    }
    return newLevel;
}

void DNF::AppendUncombinedImplicants(
    const ImplicantList &currentLevel,
    ImplicantList &primeImplicants) const
{
    for (auto implicant : currentLevel)
    {
        if (implicant.wasCombined_ == false)
        {
            AppendUniqueImplicant(primeImplicants, implicant);
        }
    }
}

void DNF::AppendUniqueImplicant(
    ImplicantList &destination,
    const Impl &candidate) const
{
    bool is_unique = 1;
    for (auto implicant : destination)
    {
        if ((candidate.number_ == implicant.number_) && (candidate.mask_ == implicant.mask_))
        {
            is_unique = 0;
            break;
        }
    }

    if (is_unique)
    {
        destination.push_back(candidate);
    }
}

DNF::CoverageTable DNF::BuildCoverageTable(
    const ImplicantList &primeImplicants,
    const ImplicantList &requiredMinterms) const
{
    CoverageTable covTable(primeImplicants.size(), std::vector<bool>(requiredMinterms.size()));
    for (size_t i = 0; i < primeImplicants.size(); i++)
    {
        auto &primeImplicant = primeImplicants[i];
        for (size_t j = 0; j < requiredMinterms.size(); j++)
        {
            auto &requiredMinterm = requiredMinterms[j];
            if (Covers(primeImplicant, requiredMinterm))
            {
                covTable[i][j] = true;
            }
        }
    }
    return covTable;
}

bool DNF::Covers(const Impl &primeImplicant, const Impl &minterm) const
{
    if ((minterm.number_ & (~primeImplicant.mask_)) == primeImplicant.number_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

DNF::IndexList DNF::SelectMinimumCover(
    const CoverageTable &coverageTable,
    const ImplicantList &primeImplicants) const
{
    IndexList minimumCover = IndexList();

    IndexList essentialCover = FindEssentialPrimeIndices(coverageTable);
    IndexList additionalCover = FindMinimumAdditionalCover(coverageTable, primeImplicants, essentialCover);

    minimumCover = essentialCover;
    for(auto additionalRow : additionalCover)
    {
        push_back_if_unique(minimumCover, additionalRow);
    }
    return minimumCover;
}

DNF::IndexList DNF::FindEssentialPrimeIndices(
    const CoverageTable &coverageTable) const
{
    IndexList essentialPrimeIndices = IndexList();
    if (coverageTable.size() == 0)
    {
        return essentialPrimeIndices;
    }
    for (size_t requiredColumnIndex = 0; requiredColumnIndex < coverageTable[0].size(); requiredColumnIndex++)
    {
        size_t times_covered = 0;
        size_t primeRowIndexUnique = 0;
        for (size_t primeRowIndex = 0; primeRowIndex < coverageTable.size(); primeRowIndex++)
        {
            if (coverageTable[primeRowIndex][requiredColumnIndex] == 1)
            {
                times_covered++;
                primeRowIndexUnique = primeRowIndex;
            }
        }
        if (times_covered == 1)
        {
            push_back_if_unique(essentialPrimeIndices, primeRowIndexUnique);
        }
    }

    return essentialPrimeIndices;
}

DNF::IndexList DNF::FindMinimumAdditionalCover(
    const CoverageTable &coverageTable,
    const ImplicantList &primeImplicants,
    const IndexList &essentialPrimeIndices) const
{
    IndexList AdditionalCover = IndexList();
    IndexList uncoveredColumns = IndexList();

    if (coverageTable.size() == 0)
    {
        return AdditionalCover;
    }

    for (size_t requiredColumnIndex = 0; requiredColumnIndex < coverageTable[0].size(); requiredColumnIndex++)
    {
        bool is_covered_by_essential = false;
        for (auto essentialPrimeIndex : essentialPrimeIndices)
        {
            if (coverageTable[essentialPrimeIndex][requiredColumnIndex] == true)
            {
                is_covered_by_essential = true;
                continue;
            }
        }
        if (is_covered_by_essential == false)
        {
            uncoveredColumns.push_back(requiredColumnIndex);
        }
    }

    if (uncoveredColumns.size() == 0)
    {
        return AdditionalCover;
    }

    IndexList nonEssentialRows = IndexList();
    for (size_t rowIndex = 0; rowIndex < coverageTable.size(); rowIndex++)
    {
        nonEssentialRows.push_back(rowIndex);
    }
    for (auto essentialPrimeIndex : essentialPrimeIndices)
    {
        std::erase(nonEssentialRows, essentialPrimeIndex);
    }

    if (uncoveredColumns.empty())
    {
        return AdditionalCover;
    }

    for (size_t targetSize = 1; targetSize <= nonEssentialRows.size(); targetSize++)
    {
        IndexList currentRows;

        auto enumerate = [&](auto &&self, std::size_t start) -> bool
        {
            if (currentRows.size() == targetSize)
            {
                if (are_columns_covered_by_rows(coverageTable, uncoveredColumns, currentRows))
                {
                    AdditionalCover = currentRows;
                    return true;
                }

                return false;
            }

            const std::size_t needed =
                targetSize - currentRows.size();

            for (std::size_t position = start;
                 position < nonEssentialRows.size();
                 ++position)
            {
                const std::size_t remaining =
                    nonEssentialRows.size() - position;

                if (remaining < needed)
                {
                    break;
                }

                currentRows.push_back(nonEssentialRows[position]);

                if (self(self, position + 1))
                {
                    return true;
                }

                currentRows.pop_back();
            }

            return false;
        };

        if (enumerate(enumerate, 0))
        {
            return AdditionalCover;
        }
    }

    return AdditionalCover;
}

DNF::ImplicantList DNF::CollectSelectedImplicants(
    const ImplicantList &primeImplicants,
    const IndexList &selectedIndices) const
{
    ImplicantList MDNF = ImplicantList();

    for(auto selectedIndex : selectedIndices)
    {
        MDNF.push_back(primeImplicants[selectedIndex]);
    }
    return MDNF;
}

void DNF::Print(std::ostream &output) const
{
    for (const Impl &impl : data_)
    {
        if (impl.isDontCare_)
        {
            continue;
        }

        PrintImpl(output, impl);
        output << '\n';
    }
}

bool DNF::are_columns_covered_by_rows(const CoverageTable &coverageTable, const IndexList &selectedColumns, const IndexList &selectedRows) const
{
    bool are_covered = true;
    for(auto selectedColumn : selectedColumns)
    {
        bool is_column_covered = false;
        for(auto selectedRow : selectedRows)
        {
            if(coverageTable[selectedRow][selectedColumn] == true)
            {
                is_column_covered = true;
                break;
            }
        }
        if(is_column_covered == false)
        {
            are_covered = false;
            break;
        }
    }

    return are_covered;
}

Impl &DNF::GetImpl(int index)
{
    if (index < 0)
    {
        throw std::out_of_range("Implicant index cannot be negative");
    }

    return data_.at(static_cast<std::size_t>(index));
}

const Impl &DNF::GetImpl(int index) const
{
    if (index < 0)
    {
        throw std::out_of_range("Implicant index cannot be negative");
    }

    return data_.at(static_cast<std::size_t>(index));
}

std::string DNF::NormalizeScale(std::string scale)
{
    scale.erase(
        std::remove_if(
            scale.begin(),
            scale.end(),
            [](unsigned char symbol)
            { return std::isspace(symbol) != 0; }),
        scale.end());

    return scale;
}

void DNF::ValidateScale(const std::string &scale)
{
    if (scale.empty())
    {
        throw std::invalid_argument("Scale cannot be empty");
    }

    const bool hasInvalidSymbol = std::any_of(
        scale.begin(),
        scale.end(),
        [](char symbol)
        { return symbol != '0' && symbol != '1' && symbol != '-'; });

    if (hasInvalidSymbol)
    {
        throw std::invalid_argument("Scale may contain only '0', '1', and '-'");
    }

    if ((scale.size() & (scale.size() - 1)) != 0)
    {
        throw std::invalid_argument("Scale length must be a power of two");
    }

    if (scale.size() > static_cast<std::size_t>(std::numeric_limits<int>::max()))
    {
        throw std::invalid_argument("Scale is too large");
    }
}

int DNF::DetermineVariableCount(std::size_t scaleLength)
{
    int variableCount = 0;

    while (scaleLength > 1)
    {
        scaleLength /= 2;
        ++variableCount;
    }

    return variableCount;
}

void DNF::ParseScale(const std::string &scale)
{
    data_.reserve(scale.size());

    for (std::size_t index = 0; index < scale.size(); ++index)
    {
        const char state = scale[index];
        if (state == '0')
        {
            continue;
        }

        data_.push_back(Impl(static_cast<int>(index), state == '-'));
    }
}

void DNF::PrintImpl(std::ostream &output, const Impl &impl) const
{
    const unsigned int number = static_cast<unsigned int>(impl.number_);
    const unsigned int mask = static_cast<unsigned int>(impl.mask_);

    for (int bit = variableCount_ - 1; bit >= 0; --bit)
    {
        const unsigned int bitMask = 1U << bit;

        if ((mask & bitMask) != 0U)
        {
            output << '-';
        }
        else if ((number & bitMask) != 0U)
        {
            output << '1';
        }
        else
        {
            output << '0';
        }
    }
}
