namespace ns {
    class [[mllif::export]] Foo {
        int _f;

      public:
        [[mllif::export]]
        Foo();
        [[mllif::export]]
        ~Foo();
        [[mllif::export]]
        int bar(int a);
    };
}

ns::Foo::Foo() {
    _f = 0;
}

ns::Foo::~Foo() {
    _f = 1;
}

int ns::Foo::bar(int a) {
    return _f + a;
}
