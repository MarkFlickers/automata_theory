#pragma once

// Значение ячейки, заданной в файле символом '-' (не определено).
inline constexpr int kUndefinedValue = 0;

class TableItem {
public:
    virtual ~TableItem() = default;

    virtual int Value() const = 0;
    virtual bool IsDefined() const = 0;
};
