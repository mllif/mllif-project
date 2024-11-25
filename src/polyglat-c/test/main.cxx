
namespace TestNS
{
    struct [[polyglat::export]] TestStruct {
        int A;
        int B;
        float C[100];

        [[polyglat::export]] TestStruct TestFn();
    };

    struct [[polyglat::export]] TestStruct2 {
        int A;
        int B;
        float C[100];
    };


    TestStruct TestStruct::TestFn() {
        return TestStruct();
    }
}

int main() {}
