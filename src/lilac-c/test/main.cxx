
struct __attribute__((lilac_export)) TestStruct {
  int A;
  int B;
  float C;
};

int TestFn(TestStruct *T) __attribute__((lilac_export)) {
  return T->A + T->B + T->C;
}

int main() {}
