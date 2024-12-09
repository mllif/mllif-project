
[[polyglat::export]] int i32() { return 0; }
[[polyglat::export]] int i32_4(int a, int b, int c, int d) { return 0; }
[[polyglat::export]] int i32_8(int a, int b, int c, int d, int e, int f, int g, int h) { return 0; }

namespace ns {
    [[polyglat::export]] int i32() { return 0; }
    [[polyglat::export]] int i32_4(int a, int b, int c, int d) { return 0; }
    [[polyglat::export]] int i32_8(int a, int b, int c, int d, int e, int f, int g, int h) { return 0; }

    struct [[polyglat::export]] small_object {
        int self;

        small_object(){}
        ~small_object(){}

        [[polyglat::export]] small_object i32();
        [[polyglat::export]] small_object i32_4(int a, int b, int c, int d);
        [[polyglat::export]] small_object i32_8(int a, int b, int c, int d, int e, int f, int g, int h);
    };

    small_object small_object::i32() { return *this; }
    small_object small_object::i32_4(int a, int b, int c, int d) { return *this; }
    small_object small_object::i32_8(int a, int b, int c, int d, int e, int f, int g, int h) { return *this; }

    struct [[polyglat::export]] large_object {
        int self[999];

        large_object(){}

        [[polyglat::export]] large_object i32();
        [[polyglat::export]] large_object i32_4(int a, int b, int c, int d);
        [[polyglat::export]] large_object i32_8(int a, int b, int c, int d, int e, int f, int g, int h);
    };

    large_object large_object::i32() { return *this; }
    large_object large_object::i32_4(int a, int b, int c, int d) { return *this; }
    large_object large_object::i32_8(int a, int b, int c, int d, int e, int f, int g, int h) { return *this; }
}
