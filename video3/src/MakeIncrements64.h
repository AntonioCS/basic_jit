#pragma once

#include "FuncGeneratorBase.h"

class MakeIncrements64 : public FuncGeneratorBase<increment_s64>
{
    using FuncGeneratorBase::FuncGeneratorBase;
public:

    //This is what the author did
    //https://godbolt.org/z/WSHbMp
    void memory_manipulationAsInVideo()
    {
        //https://godbolt.org/z/kt_Z5g
        append<u8>( //mov    DWORD PTR [rsp + 8],ecx 
            0x89, 0x4C, 0x24, 0x08
        );

        reserveStackSpace(24);              //sub     rsp, 24
        moveToStackOffsetImm<s32>(0, 1);   //mov     DWORD PTR diff$[rsp], 1 (changed to: "mov DWORD PTR [rsp + 0], 1" as diff$ = 0)
        append<u8>(
            0x8b, 0x04, 0x24,                   //mov     eax, DWORD PTR[rsp]
            0x8b, 0x4c, 0x24, 0x20              //mov     ecx, DWORD PTR num$[rsp]  --- num$ = 32
        );

        //addToEAXvalueAtStackOffset()
        //addToECXvalueAtStackOffset(32); //

        append<u8>( // to make 64 bit we just add asm_x64_rex_w (0x48)
            0x01, 0xc1, //add ecx, eax 
            0x89, 0xc8  //mov eax,ecx
        );

        freeStackSpace();                       //add     rsp, 24
        asm_ret();
    }

    increment_s64 makeFuncAsInVideo()
    {
        memory_manipulationAsInVideo();
        return reinterpret_cast<increment_s64>(m_memory);
    }

    void memory_manipulation() override
    {
        //https://godbolt.org/z/jQxaKI
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
