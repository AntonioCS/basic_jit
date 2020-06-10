
#include <iostream>
#include "custom_types.h"
#include "MakeConstant32.h"
#include "MakeIncrements64.h"
#include "MakeIdentity64.h"

//https://godbolt.org/z/otLCjv
//48 c7 c0 2a 00 00 00      mov    rax, 0x2a  //0x2a = 42
//c3                        ret
//little indian - order of bytes least significant to most significant
//
//Sites:
// - https://docs.microsoft.com/en-us/cpp/build/stack-usage?view=vs-2019
// - https://defuse.ca/online-x86-assembler.htm
// - https://docs.microsoft.com/en-us/cpp/cpp/calling-conventions?view=vs-2019
// - https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=vs-2019
// - https://docs.microsoft.com/en-us/cpp/assembler/masm/dword?view=vs-2019
// - https://docs.microsoft.com/en-us/cpp/assembler/masm/word?view=vs-2019
//

namespace
{
    template<typename T>
    void test_helper(T expected_result, T result, const char* msg)
    {
        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }
}

int main() {
    {
        const char* msg = "should create function that will return 42\n";
        constexpr auto expected_result = 42;
        MakeConstant32 makeConstant32;
        constant_s32 the_answer = makeConstant32.makeFunc(expected_result);// make_constant_32(expected_result);
        s32 result = the_answer();


        test_helper(expected_result, result, msg);
        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "Incorrect result!") << '\n';
        std::cout << ((the_answer() == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    { 
        const char* msg = "should create function that will return 21\n";
        constexpr auto expected_result = 21;
        MakeConstant32 makeConstant32;
        constant_s32 the_answer = makeConstant32.makeFunc(expected_result);// make_constant_32(expected_result);
        s32 result = the_answer();

        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        const char* msg = "should create a function that returns s64 value that was passed\n";
        constexpr auto expected_result = 42;
        MakeIdentity64 makeIdentity64;
        auto id_64 = makeIdentity64.makeFunc();

        auto result = id_64(expected_result);

        std::cout << msg;
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        const char* msg = "should create a function that increases the s64 value that was passed\n";
        constexpr auto expected_result = 42;

        MakeIncrements64 makeIncrement_s64{512};
        auto func = makeIncrement_s64.makeFunc();

        auto result = func(expected_result - 1);

        std::cout << msg;
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }
}