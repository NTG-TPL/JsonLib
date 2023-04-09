#ifndef TESTS_H
#define TESTS_H
#include <cassert>
#include <chrono>
#include <sstream>
#include <string_view>
#include "json_builder.h"

namespace tests {

    using namespace std::literals;

    json::Document LoadJSON(const std::string& s);

    std::string Print(const json::Node& node);

    void MustFailToLoad(const std::string& s);

    template <typename Fn>
    void MustThrowLogicError(Fn fn) {
        try {
            fn();
            std::cerr << "logic_error is expected"sv << std::endl;
            assert(false);
        } catch (const std::logic_error&) {
            // ok
        } catch (const std::exception& e) {
            std::cerr << "exception thrown: "sv << e.what() << std::endl;
            assert(false);
        } catch (...) {
            std::cerr << "Unexpected error"sv << std::endl;
            assert(false);
        }
    }

    void TestNull();

    void TestNumbers();

    void TestStrings();

    void TestBool();

    void TestArray();

    void TestDict();

    void TestErrorHandling();

    void Benchmark();

    void TestJsonBuilder();

    void TestJsonBuilderThrows();

    void Test();

}  // namespace
#endif // TESTS_H
