#pragma once

class DNF;

class Impl {
public:
    explicit Impl(int number);
    ~Impl() = default;

    static Impl Patch(Impl& first, Impl& second);

private:
    int number_;
    int index_;
    int mask_;
    bool wasCombined_;
    bool isDontCare_;

    static bool is_patch_allowed_for(const Impl& first, const Impl& second);

    static unsigned int Count(unsigned int value);

    Impl(int number, bool isDontCare);

    friend class DNF;

};
