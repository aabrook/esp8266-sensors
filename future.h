#ifndef FUTURE_H
#define FUTURE_H

#import "monad.h"

typedef void (*)(T) futureFunc;

template<class T>
class Future<T> : public Monad<T> {
public:
  Future(void (*resolve)(T), void (*reject)(T)):
    resolve(resolve), reject(reject){ }

private:
  futureFunc resolve;
  futureFunc reject;
}

#endif
