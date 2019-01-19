#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <fstream>
#include <cassert>

#include "my_function.h"

using namespace std;

int foo() {
    return 1;
}

int bar() {
    return 2;
}

double pi() {
    // kak grubo
    return 3.14;
}

void test_defaultConstructor() {
    my_function<int(void)> f(foo);
    assert(f() == 1);
}

void test_copyConstructor() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second(b);
    assert(second() == 2);
}

void test_nullptrConstructor() {
    my_function<void(void)> f(nullptr);
}

void test_moveConstructor() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second(std::move(b));
    assert(second() == 2);
}

void test_operatorAssignment() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second = b;
    assert(second() == 2);
}

void test_moveAssignment() {
    my_function<int(void)> b(bar);
    my_function<int(void)> second(foo);
    second = std::move(b);
    assert(second() == 2);
}

void test_explicitOperatorBool() {
    my_function<int(void)> f(nullptr);
    assert(!f);
    f = foo;
    assert(f);
}

void test_lambda() {
    int a = 10;
    my_function<int(int)> l = [a](int x) {
        return a + x;
    };
    assert(l(5) == 15);
}

void test_swap() {
    my_function<int()> f(foo);
    my_function<int()> b(bar);
    assert(f() == 1);
    assert(b() == 2);

    f.swap(b);

    assert(f() == 2);
    assert(b() == 1);
}

void test_diffTypes() {
    my_function<int()> f = foo;
    assert(f() == 1);
    f = pi;
    assert(pi() == 3.14);
}


void NIKITOZZZZ_test() {
    int foo = 1;
    double bar = 3;
    double bar2 = 3;
    double bar3 = 3;

    my_function<int (std::ostream &)> f([=](std::ostream &os) mutable {
        os << "test " << foo << " " << bar << std::endl;
        os << "test " << bar2 << " " << bar3 << std::endl;
        foo *= 2;
        foo += 2;
        bar -= 0.1;
        os << "test " << foo << " " << bar << std::endl;
        return foo;
    });

    f(std::cout);
}


void all_test() {
    test_defaultConstructor();
    test_copyConstructor();
    test_nullptrConstructor();
    test_moveConstructor();
    test_operatorAssignment();
    test_moveAssignment();
    test_explicitOperatorBool();
    test_swap();
    test_lambda();
    test_diffTypes();
    NIKITOZZZZ_test();
}

int main() {
    all_test();
    return 0;
}