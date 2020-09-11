#include <core.h>

class Test {
  public:
  int i;
  Test(int j): i(j) {
    outStream << "Test() from C++\n";
  }
  ~Test() {
    outStream << "~Test() from C++, " << this->i << "\n";
  }
};

DL_EXPORTED SharedPtr<Test> getTestSrdRef() {
  return newSrdObj<Test>(61);
}

DL_EXPORTED void setTestSrdRef(SharedPtr<Test> r) {
  static SharedPtr<Test> ref;
  ref = r;
  if (r != 0) outStream << "setTestSrdRef from C++, " << r->i << "\n";
}

DL_EXPORTED SharedPtr<Test> setAndGetTestSrdRef(SharedPtr<Test> r) {
  outStream << "setAndGetTestSrdRef, " << r.getRefCounter()->count << " " << r->i << "\n";
  return r;
}

DL_EXPORTED Array<Int> getTestArray() {
  Array<Int> arr;
  arr.add(9);
  arr.add(22);
  arr.add(13);
  return arr;
}

DL_EXPORTED void printTestArray(Array<Int> arr) {
  for (Int i = 0; i < arr.getLength(); ++i) {
    outStream << arr(i) << "\n";
  }
}

DL_EXPORTED String getTestString() {
  return String("test string from c++");
}

DL_EXPORTED void printTestString(String str) {
  outStream << str << "\n";
}
