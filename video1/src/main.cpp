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
    constexpr auto asm_ret = 0xc3;

    using i64 = int64_t;
    using u8 = uint8_t;
    using the_answer = i64(*)();
}

int main() {
    constexpr std::size_t buffsize{ 1024 };
    constexpr auto expected_result = 42;

    LPVOID memory = VirtualAlloc(
        nullptr, buffsize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
    );

    //c++ will not allow assignment to a void pointer (LPVOID)
    u8* memory_c = static_cast<u8*>(memory);

    //mov    rax, 0x2a
    //{ 0x48, 0xC7, 0xC0, 0x2A, 0x00, 0x00, 0x00 }
    memory_c[0] = asm_x64_rex_w;
    memory_c[1] = 0xC7;
    memory_c[2] = 0xC0;
    memory_c[3] = 0x2A; //42
    memory_c[4] = 0x00;
    memory_c[5] = 0x00;
    memory_c[6] = 0x00;
    memory_c[7] = asm_ret;


    i64 result = ((the_answer)memory)();

    std::cout << "Result: " << result << '\n';

    if (result == expected_result) {
        std::cout << "Correct result!!\n";
    } else {
        std::cout << "Incorrect result!\n";
    }

}