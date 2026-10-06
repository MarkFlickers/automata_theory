#include "FileIO.hpp"
#include "Printer.hpp"
#include "Table.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>

int main() {
    try {
        const Table transitions = ReadTransitionTable("f_table.txt");
        const Table outputs = ReadOutputTable("g_table.txt");
        ValidateAutomaton(transitions, outputs);

        std::cout << "Source Mealy machine: " << transitions.InputCount() << " inputs, "
                  << transitions.StateCount() << " states\n";
        PrintAutomaton(std::cout, transitions, outputs);

        // Фазы алгоритма Пола - Ангера. Классы появятся после обсуждения
        // (см. README, раздел «Что предстоит реализовать»).
        std::cout << "\n[1/4] Pre-compatibility table (PreCompTable)\n" << std::flush;
        // TODO [1]: PreCompTable preTable(transitions, outputs); вывести таблицу.

        std::cout << "[2/4] Full compatibility table (FCompTable)\n" << std::flush;
        // TODO [2]: FCompTable fullTable(preTable); вывести таблицу.

        std::cout << "[3/4] Maximal compatibility classes (SMaxComp)\n" << std::flush;
        // TODO [3]: SMaxComp classes(fullTable); classes.Sort(); вывести классы.

        std::cout << "[4/4] Minimized machine\n" << std::flush;
        // TODO [4]: построить таблицы f и g минимизированного автомата
        //           и вывести их через PrintAutomaton(std::cout, f, g, "A").

        throw std::logic_error("TODO [1]: Paull-Unger phases are not implemented yet");
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
