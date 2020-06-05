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
    constexpr auto asm_x64_ret = 0xc3; //ret

    using s64 = int64_t;
    using s32 = int32_t;
    using u8 = uint8_t;
    using s8 = int8_t;
    using constant_s32 = s32(*)();
    using identify_s64 = s64(*)(s64);
    using increment_s64 = s64(*)(s64);

    constexpr std::size_t buffsize{ 1024 };

    template <typename funcType>
    class FuncGeneratorBase
    {
    protected:
        LPVOID m_memory_base = nullptr;
        u8* m_memory = nullptr;
        std::size_t m_position{};
        std::size_t m_size{};
    public:

        FuncGeneratorBase(std::size_t size = buffsize) : m_size(size)
        {
            m_memory_base = VirtualAlloc(
                nullptr, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE
            );

            if (m_memory_base == nullptr) {
                throw std::runtime_error{ "Unable to allocate memory" };
            }

            SecureZeroMemory(m_memory_base, size); //https://docs.microsoft.com/en-us/previous-versions/windows/desktop/legacy/aa366877(v%3dvs.85)

            m_memory = static_cast<u8*>(m_memory_base);
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

            append_ret();
        }

        constant_s32 makeFunc(s32 value)
        {
            memory_manipulation(value);
            return reinterpret_cast<constant_s32>(m_memory);
        }
    };

    class MakeIncrements64 : public FuncGeneratorBase<increment_s64>
    {
        using FuncGeneratorBase::FuncGeneratorBase;
    public:

        void memory_manipulation() override
        {
            append<u8>(
                //mov rax, rcx
                asm_x64_rex_w,
                0x89,
                0xC8,

                //in the original video the author did value + 1 (I just did ++value)
                // inc rax
                asm_x64_rex_w,
                0xFF,
                0xC0,
                asm_x64_ret
            );

            //m_memory[0] = asm_x64_rex_w;   //mov    rax, rcx
            //m_memory[1] = 0x89;
            //m_memory[2] = 0xC8;

            //in the original video the author did value + 1 (I just did ++value)
            //m_memory[3] = asm_x64_rex_w; // inc rax
            //m_memory[4] = 0xFF;
            //m_memory[5] = 0xC0;

            //m_memory[6] = asm_x64_ret;
        }
    };


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

        memory[7] = asm_x64_ret;

        return reinterpret_cast<constant_s32>(memory);
    }

    identify_s64 make_identity_64()
    {
        u8* memory = memory_func_allocation();

        //int64_t func(int64_t value) {
        //    return value; //< - mov    rax, rcx - The asm generated is a bit more complicated than this
        //}
        //https://godbolt.org/z/xG-nYW
        //mov    rax, rcx  = 0x48, 0x89, 0xC8
        memory[0] = asm_x64_rex_w;
        memory[1] = 0x89;
        memory[2] = 0xC8;
        memory[3] = asm_x64_ret;

        return (identify_s64)(memory);
    }

    /*
    increment_s64 make_increment_s64()
    {
        u8* memory = memory_func_allocation();
        memory[0] = asm_x64_rex_w; //mov    rax, rcx 
        memory[1] = 0x89;
        memory[2] = 0xC8;

        memory[3] = asm_x64_rex_w; // inc rax
        memory[4] = 0xFF;
        memory[5] = 0xC0;

        memory[6] = asm_x64_ret;

        return (increment_s64)(memory);
    }*/
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
        MakeConstant32 makeConstant32;
        constant_s32 the_answer = makeConstant32.makeFunc(expected_result);// make_constant_32(expected_result);
        s32 result = the_answer();

        std::cout << msg;
        std::cout << "Result: " << result << '\n';
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        const char* msg = "should create a function that returns s64 value that was passed\n";
        constexpr auto expected_result = 42;
        auto id_64 = make_identity_64();

        auto result = id_64(expected_result);

        std::cout << msg;
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }

    {
        const char* msg = "should create a function that increases the s64 value that was passed\n";
        constexpr auto expected_result = 42;
        MakeIncrements64 makeIncrement_s64{512};


        auto func = makeIncrement_s64.makeFunc();
            //make_increment_s64();

        auto result = func(expected_result - 1);

        std::cout << msg;
        std::cout << ((result == expected_result) ? "Correct result!!" : "INCORRECT result!") << '\n';
    }
}