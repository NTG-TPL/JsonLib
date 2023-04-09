#include "include/tests.h"
#include "include/test_framework.h"
#include <sstream>
namespace tests {

    using namespace json;

    json::Document LoadJSON(const std::string& s) {
        std::istringstream strm(s);
        return json::Load(strm);
    }

    std::string Print(const Node& node) {
        std::ostringstream out;
        Print(Document{node}, out);
        return out.str();
    }

    void MustFailToLoad(const std::string& s) {
        try {
            LoadJSON(s);
            std::cerr << "ParsingError exception is expected on '"sv << s << "'"sv << std::endl;
            ASSERT(false);
        } catch (const json::ParsingError&) {
            // ok
        } catch (const std::exception& e) {
            std::cerr << "exception thrown: "sv << e.what() << std::endl;
            ASSERT(false);
        } catch (...) {
            std::cerr << "Unexpected error"sv << std::endl;
            ASSERT(false);
        }
    }

    void TestNull() {
        Node null_node;
        ASSERT(null_node.IsNull());
        ASSERT(!null_node.IsInt());
        ASSERT(!null_node.IsDouble());
        ASSERT(!null_node.IsPureDouble());
        ASSERT(!null_node.IsString());
        ASSERT(!null_node.IsArray());
        ASSERT(!null_node.IsDict());

        Node null_node1{nullptr};
        ASSERT(null_node1.IsNull());

        ASSERT(Print(null_node) == "null"s);
        ASSERT(null_node == null_node1);
        ASSERT(!(null_node != null_node1));

        const Node node = LoadJSON("null"s).GetRoot();
        ASSERT(node.IsNull());
        ASSERT(node == null_node);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        ASSERT(LoadJSON(" \t\r\n\n\r null \t\r\n\n\r "s).GetRoot() == null_node);
    }

    void TestNumbers() {
        const Node int_node{42};
        ASSERT(int_node.IsInt());
        ASSERT(int_node.AsInt() == 42);
        // целые числа являются подмножеством чисел с плавающей запятой
        ASSERT(int_node.IsDouble());
        // Когда узел хранит int, можно получить соответствующее ему double-значение
        ASSERT(int_node.AsDouble() == 42.0);
        ASSERT(!int_node.IsPureDouble());
        ASSERT(int_node == Node{42});
        // int и double - разные типы, поэтому не равны, даже когда хранят
        ASSERT(int_node != Node{42.0});

        const Node dbl_node{123.45};
        ASSERT(dbl_node.IsDouble());
        ASSERT(dbl_node.AsDouble() == 123.45);
        ASSERT(dbl_node.IsPureDouble());  // Значение содержит число с плавающей запятой
        ASSERT(!dbl_node.IsInt());

        ASSERT(Print(int_node) == "42"s);
        ASSERT(Print(dbl_node) == "123.45"s);
        ASSERT(Print(Node{-42}) == "-42"s);
        ASSERT(Print(Node{-3.5}) == "-3.5"s);

        ASSERT(LoadJSON("42"s).GetRoot() == int_node);
        ASSERT(LoadJSON("123.45"s).GetRoot() == dbl_node);
        ASSERT(LoadJSON("0.25"s).GetRoot().AsDouble() == 0.25);
        ASSERT(LoadJSON("3e5"s).GetRoot().AsDouble() == 3e5);
        ASSERT(LoadJSON("1.2e-5"s).GetRoot().AsDouble() == 1.2e-5);
        ASSERT(LoadJSON("1.2e+5"s).GetRoot().AsDouble() == 1.2e5);
        ASSERT(LoadJSON("-123456"s).GetRoot().AsInt() == -123456);
        ASSERT(LoadJSON("0").GetRoot() == Node{0});
        ASSERT(LoadJSON("0.0").GetRoot() == Node{0.0});
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        ASSERT(LoadJSON(" \t\r\n\n\r 0.0 \t\r\n\n\r ").GetRoot() == Node{0.0});
    }

    void TestStrings() {
        Node str_node{"Hello, \"everybody\""s};
        ASSERT(str_node.IsString());
        ASSERT(str_node.AsString() == "Hello, \"everybody\""s);

        ASSERT(!str_node.IsInt());
        ASSERT(!str_node.IsDouble());

        ASSERT(Print(str_node) == "\"Hello, \\\"everybody\\\"\""s);

        ASSERT(LoadJSON(Print(str_node)).GetRoot() == str_node);
        const std::string escape_chars
            = R"("\r\n\t\"\\")"s;  // При чтении строкового литерала последовательности \r,\n,\t,\\,\"
        // преобразовываться в соответствующие символы.
        // При выводе эти символы должны экранироваться, кроме \t.
        ASSERT(Print(LoadJSON(escape_chars).GetRoot()) == "\"\\r\\n\t\\\"\\\\\""s);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        ASSERT(LoadJSON("\t\r\n\n\r \"Hello\" \t\r\n\n\r ").GetRoot() == Node{"Hello"s});
    }

    void TestBool() {
        Node true_node{true};
        ASSERT(true_node.IsBool());
        ASSERT(true_node.AsBool());

        Node false_node{false};
        ASSERT(false_node.IsBool());
        ASSERT(!false_node.AsBool());

        ASSERT(Print(true_node) == "true"s);
        ASSERT(Print(false_node) == "false"s);

        ASSERT(LoadJSON("true"s).GetRoot() == true_node);
        ASSERT(LoadJSON("false"s).GetRoot() == false_node);
        ASSERT(LoadJSON(" \t\r\n\n\r true \r\n"s).GetRoot() == true_node);
        ASSERT(LoadJSON(" \t\r\n\n\r false \t\r\n\n\r "s).GetRoot() == false_node);
    }

    void TestArray() {
        Node arr_node{Array{1, 1.23, "Hello"s}};
        ASSERT(arr_node.IsArray());
        const Array& arr = arr_node.AsArray();
        ASSERT(arr.size() == 3);
        ASSERT(arr.at(0).AsInt() == 1);

        ASSERT(LoadJSON("[1,1.23,\"Hello\"]"s).GetRoot() == arr_node);
        ASSERT(LoadJSON(Print(arr_node)).GetRoot() == arr_node);
        ASSERT(LoadJSON(R"(  [ 1  ,  1.23,  "Hello"   ]   )"s).GetRoot() == arr_node);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        ASSERT(LoadJSON("[ 1 \r \n ,  \r\n\t 1.23, \n \n  \t\t  \"Hello\" \t \n  ] \n  "s).GetRoot()
               == arr_node);
    }

    void TestDict() {
        Node dict_node{Dict{{"key1"s, "value1"s}, {"key2"s, 42}}};
        ASSERT(dict_node.IsDict());
        const Dict& dict = dict_node.AsDict();
        ASSERT(dict.size() == 2);
        ASSERT(dict.at("key1"s).AsString() == "value1"s);
        ASSERT(dict.at("key2"s).AsInt() == 42);

        ASSERT(LoadJSON("{ \"key1\": \"value1\", \"key2\": 42 }"s).GetRoot() == dict_node);
        auto v = LoadJSON(Print(dict_node)).GetRoot();
        ASSERT(LoadJSON(Print(dict_node)).GetRoot() == dict_node);
        // Пробелы, табуляции и символы перевода строки между токенами JSON файла игнорируются
        ASSERT(
            LoadJSON(
                "\t\r\n\n\r { \t\r\n\n\r \"key1\" \t\r\n\n\r: \t\r\n\n\r \"value1\" \t\r\n\n\r , \t\r\n\n\r \"key2\" \t\r\n\n\r : \t\r\n\n\r 42 \t\r\n\n\r } \t\r\n\n\r"s)
                .GetRoot()
            == dict_node);
    }

    void TestErrorHandling() {
        MustFailToLoad("["s);
        MustFailToLoad("]"s);

        MustFailToLoad("{"s);
        MustFailToLoad("}"s);

        MustFailToLoad("\"hello"s);  // незакрытая кавычка

        MustFailToLoad("tru"s);
        MustFailToLoad("fals"s);
        MustFailToLoad("nul"s);

        Node dbl_node{3.5};
        MustThrowLogicError([&dbl_node] {
            dbl_node.AsInt();
        });
        MustThrowLogicError([&dbl_node] {
            dbl_node.AsString();
        });
        MustThrowLogicError([&dbl_node] {
            dbl_node.AsArray();
        });

        Node array_node{Array{}};
        MustThrowLogicError([&array_node] {
            array_node.AsDict();
        });
        MustThrowLogicError([&array_node] {
            array_node.AsDouble();
        });
        MustThrowLogicError([&array_node] {
            array_node.AsBool();
        });
    }

    void Benchmark() {
        const auto start = std::chrono::steady_clock::now();
        Array arr;
        arr.reserve(1'000);
        for (int i = 0; i < 1'000; ++i) {
            arr.emplace_back(Dict{
                {"int"s, 42},
                {"double"s, 42.1},
                {"null"s, nullptr},
                {"string"s, "hello"s},
                {"array"s, Array{1, 2, 3}},
                {"bool"s, true},
                {"map"s, Dict{{"key"s, "value"s}}},
            });
        }
        std::stringstream strm;
        json::Print(Document{arr}, strm);
        const auto doc = json::Load(strm);
        ASSERT(doc.GetRoot() == arr);
        const auto duration = std::chrono::steady_clock::now() - start;
        std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << "ms"sv
                  << std::endl;
    }

    void TestJsonBuilder(){
        {
            std::stringstream ss_test ;
            ss_test << "\"just a string\"";

            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}.
                    Value("just a string"s).
                            Build()
                }, ss_out
            );
            ASSERT_EQUAL(ss_out.str(), ss_test.str())
        }
        {
            std::stringstream ss_test ;
            ss_test <<  "[\n"
                        "    1,\n"
                        "    2,\n"
                        "    3,\n"
                        "    4\n"
                        "]";

            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}.
                            StartArray().
                            Value(1).
                            Value(2).
                            Value(3).
                            Value(4).
                            EndArray().
                            Build()
                }, ss_out
            );
            ASSERT_EQUAL(ss_out.str(), ss_test.str())
        }

        {
            std::stringstream ss_test ;
            ss_test <<  "{\n"
                        "    \"key1\": 123,\n"
                        "    \"key2\": \"value2\",\n"
                        "    \"key3\": \"643\"\n"
                        "}";

            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}.
                            StartDict()
                            .Key("key1"s).Value(123)
                            .Key("key2"s).Value("value2"s)
                            .Key("key3"s).Value("643"s)
                            .EndDict()
                    .Build()
                }, ss_out
            );
            ASSERT_EQUAL(ss_out.str(), ss_test.str())
        }

        {
            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}
                            .Value(1)
                    .Build()
                }, ss_out
            );
            ASSERT_EQUAL(ss_out.str(), "1");
        }

        {
            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}
                            .Value(true)
                    .Build()
                }, ss_out
            );
            ASSERT_EQUAL(ss_out.str(), "true");
        }

        {
            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}
                            .Value(1e10)
                    .Build()
                }, ss_out
            );
            ASSERT_EQUAL(ss_out.str(), "1e+10");
        }

        {
            std::stringstream ss_test ;
            ss_test <<  "{\n"
                        "    \"key1\": 123,\n"
                        "    \"key2\": \"value2\",\n"
                        "    \"key3\": [\n"
                        "        456,\n"
                        "        {\n"
                        "\n"
                        "        },\n"
                        "        {\n"
                        "            \"\": null\n"
                        "        },\n"
                        "        \"\"\n"
                        "    ]\n"
                        "}";

            std::stringstream ss_out;
            json::Print(
                json::Document{
                    json::Builder{}
                    .StartDict()
                        .Key( "key1"s ).Value( 123 )
                        .Key( "key2"s ).Value( "value2"s )
                        .Key( "key3"s ).StartArray()
                            .Value( 456 )
                            .StartDict().EndDict()
                            .StartDict()
                                .Key( ""s )
                                .Value( nullptr )
                            .EndDict()
                            .Value( ""s )
                        .EndArray()
                    .EndDict()
                    .Build()
                },
                ss_out
            );
            ASSERT_EQUAL(ss_out.str(), ss_test.str());
        }
    }

    void TestJsonBuilderThrows(){

        using std::cout;
        {
            ASSERT_THROWS(
                json::Document{
                    json::Builder{}
                            .Value(1)
                            .Value(2)
                            .Value(3)
                            .Value(4)
                            .Value(5)
                    .Build()
                }, std::logic_error)
        }

        {
            ASSERT_THROWS(
                json::Document{
                    json::Builder{}.Build()
                }, std::logic_error)
        }

        {
            ASSERT_THROWS(
                json::Document{
                    json::Builder{}.Build()
                }, std::logic_error)
        }

//        {
//            ASSERT_THROWS(
//                json::Document{
//                    json::Builder{}.
//                                  StartDict()
//                                  .Key("123")
//                                  .EndDict()
//                                  .Build()
//                }, std::logic_error)
//        }


        {
            ASSERT_THROWS(
                json::Document{
                    json::Builder{}.
                                  StartArray()
                                  .Value(123)
                                  .EndArray()
                                  .Value(245)
                                  .Build()
                }, std::logic_error)
        }

        {
            ASSERT_DOESNT_THROW(
                json::Document{
                    json::Builder{}.StartArray()
                                  .EndArray()
                                  .Build()
                })
        }

        {
            ASSERT_THROWS(
                json::Document{
                    json::Builder{}.Value(123)
                                  .StartArray()
                                  .EndArray()
                                  .Build()
                }, std::logic_error)
        }

        {
            ASSERT_DOESNT_THROW(
                json::Document{
                    json::Builder{}.StartDict()
                                  .EndDict()
                                  .Build()
                })
        }
    }

    void Test(){
        TestRunner runner;
        RUN_TEST(runner, TestNull);
        RUN_TEST(runner, TestNumbers);
        RUN_TEST(runner, TestStrings);
        RUN_TEST(runner, TestBool);
        RUN_TEST(runner, TestArray);
        RUN_TEST(runner, TestDict);
        RUN_TEST(runner, TestErrorHandling);
        RUN_TEST(runner, Benchmark);
        RUN_TEST(runner, TestJsonBuilder);
        RUN_TEST(runner, TestJsonBuilderThrows);
    }

}
