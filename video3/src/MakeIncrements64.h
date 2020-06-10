#pragma once


#include "FuncGeneratorBase.h"

class MakeIncrements64 : public FuncGeneratorBase<increment_s64>
{
    using FuncGeneratorBase::FuncGeneratorBase;
public:

    void memory_manipulation2()
    {
        //https://godbolt.org/z/kt_Z5g

        append<u8>(
            asm_x64_rex_w, // sub     rsp, 24
            0x83,
            0xEC,
            0x18
            );

    }

    increment_s64 makeFunc2()
    {
        memory_manipulation2();
        return reinterpret_cast<increment_s64>(m_memory);
    }

    void memory_manipulation() override
    {
        append<u8>(
            //mov rax, rcx
            asm_x64_rex_w,
            0x89,
            0xC8,

            //in the original video the author did value + 1 (I just did ++value)
            // inc rax
            asm_x64_rex_w, // inc rax
            0xFF,
            0xC0,
            asm_x64_ret
        );
    }
};
