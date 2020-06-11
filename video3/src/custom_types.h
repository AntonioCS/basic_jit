#pragma once
#include <cstdint>
#include <cstddef> //std::size_t


using s64 = int64_t;
using s32 = int32_t;
using u8 = uint8_t;
using s8 = int8_t;
using constant_s32 = s32(*)();
using identify_s64 = s64(*)(s64);
using increment_s64 = s64(*)(s64);

constexpr std::size_t buffsize{ 1024 };
