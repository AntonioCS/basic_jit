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

    constexpr auto asm_x64_rex_w = 0x48; //indicates instruction is x64
    constexpr auto asm_rax = 0xC0;
    constexpr auto asm_mov = 0xC7;
    constexpr auto asm_res = 0xc3;

    using s64 = int64_t;
    using s32 = int32_t;
    using u8 = uint8_t;
    using constant_s32 = s32(*)();


    // Little Indian
    // s32 - 4 bytes
    // 42 = 0x2a 0x00 0x00 0x00 
    constant_s32 make_contant_32(s32 value)
    {
        (void)value;
        constexpr std::size_t buffsize{ 1024 };

        LPVOID memory = VirtualAlloc(
            nullptr, buffsize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
        );

        //c++ will not allow assignment to a void pointer (LPVOID)
        u8* memory_c = static_cast<u8*>(memory);

        memory_c[0] = asm_x64_rex_w;
        memory_c[1] = asm_mov;
        memory_c[2] = asm_rax;

        //auto* immediate_value = static_cast<s32*>(&memory_c[3]);
        auto* immediate_value = (s32*)(&memory_c[3]);
        *immediate_value = value;

        memory_c[7] = asm_res;

        return (constant_s32)(memory);
    }

}

int main() {
    {
        constexpr auto expected_result = 42;
        constant_s32 the_answer = make_contant_32(expected_result);
        s32 result = the_answer();

        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "Incorrect result!") << '\n';
    }

    {
        constexpr auto expected_result = 21;
        constant_s32 the_answer = make_contant_32(expected_result);
        s32 result = the_answer();

        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "Incorrect result!") << '\n';
    }

}