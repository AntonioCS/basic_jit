
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
// - https://eli.thegreenplace.net/2011/02/04/where-the-top-of-the-stack-is-on-x86/ <-- Stack explanation
// - https://stackoverflow.com/a/30194393/8715 <-- for makeIncrement_s64 (read comments), why without optimization things are moved into the stack
//

namespace
{
    template<typename T, typename Y>
    void test_helper(T expected_result, Y result, const char* msg)
    {
        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        //https://stackoverflow.com/a/2616912/8715
        std::cout << ((result == static_cast<Y>(expected_result)) ? "\033[0;42mCorrect result!!\033[0m" : "\033[0;41mINCORRECT result!\033[0m") << '\n';
    }
}

int main() {
    {
        constexpr auto expected_result = 42;
        MakeConstant32 makeConstant32;
        constant_s32 the_answer = makeConstant32.makeFunc(expected_result);// make_constant_32(expected_result);
        s32 result = the_answer();
        const char* msg = "should create function that will return 42\n";


        test_helper(expected_result, result, msg);
       /* std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "Incorrect result!") << '\n';
        std::cout << ((the_answer() == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';*/
    }

    { 
        constexpr auto expected_result = 21;
        MakeConstant32 makeConstant32;
        constant_s32 the_answer = makeConstant32.makeFunc(expected_result);// make_constant_32(expected_result);
        const char* msg = "should create function that will return 21\n";
        s32 result = the_answer();


        test_helper(expected_result, result, msg);
        /*std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';*/
    }

    {
        MakeIdentity64 makeIdentity64;
        auto id_64 = makeIdentity64.makeFunc();
        const char* msg = "should create a function that returns s64 value that was passed\n";
        constexpr auto expected_result = 42;
        auto result = id_64(expected_result);

        test_helper(expected_result, result, msg);
        /*std::cout << msg;
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';*/
    }

    {
        MakeIncrements64 makeIncrement_s64{512};
        auto func = makeIncrement_s64.makeFunc();
        const char* msg = "should create a function that increases the s64 value that was passed\n";
        constexpr auto expected_result = 42;
        auto result = func(expected_result - 1);

        test_helper(expected_result, result, msg);
        //std::cout << msg;
        //std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        MakeIncrements64 makeIncrement_s64{ 512 };
        auto func = makeIncrement_s64.makeFuncAsInVideo();
        const char* msg = "should create a function that increases the s64 value that was passed (original more or less)\n";
        constexpr auto expected_result = 42;
        auto result = func(expected_result - 1);

        test_helper(expected_result, result, msg);
        //std::cout << msg;
        //std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }
}