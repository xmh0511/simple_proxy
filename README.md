# simple_proxy
微软[Proxy](https://github.com/microsoft/proxy)核心实现

````cpp
struct Implementor{
    int Draw(double p){
        std::cout<< p<<std::endl;
        return 10;
    }
    std::string Area(std::string d){
        std::cout<<d<<std::endl;
        return "just return!";
    }
    void Action(){
        std::cout<<"void Action()\n";
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
struct C:MetaInfo<void>{
    template<class T>
    void operator()(T& self){
        return self.Action();
    }
};
int main(){
    auto proxy = Proxy<A,B,C>{Implementor{}};
    auto r = proxy.invoke<A>(1);
    std::cout<< r<<std::endl;
    auto r1 = proxy.invoke<B>("abc");
    std::cout<< r1<<std::endl;
    proxy.invoke<C>();
}
````
