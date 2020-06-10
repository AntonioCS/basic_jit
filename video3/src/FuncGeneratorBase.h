#pragma once

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

    void clearMemory()
    {
        SecureZeroMemory(m_memory_base, m_size); //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/aa366877(v%3dvs.85)
    }

    virtual ~FuncGeneratorBase()
    {
        if (m_memory_base) {
            VirtualFree(m_memory_base, 0, MEM_RELEASE);
        }
    }
    template <typename  vtype>
    void append(vtype value)
    {
        m_memory[m_position] = value;
        m_position += sizeof value;
    }

    template <typename  vtype, typename ...Targs>
    void append(Targs&&... targs) {
        (append(static_cast<vtype>(std::forward<Targs>(targs))), ...);
    }

    void append_ret()
    {
        append<u8>(asm_x64_ret);
    }

    virtual void memory_manipulation() = 0;

    virtual funcType makeFunc()
    {
        memory_manipulation();
        return reinterpret_cast<funcType>(m_memory);
    }
};

