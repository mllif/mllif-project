namespace ns {
    class [[mllif::export]] Foo {
        int _f;

      public:
        [[mllif::export]]
        int bar(int a);
    };
}

int ns::Foo::bar(int a) {
    return _f + a;
}
