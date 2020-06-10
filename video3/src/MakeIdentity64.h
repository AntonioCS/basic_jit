#pragma once

#include "FuncGeneratorBase.h"

class MakeIdentity64 : public FuncGeneratorBase<identify_s64>
{
    void memory_manipulation() override
    {
        //https://docs.microsoft.com/en-us/cpp/build/x64-calling-convention?view=vs-2019#example-of-argument-passing-1---all-integers
        //First int parameter is stored in rcx
        //int64_t func(int64_t value) {
        //    return value; //< - mov    rax, rcx - The asm generated is a bit more complicated than this
        //}
        //https://godbolt.org/z/xG-nYW
        //mov    rax, rcx  = 0x48, 0x89, 0xC8        
        append<u8>(
            asm_x64_rex_w,
            0x89,
            0xC8,
            asm_x64_ret
            );
    }
};
