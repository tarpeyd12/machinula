#ifndef _LTHREAD_H_INCLUDED
#define _LTHREAD_H_INCLUDED

#include <functional>

#include "./TinyThread++-1.1/source/tinythread.h"

namespace lthread
{
    // this class is used to hold the functor that _fThread uses, so that the functor itself is
    //  set before the thread starts
    template < typename F >
    class _fThread_functorHolder
    {
        protected:
            static inline void _functor_proxy_func( void * _func )
            {
                static_cast<_fThread_functorHolder<F>*>(_func)->_functor();
            }

            F _functor;
        public:
            _fThread_functorHolder()
            {  }

            _fThread_functorHolder( F _func )
            : _functor( _func )
            {  }
    };

    // this class is designed explicitly to add C++11 lambda functionality to TinyThread++
    template < typename F >
    class _fThread : public _fThread_functorHolder<F>, public tthread::thread
    {
        public:
            _fThread( F _func )
            : _fThread_functorHolder<F>(_func), tthread::thread( _fThread<F>::_functor_proxy_func, static_cast<void*>(this) )
            {  }

            _fThread( void (_func)(void*), void * _data )
            : _fThread_functorHolder<F>(), tthread::thread( _func, _data )
            {  }
    };

    typedef _fThread< std::function<void()> > thread;

    using namespace tthread; // put everything in tthread into lthread

    using lthread::thread; // probably not needed, but just in case. (also might break things)
}

#endif // _LTHREAD_H_INCLUDED
