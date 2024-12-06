
[[polyglat::export]] int i32() { return 0; }
[[polyglat::export]] int i32_4(int a, int b, int c, int d) { return 0; }
[[polyglat::export]] int i32_8(int, int, int, int, int, int, int, int) { return 0; }

namespace ns {
    [[polyglat::export]] int i32() { return 0; }
    [[polyglat::export]] int i32_4(int a, int b, int c, int d) { return 0; }
    [[polyglat::export]] int i32_8(int, int, int, int, int, int, int, int) { return 0; }

    struct [[polyglat::export]] small_object {
        int self;

        small_object(){}
        ~small_object(){}

        [[polyglat::export]] small_object i32() { return *this; }
        [[polyglat::export]] small_object i32_4(int a, int b, int c, int d) { return *this; }
        [[polyglat::export]] small_object i32_8(int, int, int, int, int, int, int, int) { return *this; }
    };

    struct [[polyglat::export]] large_object {
        int self[999];

        large_object(){}

        [[polyglat::export]] large_object i32() { return *this; }
        [[polyglat::export]] large_object i32_4(int a, int b, int c, int d) { return *this; }
        [[polyglat::export]] large_object i32_8(int, int, int, int, int, int, int, int) { return *this; }
    };
}
