#include <iostream>
#include <tuple>
#include <memory>
template<class Ret, class ...Args>
struct MetaInfo{
    using ret_type = Ret;
    using args_type = std::tuple<Args...>;
    using index_seq = decltype(std::make_index_sequence<sizeof...(Args)>{});
};
template<class Concret, class Impl, class Tuple, std::size_t ...N>
auto invoke_help(Impl&& impl, Tuple&& tp, std::index_sequence<N...>){
    return Concret{}(std::forward<Impl>(impl), std::get<N>(std::forward<Tuple>(tp))...);
}
template<class Concret, class Impl>
typename Concret::ret_type invoke(void* instance, void* args_tuple){
    auto impl = reinterpret_cast<Impl*>(instance);
    auto argus = reinterpret_cast<typename Concret::args_type*>(args_tuple);
    return invoke_help<Concret>(*impl, *argus, typename Concret::index_seq{});
}

template<class T>
struct Dispatcher{
    T::ret_type(*func)(void*,void*);
    template<class Impl>
    Dispatcher(Impl):func(invoke<T,Impl>){

    }
};
template<class ...Types>
struct Proxy{
    char arr[16];
    std::unique_ptr<std::tuple<Dispatcher<Types>...>> dispatches = nullptr;
    Proxy() = default;
    template<class Impl>
    Proxy(Impl t):dispatches(std::make_unique<std::tuple<Dispatcher<Types>...>>((Types{},t)...)){
        new(arr) Impl{t};
    }
    template<class T, class...Args>
    auto invoke(Args&&...args){
        auto args_tuple = typename T::args_type{std::forward<Args>(args)...};
        auto f = std::get<Dispatcher<T>>(*dispatches).func;
        return f(arr,&args_tuple);
    }
};
struct Implementor{
    int Draw(double p){
        std::cout<< p<<std::endl;
        return 10;
    }
    std::string Area(std::string d){
        std::cout<<d<<std::endl;
        return "just return!";
    }
};
struct A:MetaInfo<int, double>{
    template<class T>
    int operator()(T& self, double b){
        return self.Draw(b);
    }
};
struct B:MetaInfo<std::string,std::string>{
    template<class T>
    std::string operator()(T& self, std::string b){
        return self.Area(b);
    }
};
int main(){
    auto proxy = Proxy<A,B>{Implementor{}};
    auto r = proxy.invoke<A>(1);
    std::cout<< r<<std::endl;
    auto r1 = proxy.invoke<B>("abc");
    std::cout<< r1<<std::endl;
}
