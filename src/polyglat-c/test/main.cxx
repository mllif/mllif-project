
namespace TestNS
{
    struct [[polyglat::export]] TestStruct {
        int A;
        int B;

        TestStruct(int a, int b) : A(a), B(b) {}
    };

    TestStruct TestFn() {
        return TestStruct(0, 0);
    }
}

int main() {}
