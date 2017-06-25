#ifndef EITHER_H
#define EITHER_H
#include "monad.h"

template <class T>
class Either : public Monad<T> {
public:
    virtual Either* bind(Either* (*bind_f)(T)) { return 0; };
    virtual Either* fmap(T (*fmap_f)(const T)) { return 0; };
    virtual void fork(void (*errored)(T*), void (*success)(T*)) {}
    virtual T getData() {}
};

template <class T>
class Right : public Either<T> {
public:
    Right(T pure):
    data(pure) { }

    Either<T>* bind(Either<T>* (*bind_f)(T)){
        return bind_f(data);
    }

    Right* fmap(T (*fmap_f)(const T)){
        data = fmap_f(data);
        return this;
    }

    void fork(void (*errored)(T*), void (*success)(T*)){
        success(&data);
    }

    T getData() { return data; }

private:
    T data;
};

template <class T>
class Left : public Either<T> {
public:
    Left(T left) : data(left) {}

    Left* bind(Either<T>* (*bind_f)(T)) {
        return this;
    }
    Left* fmap(T* (*fmap_f)(const T)){
        return this;
    }

    void fork(void (*errored)(T*), void (*success)(T*)){
        errored(&data);
    }

    T getData() { return data; }

private:
    T data;
};

#endif
