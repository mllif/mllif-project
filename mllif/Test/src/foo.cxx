
namespace ns {
    struct record {
        [[mllif::export]] int foo(int a, int b);
    };
}

int ns::record::foo(int a, int b) {
    return a;
}
