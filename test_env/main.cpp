#include <vector>
#include <map>
#include <string>
#include <iostream>

class A {
    public:
        int     _test;
        A(int test): _test(test) {}
        ~A() {}
};

int     main() {
    A       value(10);
    std::vector<A>      test;
    test.push_back(value);

    std::cout << "first : " << value._test << std::endl; 
    std::cout << "vector1: " << test[0]._test << std::endl;
    value._test = 42; 
    std::cout << "second : " << value._test << std::endl; 
    std::cout << "vector2: " << test[0]._test << std::endl;
    return (0);
}