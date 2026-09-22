#pragma once

#include <filesystem>
#include <string>
#include <vector>

// Запускает программу из ЛР1 (метод Квайна - Мак-Класки) для одной СДНФ.
// Шкала и результат передаются через файлы <name>_scale.txt и <name>_mdnf.txt
// в папке exchangeDirectory.
class Lab1Runner {
public:
    Lab1Runner(
        const std::filesystem::path& executable,
        const std::filesystem::path& exchangeDirectory);

    std::vector<std::string> Minimize(const std::string& name, const std::string& scale) const;

private:
    std::filesystem::path executable_;
    std::filesystem::path exchangeDirectory_;

    int Run(const std::filesystem::path& scalePath, const std::filesystem::path& mdnfPath) const;
};
