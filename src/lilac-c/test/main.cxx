
namespace TestNS
{
    struct [[lilac::export]] TestStruct {
        int A;
        int B;
        float C[100];

        [[lilac::export]] TestStruct TestFn();
    };

    struct [[lilac::export]] TestStruct2 {
        int A;
        int B;
        float C[100];
    };


    TestStruct TestStruct::TestFn() {
        return TestStruct();
    }
}

int main() {}
