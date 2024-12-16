
namespace ns {
    struct record {
        [[polyglat::export]] int foo(int a);
    };
}

int ns::record::foo(int a) {
    return a;
}
