//
// Created by Nikitos on 19/01/2019.
//

#ifndef J_MY_FUNCTION_H
#define J_MY_FUNCTION_H

#include <memory>
#include <exception>
#include <algorithm>
#include <cassert>
#include <cstring>

using std::unique_ptr;
const unsigned int BUFFER_SIZE = 64;

template <typename noType>
class my_function;

template <typename returnType, typename ... argsTypes>
class my_function <returnType(argsTypes ...)> {
public:
    my_function() noexcept : is_small(false), pFunc_(nullptr) {}

    my_function(std::nullptr_t) noexcept : is_small(false), pFunc_(nullptr) {}

    template <typename funcType>
    my_function(funcType f) : pFunc_(nullptr) {
        if (sizeof(funcType) <= BUFFER_SIZE) {
            is_small = true;
            myCaller_ = nullptr;
            new (buffer) free_holder<funcType>(f);
        } else {
            is_small = false;
            myCaller_ = std::make_unique<free_holder<funcType>>(f);
        }
    }

    returnType operator()(argsTypes ... args) const {
        if (is_small) {
            return ((holder_base*)(buffer))->call(std::forward<argsTypes>(args) ...);
        } else {
            if (pFunc_ != nullptr)
                return pFunc_(std::forward<argsTypes>(args) ...);
            else
                return myCaller_->call(std::forward<argsTypes>(args) ...);
        }
    }

    my_function(const my_function & other) {
        is_small = other.is_small;
        if (other.is_small) {
            memcpy(buffer, other.buffer, BUFFER_SIZE);
        } else {
            if (!other) {
                myCaller_ = nullptr;
                pFunc_ = nullptr;
            } else if (other.pFunc_ != nullptr) {
                myCaller_ = nullptr;
                pFunc_ = other.pFunc_;
            } else {
                myCaller_ = other.myCaller_->clone();
                pFunc_ = nullptr;
            }
        }
    }

    my_function(my_function && other) noexcept {
        is_small = other.is_small;
        if (other.is_small) {
            memcpy(buffer, other.buffer, BUFFER_SIZE);
        } else {
            if (!other) {
                myCaller_ = nullptr;
                pFunc_ = nullptr;
            } else if (other.pFunc_ != nullptr) {
                myCaller_ = nullptr;
                pFunc_ = other.pFunc_;
            } else {
                myCaller_ = std::move(other.myCaller_);
                pFunc_ = nullptr;
            }
        }
    }

    my_function & operator =(const my_function & other) {
        my_function<returnType(argsTypes...)>(other).swap(*this);
        return *this;
    }

    my_function & operator =(my_function&& other) noexcept {
        my_function<returnType(argsTypes...)>(std::forward<my_function<returnType(argsTypes...)>>(other)).swap(*this);
        return *this;
    }

    void swap(my_function & other) noexcept {
        std::swap(myCaller_, other.myCaller_);
        std::swap(pFunc_, other.pFunc_);
        std::swap(is_small, other.is_small);

        char tmpBuffer[BUFFER_SIZE];
        memcpy(tmpBuffer, buffer, BUFFER_SIZE);
        memcpy(buffer, other.buffer, BUFFER_SIZE);
        memcpy(other.buffer, tmpBuffer, BUFFER_SIZE);
    }

    explicit operator bool() const noexcept {
        return is_small || myCaller_ != nullptr || pFunc_ != nullptr;
    }

    ~my_function() = default;

private:
    class holder_base {
    public:
        holder_base() {}
        virtual ~holder_base() {}
        virtual returnType call(argsTypes ... args) = 0;
        virtual unique_ptr<holder_base> clone() = 0;
    private:
        holder_base(const holder_base &);
        void operator = (const holder_base &);
    };

    template<typename funcType>
    class free_holder : public holder_base {
    public:
        free_holder(funcType inFunc) : holder_base(), myFunc(inFunc) {}

        virtual returnType call(argsTypes ... args) override {
            return myFunc(std::forward<argsTypes>(args) ...);
        }

        virtual unique_ptr<holder_base> clone() override {
            return unique_ptr<holder_base>(new free_holder<funcType>(myFunc));
        }

        ~free_holder() = default;
    private:
        funcType myFunc;
    };

private:
    unique_ptr<holder_base> myCaller_;
    char buffer[BUFFER_SIZE];
    bool is_small;
    returnType (*pFunc_) (argsTypes ...);
};

#endif //J_MY_FUNCTION_H
