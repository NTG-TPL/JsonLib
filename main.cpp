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
                .Key("key1"s).Value(123)
                .Key("key2"s).Value("value2"s)
                .Key("key3"s).StartArray()
                    .Value(456)
                    .StartDict().EndDict()
                    .StartDict()
                        .Key(""s).Value(nullptr)
                    .EndDict()
                    .Value(""s)
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

   std::cout << __FUNCTION__ << " is finished" << std::endl;
}
