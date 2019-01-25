//
// Created by Nikitos on 19/01/2019.
//

#ifndef Function_Function_H
#define Function_Function_H

#include <memory>
#include <exception>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>

using std::unique_ptr;
using std::forward;
using std::move;

const static unsigned int BUFFER_SIZE = 64;

class bad_function_call : std::exception {
public:
    bad_function_call() : exception() {}
    virtual const char* what() const noexcept {
        return "Error: bad function call";
    }
    virtual ~bad_function_call() {}
};

template <typename noType>
class Function;

template <typename returnType, typename ... argsTypes>
class Function <returnType(argsTypes ...)> {
public:
    Function() noexcept : type(-1), caller(nullptr) {}

    Function(std::nullptr_t) noexcept : type(-1), caller(nullptr) {}

    template <typename funcType>
    Function(funcType f) {
        if (sizeof(funcType) <= BUFFER_SIZE) {
            type = 0;
            new (buffer) free_holder<funcType>(std::move(f));
        } else {
            type = 1;
            caller = std::make_unique<free_holder<funcType>>(std::move(f));
        }
    }

    Function(returnType (*func)(argsTypes ... args)) : type(2), pFunc(func) {}

    returnType operator()(argsTypes ... args) const {
        switch (type) {
            case 0:
                return (reinterpret_cast<holder_base *>(const_cast<char *>(buffer)))->call(std::forward<argsTypes>(args) ...);
            case 1:
                return caller->call(std::forward<argsTypes>(args) ...);
            case 2:
                return pFunc(std::forward<argsTypes>(args) ...);
            default:
                throw bad_function_call();
        }
    }

    Function(const Function & other) : type(other.type) {
        switch (type) {
            case 0:
                (reinterpret_cast<holder_base *>(const_cast<char *>(other.buffer)))->create(buffer);
                break;
            case 1:
                caller = other.caller->clone();
                break;
            case 2:
                pFunc = other.pFunc;
                break;
            default:
                break;
        }
    }

    Function(Function && other) noexcept : type(other.type), caller(nullptr) {
        other.type = -1;

        switch (type) {
            case 0:
                std::swap(buffer, other.buffer);
                break;
            case 1:
                caller = std::move(other.caller);
                break;
            case 2:
                pFunc = other.pFunc;
                break;
            default:
                break;
        }
    }

    Function & operator =(const Function & other) {
        Function<returnType(argsTypes...)>(other).swap(*this);
        return *this;
    }

    Function & operator =(Function&& other) noexcept {
        Function<returnType(argsTypes...)>(std::forward<Function<returnType(argsTypes...)>>(other)).swap(*this);
        return *this;
    }

    void swap(Function & other) noexcept {
        using std::swap;

        swap(type, other.type);
        swap(buffer, other.buffer);
    }

    explicit operator bool() const noexcept {
        return type != -1;
    }

    ~Function() {
        if (type == 0) {
            auto p = reinterpret_cast<holder_base *>(const_cast<char *>(buffer));
            p->~holder_base();
        } else if (type == 1) {
            caller.reset();
        } else if (type == 2) {
            pFunc = nullptr;
        }
    }

private:
    class holder_base {
    public:
        holder_base() = default;
        virtual ~holder_base() = default;
        virtual returnType call(argsTypes ... args) = 0;
        virtual unique_ptr<holder_base> clone() = 0;
        virtual void create(void * buffer) = 0;

        holder_base(const holder_base &) = delete;
        void operator = (const holder_base &) = delete;
    };

    template <typename funcType>
    class free_holder : public holder_base {
    public:
        explicit free_holder(funcType & inFunc) : holder_base(), myFunc((inFunc)) {}

        returnType call(argsTypes ... args) override {
            return myFunc(std::forward<argsTypes>(args) ...);
        }

        unique_ptr<holder_base> clone() override {
            return unique_ptr<holder_base>(new free_holder<funcType>(myFunc));
        }

        void create(void * buffer) override {
            new (buffer) free_holder<funcType>(myFunc);
        }

        ~free_holder() = default;
    private:
        funcType myFunc;
    };

private:
    short type;

    union {
        unique_ptr<holder_base> caller;
        returnType (*pFunc)(argsTypes ...);
        char buffer[BUFFER_SIZE];
    };
};

#endif //Function_Function_H
