#pragma once

#include <vector>
#include <stdexcept>
#include <windows.h>
#include "custom_types.h"

constexpr auto asm_x64_rex_w = 0x48; //indicates instruction is x64
constexpr auto asm_x64_ret = 0xc3; //ret

template <typename funcType>
class FuncGeneratorBase
{
protected:
    LPVOID m_memory_base = nullptr;
    u8* m_memory = nullptr;
    std::size_t m_position{};
    std::size_t m_size{};
    std::vector<std::size_t> m_stack_allocations{};
public:
    FuncGeneratorBase(std::size_t size = buffsize, s32 permisson_flags = PAGE_EXECUTE_READWRITE) : m_size(size)
    {
        m_memory_base = VirtualAlloc(
            nullptr, size, MEM_COMMIT | MEM_RESERVE, permisson_flags
        );

        if (m_memory_base == nullptr) {
            throw std::runtime_error{ "Unable to allocate memory" };
        }

        clearMemory();
        m_memory = static_cast<u8*>(m_memory_base);
    }

    virtual ~FuncGeneratorBase()
    {
        if (m_memory_base) {
            VirtualFree(m_memory_base, 0, MEM_RELEASE);
        }
    }

    void clearMemory()
    {
        SecureZeroMemory(m_memory_base, m_size); //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/aa366877(v%3dvs.85)
    }

    template <typename vtype>
    void append(vtype value)
    {
        m_memory[m_position] = value;
        m_position += sizeof value;
    }

    template <typename  vtype, typename ...Targs>
    void append(Targs&&... targs) {
        (append(static_cast<vtype>(std::forward<Targs>(targs))), ...);
    }

    void asm_ret()
    {
        append<u8>(asm_x64_ret);
    }

    virtual void memory_manipulation() = 0;

    virtual funcType makeFunc()
    {
        memory_manipulation();
        return reinterpret_cast<funcType>(m_memory);
    }

    virtual void reserveStackSpace(std::size_t size)
    {
        append<u8>( // sub rsp, <size>
            asm_x64_rex_w,
            0x83,
            0xec,
            size
        );

        m_stack_allocations.push_back(size);
    }

    virtual void freeStackSpace()
    {
        if (m_stack_allocations.empty()) {
            throw std::runtime_error{ "Unable to get stack space value" };
        }

        auto value = m_stack_allocations.back();
        append<u8>( //add rsp, <value>
            asm_x64_rex_w,
            0x83,
            0xc4,
            value
        );

        m_stack_allocations.pop_back();
    }

    //https://www.tutorialspoint.com/assembly_programming/assembly_registers.htm
    //https://youtu.be/3Itz_B_eb78?list=PLvdK1vRmp8wMzH4w_8sQ30NKU3Bt4Cc-M&t=2626
    template<typename valueType>
    void moveToStackOffsetImm(u8 offset, valueType value)
    {
        append<u8>( // mov DWORD PTR [rsp + <offset>], <value>, original: mov     DWORD PTR diff$[rsp], 1 (diff$ is just the offset)
            0xc7,  //x86 mode apparently
            0x44,
            0x24,
            offset
        );

        append<valueType>(value);
    }

    void add_ECX_valueAtStackOffset(u8 offset)
    {
        //https://youtu.be/3Itz_B_eb78?list=PLvdK1vRmp8wMzH4w_8sQ30NKU3Bt4Cc-M&t=2424
        //add ecx, DWORD PTR[rsp + <offset>]
        append<u8>(
            0x03,
            0x4C,
            0x24,
            offset
        );
    }

    void add_EAX_valueAtStackOffset(u8 offset)
    {
        //add    eax,DWORD PTR [rsp + <offset>]
        append<u8>( 
            0x03, 
            0x44, 
            0x24, 
            offset
        );
    }

    void asm_mov_rax_rcx()
    {
        //mov rax, rcx
        append<u8>(
            asm_x64_rex_w,
            0x89,
            0xc8
        );
    }
    //void move_ECX_to_EAX
};

