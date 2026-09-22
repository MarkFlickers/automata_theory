#include "Lab1Runner.hpp"

#include "FileIO.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <stdexcept>

#ifndef _WIN32
#include <sys/wait.h>
#endif

namespace {

bool IsValidName(const std::string &name)
{
    return !name.empty() &&
           std::all_of(name.begin(), name.end(), [](unsigned char symbol)
                       { return std::isalnum(symbol) != 0 || symbol == '_'; });
}

}  // namespace

Lab1Runner::Lab1Runner(
    const std::filesystem::path &executable,
    const std::filesystem::path &exchangeDirectory)
    : executable_(std::filesystem::absolute(executable).lexically_normal()),
      exchangeDirectory_(std::filesystem::absolute(exchangeDirectory).lexically_normal())
{
    if (!std::filesystem::is_regular_file(executable_))
    {
        throw std::runtime_error("LR1 executable not found: " + executable_.string());
    }

    std::filesystem::create_directories(exchangeDirectory_);
}

std::vector<std::string> Lab1Runner::Minimize(const std::string &name, const std::string &scale) const
{
    if (!IsValidName(name))
    {
        throw std::invalid_argument(
            "Function name must contain only letters, digits and '_': '" + name + "'");
    }

    const std::filesystem::path scalePath = exchangeDirectory_ / (name + "_scale.txt");
    const std::filesystem::path mdnfPath = exchangeDirectory_ / (name + "_mdnf.txt");

    WriteScale(scale, scalePath);
    std::filesystem::remove(mdnfPath);

    const int exitCode = Run(scalePath, mdnfPath);
    if (exitCode != 0)
    {
        throw std::runtime_error(
            "LR1 failed to minimize " + name + " (exit code " + std::to_string(exitCode) + ")");
    }

    return ReadMdnf(mdnfPath);
}

int Lab1Runner::Run(const std::filesystem::path &scalePath, const std::filesystem::path &mdnfPath) const
{
#ifdef _WIN32
    // cmd.exe снимает внешнюю пару кавычек, поэтому вся команда берётся в ещё одни кавычки
    const std::wstring command =
        L"\"\"" + executable_.wstring() + L"\" \"" + scalePath.wstring() +
        L"\" \"" + mdnfPath.wstring() + L"\"\"";

    return _wsystem(command.c_str());
#else
    const std::string command =
        "\"" + executable_.string() + "\" \"" + scalePath.string() +
        "\" \"" + mdnfPath.string() + "\"";

    const int status = std::system(command.c_str());
    return status != -1 && WIFEXITED(status) ? WEXITSTATUS(status) : -1;
#endif
}
