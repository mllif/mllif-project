
namespace ns {
    struct record {
        [[mllic::export]] int foo(int a);
    };
}

int ns::record::foo(int a) {
    return a;
}
