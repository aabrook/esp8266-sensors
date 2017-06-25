#ifndef MONAD_H
#define MONAD_H

template <class T>
class Monad {
  public:
    virtual Monad<T>* bind(Monad* (*bind_f)(T)) { return 0; };
    virtual Monad<T>* fmap(T (*fmap_f)(const T)) { return 0; };
};

#endif
