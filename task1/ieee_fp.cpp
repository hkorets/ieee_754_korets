#include <cstdint>
#include <cstring>
#include <limits>
#include <cmath>
#include <ostream>
#include <iostream>
#include <bitset>

float decode_half(uint16_t bits) {
    uint16_t sign     = (bits >> 15) & 0x1;
    uint16_t exponent = (bits >> 10) & 0x1F;
    uint16_t fraction = bits & 0x3FF;

    float sign_val = (sign == 0) ? 1.0f : -1.0f;

    if (exponent == 0x1F) {
        if (fraction == 0)
            return sign_val * INFINITY;
        else {
            uint32_t nan_bits = (sign << 31) | (0xFF << 23) | (fraction << 13);
            float result;
            std::memcpy(&result, &nan_bits, sizeof(result));
            return result;
        }
    }
    if (exponent == 0)
        return sign_val * std::ldexp(static_cast<float>(fraction), -24);

    return sign_val * std::ldexp(1.0f + static_cast<float>(fraction) / 1024.0f, exponent - 15);
}

uint16_t encode_half(float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));

    uint32_t sign     = (bits >> 31) & 0x1;
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;

    if (exponent == 0xFF && fraction != 0) {
        uint16_t half_fraction = static_cast<uint16_t>(fraction >> 13);
        return (sign << 15) | 0x7C00 | (half_fraction ? half_fraction : 0x200);
    }
    if (exponent == 0xFF)
        return (sign << 15) | 0x7C00;

    if (exponent <= 102)  // Subnormal
        return (sign << 15);

    if (exponent >= 143)  // Overflow, encode as Inf
        return (sign << 15) | 0x7C00;

    int16_t half_exp = static_cast<int16_t>(exponent - 127 + 15);
    uint16_t half_fraction = static_cast<uint16_t>(fraction >> 13);

    return (sign << 15) | (half_exp << 10) | half_fraction;
}

float decode_float(uint32_t bits) {
    uint32_t sign     = (bits >> 31) & 0x1;
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;

    if (exponent == 0xFF) {
        if (fraction == 0)
            return sign ? -INFINITY : INFINITY;
        else {
            float result;
            std::memcpy(&result, &bits, sizeof(result));
            return result;
        }
    }

    float sign_val = (sign == 0) ? 1.0f : -1.0f;
    if (exponent == 0)
        return sign_val * std::ldexp(static_cast<float>(fraction), -149);

    return sign_val * std::ldexp(1.0f + static_cast<float>(fraction) / (1 << 23), exponent - 127);
}

uint32_t encode_float(float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));

    if ((bits & 0x7F800000) == 0x7F800000 && (bits & 0x007FFFFF))
        return bits; // preserve original NaN

    return bits;
}

double decode_double(uint64_t bits) {
    uint64_t sign     = (bits >> 63) & 0x1;
    uint64_t exponent = (bits >> 52) & 0x7FF;
    uint64_t fraction = bits & 0xFFFFFFFFFFFFFULL;

    if (exponent == 0x7FF) {
        if (fraction == 0)
            return sign ? -INFINITY : INFINITY;
        else {
            double result;
            std::memcpy(&result, &bits, sizeof(result));
            return result;
        }
    }

    double sign_val = (sign == 0) ? 1.0 : -1.0;
    if (exponent == 0)
        return sign_val * std::ldexp(static_cast<double>(fraction), -1074);

    return sign_val * std::ldexp(1.0 + static_cast<double>(fraction) / (1ULL << 52), exponent - 1023);
}

uint64_t encode_double(double value) {
    uint64_t bits;
    std::memcpy(&bits, &value, sizeof(bits));

    if ((bits & 0x7FF0000000000000ULL) == 0x7FF0000000000000ULL &&
        (bits & 0x000FFFFFFFFFFFFFULL))
        return bits; // preserve original NaN

    return bits;
}

void print_binary(uint16_t bits) {
    std::cout << std::bitset<16>(bits) << std::endl;
}

void print_binary32(uint32_t bits) {
    std::cout << std::bitset<32>(bits) << '\n';
}

void print_binary64(uint64_t bits) {
    std::cout << std::bitset<64>(bits) << '\n';
}

int main() {
        //Half precision (encoding and decoding)
        std::cout << "Half precision (decoding and encoding)" << std::endl;
        std::cout << decode_half(0x3c00) << std::endl;
        print_binary(encode_half(decode_half(0x3c00)));
        std::cout << decode_half(0x03ff) << std::endl;
        print_binary(encode_half(decode_half(0x03ff)));
        std::cout << decode_half(0xc000) << std::endl;
        print_binary(encode_half(decode_half(0xc000)));
        std::cout << decode_half(0x7bff) << std::endl;
        print_binary(encode_half(decode_half(0x7bff)));
        std::cout << decode_half(0xfc00) << std::endl;
        print_binary(encode_half(decode_half(0xfc00)));
        std::cout << decode_half(0x7e00) << std::endl;
        print_binary(encode_half(decode_half(0x7e00)));
        std::cout << decode_half(0x7d00) << std::endl;
        print_binary(encode_half(decode_half(0x7d00)));

        //Half precision (encoding and decoding)
        std::cout << "Single precision (decoding and encoding)" << std::endl;
        std::cout << decode_float(0x3f800000) << std::endl;
        print_binary32(encode_float(decode_float(0x3f800000)));
        std::cout << decode_float(0x40490fdb) << std::endl;
        print_binary32(encode_float(decode_float(0x40490fdb)));
        std::cout << decode_float(0xc0000000) << std::endl;
        print_binary32(encode_float(decode_float(0xc0000000)));
        std::cout << decode_float(0x7f7fffff) << std::endl;
        print_binary32(encode_float(decode_float(0x7f7fffff)));
        std::cout << decode_float(0x7f800000) << std::endl;
        print_binary32(encode_float(decode_float(0x7f800000)));
        std::cout << decode_float(0x7fc00000) << std::endl;
        print_binary32(encode_float(decode_float(0x7fc00000)));
        std::cout << decode_float(0x7fa00000) << std::endl;
        print_binary32(encode_float(decode_float(0x7fa00000)));

        std::cout << "Double precision (decoding and encoding)" << std::endl;
        std::cout << decode_double(0x3FF0000000000000) << std::endl;
        print_binary64(encode_double(decode_double(0x3FF0000000000000)));
        std::cout << decode_double(0x0000000000000001) << std::endl;
        print_binary64(encode_double(decode_double(0x0000000000000001)));
        std::cout << decode_double(0xC000000000000000) << std::endl;
        print_binary64(encode_double(decode_double(0xC000000000000000)));
        std::cout << decode_double(0x4037000000000000) << std::endl;
        print_binary64(encode_double(decode_double(0x4037000000000000)));
        std::cout << decode_double(0x7FF0000000000000) << std::endl;
        print_binary64(encode_double(decode_double(0x7FF0000000000000)));
        std::cout << decode_double(0x7FF8000000000000) << std::endl;
        print_binary64(encode_double(decode_double(0x7FF8000000000000)));
        std::cout << decode_double(0x7FF4000000000000) << std::endl;
        print_binary64(encode_double(decode_double(0x7FF4000000000000)));
   }
   
