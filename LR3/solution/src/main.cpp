#include "FCompTable.hpp"
#include "FileIO.hpp"
#include "MinAutomaton.hpp"
#include "PreCompTable.hpp"
#include "Printer.hpp"
#include "SMaxComp.hpp"
#include "Table.hpp"

#include <exception>
#include <iostream>

int main() {
    try {
        const Table transitions = ReadTransitionTable("f_table.txt");
        const Table outputs = ReadOutputTable("g_table.txt");
        ValidateAutomaton(transitions, outputs);

        std::cout << "Source Mealy machine: " << transitions.InputCount() << " inputs, "
                  << transitions.StateCount() << " states\n";
        PrintAutomaton(std::cout, transitions, outputs);

        std::cout << "\n[1/4] Pre-compatibility table (x - incompatible, v - unconditional)\n"
                  << std::flush;
        const PreCompTable preTable(transitions, outputs);
        preTable.Print(std::cout);

        std::cout << "\n[2/4] Full compatibility table\n" << std::flush;
        const FCompTable fullTable(preTable);
        fullTable.Print(std::cout);

        std::cout << "\n[3/4] Maximal compatibility classes\n" << std::flush;
        SMaxComp classes(fullTable);
        classes.Print(std::cout);
        std::cout << "Sorted:\n" << std::flush;
        classes.Sort();
        classes.Print(std::cout);

        std::cout << "\n[4/4] Minimized machine\n" << std::flush;
        const MinAutomaton minimized(classes, transitions, outputs);
        minimized.Print(std::cout);

        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
