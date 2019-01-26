//
// Created by Nikitos on 19/01/2019.
//

#include "Function.h"
#include <iostream>
#include <cassert>
#include <functional>
#include <vector>

using namespace std;

int foo() {
    return 1;
}

int bar() {
    return 2;
}

double pi() {
    return 3.14;
}

double qqe(double a) {
    return a;
}

void test_defaultConstructor() {
    Function<int(void)> f(foo);
    assert(f() == 1);
}

void test_copyConstructor() {
    Function<int(void)> b(bar);
    Function<int(void)> second(b);
    assert(second() == 2);
}

void test_nullptrConstructor() {
    Function<void(void)> f(nullptr);
}

void test_nullptrFunction() {
    Function<void(void)> f(nullptr);
    Function<void(void)> p(f);
}

void test_moveConstructor() {
    Function<int(void)> b(bar);
    Function<int(void)> second(std::move(b));
    assert(second() == 2);
}

void test_operatorAssignment() {
    Function<int(void)> b(bar);
    Function<int(void)> second = b;
    assert(second() == 2);
}

void test_moveAssignment() {
    Function<int(void)> b(bar);
    Function<int(void)> second(foo);
    second = std::move(b);
    assert(second() == 2);
}

void test_explicitOperatorBool() {
    Function<int(void)> f(nullptr);
    assert(!f);
    f = foo;
    assert(f);
}

void test_lambda() {
    int a = 10;
    Function<int(int)> l = [a](int x) {
        return a + x;
    };
    assert(l(5) == 15);
}

void test_swap() {
    Function<int()> f(foo);
    Function<int()> b(bar);
    assert(f() == 1);
    assert(b() == 2);

    f.swap(b);

    assert(f() == 2);
    assert(b() == 1);
}

void test_diffTypes() {
    Function<int()> f = foo;
    assert(f() == 1);
    f = pi;
    assert(pi() == 3.14);
}

void test_copy() {
    std::vector<int> buffer(100, -1);
    Function<int()> g;
    {
        Function<int()> f = [buffer]() {
            return buffer[99];
        };
        Function<int()> t(f);
        g = f;
        Function<int()> h(f);
        assert(f() == -1);
        assert(g() == -1);
        assert(h() == -1);
    }
    assert(g() == -1);
}

void test_copy_small_object() {
    std::shared_ptr<std::vector<int>> buffer = std::make_shared<std::vector<int>>(100, -1);
    Function<int()> g;
    {
        Function<int()> f = [buffer]() {
            return (*buffer)[99];
        };
        g = f;
        Function<int()> h(f);
        assert(f() == -1);
        assert(g() == -1);
        assert(h() == -1);
    }
    assert(g() == -1);
}

void NIKITOZZZZ_test() {
    int foo = 1;
    double bar = 3;
    double bar2 = 3;
    double bar3 = 3;

    Function<int (std::ostream &)> f([=](std::ostream &os) mutable {
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

size_t res;

struct Func {
    size_t operator() () {
        res = 70;
        return res;
    }
};

void test70() {
    Func F12;
    Function<size_t ()> f12(F12);
    assert(f12() == 70);
}

void all_test() {
    test_defaultConstructor();
    test_copyConstructor();
    test_nullptrConstructor();
    test_nullptrFunction();
    test_moveConstructor();
    test_operatorAssignment();
    test_moveAssignment();
    test_explicitOperatorBool();
    test_swap();
    test_lambda();
    test_diffTypes();
    test_copy();
    test_copy_small_object();
    NIKITOZZZZ_test();
    test70();
    std::cout << "OK!";
}

int main() {
    all_test();
    return 0;
}