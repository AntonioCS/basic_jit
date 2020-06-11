#pragma once

#include "custom_types.h"
#include "FuncGeneratorBase.h"

class MakeConstant32 : public FuncGeneratorBase<constant_s32>
{
    using FuncGeneratorBase::FuncGeneratorBase;
public:
    void memory_manipulation() override { }

    void memory_manipulation(s32 value)
    {
        //{ 0x48, 0xC7, 0xC0, <immediate_value> }
        append<u8>(
            asm_x64_rex_w,
            0xC7,
            0xC0
            );

        append(value);

        asm_ret();
    }

    constant_s32 makeFunc(s32 value)
    {
        memory_manipulation(value);
        return reinterpret_cast<constant_s32>(m_memory);
    }
};
