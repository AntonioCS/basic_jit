#include <windows.h>
#include <cstdint> //int64_t, uint8_t
//#include <cstddef> //size_t
#include <iostream>
//https://defuse.ca/online-x86-assembler.htm
//https://godbolt.org/z/otLCjv
//48 c7 c0 2a 00 00 00      mov    rax, 0x2a  //0x2a = 42
//c3                        ret
//little indian - order of bytes least significant to most significant
namespace {

    //  mov    rax, rcx
    //  0x48, 0x89, 0xC8

    constexpr auto asm_x64_rex_w = 0x48; //indicates instruction is x64
    constexpr auto asm_ret = 0xc3; //ret

    using s64 = int64_t;
    using s32 = int32_t;
    using u8 = uint8_t;
    using constant_s32 = s32(*)();
    using identify_s64 = s64(*)(s64);

    constexpr std::size_t buffsize{ 1024 };

    // Little Indian
    // s32 - 4 bytes
    // 42 = 0x2a 0x00 0x00 0x00

    u8* memory_func_allocation(std::size_t size = buffsize)
    {
        LPVOID memory = VirtualAlloc(
            nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
        );

        SecureZeroMemory(memory, size); //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/aa366877(v%3dvs.85)

        //c++ will not allow assignment to a void pointer (LPVOID)
        return static_cast<u8*>(memory);
    }

    constant_s32 make_constant_32(s32 value)
    {
        u8* memory = memory_func_allocation();

        //mov    rax, <immediate_value>
        //{ 0x48, 0xC7, 0xC0, <immediate_value> }
        memory[0] = asm_x64_rex_w;
        memory[1] = 0xC7;
        memory[2] = 0xC0;

        auto* immediate_value = (s32*)(&memory[3]);
        *immediate_value = value; //fills the the next 4 bytes with the value

        memory[7] = asm_ret;

        return (constant_s32)(memory);
    }

    identify_s64 make_identity_64()
    {
        u8* memory = memory_func_allocation();

        //int64_t func(int64_t value) {
        //    return value; //< - mov    rax, rcx
        //}
        //https://godbolt.org/z/xG-nYW
        //mov    rax, rcx  = 0x48, 0x89, 0xC8
        memory[0] = asm_x64_rex_w;
        memory[1] = 0x89;
        memory[2] = 0xC8;
        memory[3] = asm_ret;

        return (identify_s64)(memory);
    }
}

//https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=vs-2019
int main() {
    {
        const char* msg = "should create function that will return 42\n";
        constexpr auto expected_result = 42;
        constant_s32 the_answer = make_constant_32(expected_result);
        s32 result = the_answer();

        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "Incorrect result!") << '\n';
        std::cout << ((the_answer() == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        const char* msg = "should create function that will return 21\n";
        constexpr auto expected_result = 21;
        constant_s32 the_answer = make_constant_32(expected_result);
        s32 result = the_answer();

        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        const char* msg = "should create a function that returns s64 values that was passed\n";
        constexpr auto expected_result = 42;
        auto id_64 = make_identity_64();

        auto result = id_64(expected_result);

        std::cout << msg;
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

}