#define NDEBUG

#include "include/tests.h"
using namespace json;
using namespace std::literals;

void Example(){
    json::Print(
        json::Document{
            json::Builder{}
            .Value("just a string"s)
            .Build()
        },
        std::cout
    );
    std::cout << '\n';

    json::Print(
        json::Document{
            json::Builder{}
            .StartDict()
                .Key("ключ 1"s).Value(123)
                .Key("ключ 2"s).Value("значение 2"s)
                .Key("ключ 3"s).StartArray()
                    .Value(456)
                    .Value("значение 2")
                    .Value(nullptr)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key("ключ 1"s).Value("значение 1")
                        .Key("ключ 2"s).Value(nullptr)
                        .Key(""s).Value(nullptr)
                    .EndDict()
                    .Value("Надеюсь, принцип понятен"s)
                .EndArray()
            .EndDict()
            .Build()
        },
        std::cout
    );
    std::cout << '\n';
}

int main(){

#ifndef NDEBUG
    tests::Test();
#endif
   Example();
}
