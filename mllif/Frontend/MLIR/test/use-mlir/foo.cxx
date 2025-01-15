namespace ns {
    struct Boo {
        float A, B;
    };

    struct Foo {
        [[mllif::export]] int foo(int a, Boo &b, int c[2]);
        [[mllif::export]] void chars(
            char a,
            char8_t b,
            char16_t c,
            char32_t d,
            wchar_t e);
        [[mllif::export]] void floats(
            double a,
            float b,
            _Float16 c);
        [[mllif::export]] void integers(
            int a,
            long b,
            long long c,
            short d,
            unsigned int e,
            unsigned long f,
            unsigned long long g,
            unsigned short h);
        [[mllif::export]] void boolean(bool b);
    };
}

int ns::Foo::foo(int a, Boo &b, int c[2]) {
    return a;
}
void ns::Foo::chars(
    char a,
    char8_t b,
    char16_t c,
    char32_t d,
    wchar_t e) {}
void ns::Foo::floats(
    double a,
    float b,
    _Float16 c) {}
void ns::Foo::integers(
    int a,
    long b,
    long long c,
    short d,
    unsigned int e,
    unsigned long f,
    unsigned long long g,
    unsigned short h) {}
void ns::Foo::boolean(bool b) {}