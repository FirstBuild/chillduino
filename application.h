/**
 * Copyright (c) 2015 FirstBuild
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef APPLICATION_H
#define APPLICATION_H

class Delegate {
  public:
    typedef void (*Function)(void *);

  private:
    void *_instance;
    Function _function;

  private:
    Delegate(void *instance, Function function) :
      _instance(instance),
      _function(function) { }

  public:
    Delegate(void) :
      _instance(0),
      _function(0) { }

    void operator() (void) {
      _function(_instance);
    }

    bool null(void) const {
      return (_function == 0);
    }

    template <void (*function)(void *)>
    static Delegate create(void *instance) {
      return Delegate(instance, function);
    }

    template <typename T, void (*function)(T *)>
    static Delegate create(T *instance) {
      return Delegate((void *) instance, (Function) function);
    }

    template <typename T, void (T::*function)(void)>
    static Delegate create(T *instance) {
      return Delegate(instance, method<T, function>);
    }

  private:
    template <typename T, void (T::*function)(void)>
    static void method(void *instance) {
      T *t = (T *) instance;
      (t->*function)();
    }
};

class Timer {
  public:
    typedef unsigned long Ticks;

  private:
    Ticks _ticks;
    Delegate _delegate;

  public:
    Timer(void) :
      _ticks(0),
      _delegate() { }

    Timer(Ticks ticks, const Delegate& delegate) :
      _ticks(ticks),
      _delegate(delegate) { }

    bool null(void) const {
      return _delegate.null();
    }

    void operator()(Ticks ticks) {
      if (!_delegate.null()) {
        if (_ticks > ticks) {
          _ticks -= ticks;
        }
        else {
          Delegate d = _delegate;
          _delegate = Delegate();
          d();
        }
      }
    }
};

class Application {
  private:
    static const int TIMERS_IN_POOL = 12;
    static Timer _timerPool[TIMERS_IN_POOL];
    static Timer::Ticks _ticks;

  public:
    static void tick(Timer::Ticks ticks = 1) {
      _ticks += ticks;
    }

    static void loop(void) {
      Timer::Ticks ticks = _ticks;
      _ticks = 0;

      for (int i = 0; i < TIMERS_IN_POOL; i++) {
        _timerPool[i](ticks);
      }
    }

    static int setTimeout(Timer::Ticks ticks, const Delegate& delegate) {
      for (int i = 0; i < TIMERS_IN_POOL; i++) {
        if (_timerPool[i].null()) {
          _timerPool[i] = Timer(ticks, delegate);
          return i;
        }
      }

      return -1;
    }

    template <void (*function)(void *)>
    static int setTimeout(Timer::Ticks ticks, void *instance) {
      Delegate delegate = Delegate::create<function>(instance);
      return setTimeout(ticks, delegate);
    }

    template <typename T, void (*function)(T *)>
    static int setTimeout(Timer::Ticks ticks, T *instance) {
      Delegate delegate = Delegate::create<T, function>(instance);
      return setTimeout(ticks, delegate);
    }

    template <typename T, void (T::*function)(void)>
    static int setTimeout(Timer::Ticks ticks, T *instance) {
      Delegate delegate = Delegate::create<T, function>(instance);
      return setTimeout(ticks, delegate);
    }
};

Timer Application::_timerPool[TIMERS_IN_POOL];
Timer::Ticks Application::_ticks = 0;

#endif /* APPLICATION_H */
