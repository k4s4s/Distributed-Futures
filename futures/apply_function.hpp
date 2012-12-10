
#ifndef _H_APPLY_FUNCTION_HPP
#define _H_APPLY_FUNCTION_HPP

#include <tuple>
#include <cstddef>
#include <type_traits>
#include <utility>

template<size_t N>
struct Apply {
    template<typename F, typename T, typename... A>
    static inline auto apply(F&& f, T && t, A &&... a)
        -> decltype(Apply<N-1>::apply(::std::forward<F>(f), ::std::forward<T>(t),
            ::std::get<N-1>(::std::forward<T>(t)), ::std::forward<A>(a)...
        ))
    {
        return Apply<N-1>::apply(::std::forward<F>(f), ::std::forward<T>(t),
            ::std::get<N-1>(::std::forward<T>(t)), ::std::forward<A>(a)...
        );
    }
};

template<>
struct Apply<0> {
    template<typename F, typename T, typename... A>
    static inline auto apply(F && f, T &&, A &&... a)
        -> decltype(f(::std::forward<A>(a)...))
    {
        return f(::std::forward<A>(a)...);
    }
};

template<typename F, typename T>
inline auto apply(F && f, T && t)
     -> decltype(Apply< ::std::tuple_size<
         typename ::std::decay<T>::type
     >::value>::apply(::std::forward<F>(f), ::std::forward<T>(t)))
{
    return Apply< ::std::tuple_size<
        typename ::std::decay<T>::type
    >::value>::apply(f, ::std::forward<T>(t));
};

/*** serialization of tuples ***/
namespace boost {
namespace serialization {

template<uint N>
struct Serialize
{
    template<class Archive, typename... Args>
    static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
    {
        ar & std::get<N-1>(t);
        Serialize<N-1>::serialize(ar, t, version);
    }
};

template<>
struct Serialize<0>
{
    template<class Archive, typename... Args>
    static void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
    {
    }
};

template<class Archive, typename... Args>
void serialize(Archive & ar, std::tuple<Args...> & t, const unsigned int version)
{
    Serialize<sizeof...(Args)>::serialize(ar, t, version);
}

}
}

#endif