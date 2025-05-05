#include <iostream>
#include <bitset>
#include <cstdint>
#include <cmath>
#include <limits>
#include <iomanip>
#include <cstring>

float decode_half(uint16_t bits) {
    uint16_t sign     = (bits >> 15) & 0x1;
    uint16_t exponent = (bits >> 10) & 0x1F;
    uint16_t fraction = bits & 0x3FF;

    float sign_val = (sign == 0) ? 1.0f : -1.0f;

    if (exponent == 0x1F) {
        if (fraction == 0)
            return sign_val * INFINITY;
        else
            return NAN;  // NaN
    } else if (exponent == 0) {
        // Denormalized number
        float mantissa = fraction / std::pow(2.0f, 10);
        return sign_val * std::pow(2, -14) * mantissa;
    } else {
        // Normalized number
        float mantissa = 1 + (fraction / std::pow(2.0f, 10));
        return sign_val * std::pow(2, exponent - 15) * mantissa;
    }
}

uint16_t encode_half(float value) {
    uint32_t bits;
    std::memcpy(&bits, &value, sizeof(bits));
    uint32_t sign     = (bits >> 31) & 0x1;
    int32_t  exponent = ((bits >> 23) & 0xFF) - 127 + 15;
    uint32_t fraction = bits & 0x7FFFFF;
    uint16_t sign_bit = sign << 15;
    if (std::isnan(value)) {
        return sign_bit | 0x7E00;
    }
    if (std::isinf(value)) {
        return sign_bit | 0x7C00;
    }
    if (value == 0.0f) {
        return sign_bit;
    }
    if (exponent >= 31) {
        return sign_bit | 0x7C00;
    }
    if (exponent <= 0) {
        if (exponent < -10) {
            return sign_bit;
        }
        uint16_t mantissa = static_cast<uint16_t>((fraction | 0x800000) >> (1 - exponent + 13));
        return sign_bit | mantissa;
    }
    uint16_t half_exponent = static_cast<uint16_t>(exponent << 10);
    uint16_t half_fraction = static_cast<uint16_t>(fraction >> 13);
    return sign_bit | half_exponent | half_fraction;
}

float decode_float(uint32_t bits) {
    uint32_t sign     = (bits >> 31) & 0x1;
    uint32_t exponent = (bits >> 23) & 0xFF;
    uint32_t fraction = bits & 0x7FFFFF;

    float sign_val = (sign == 0) ? 1.0f : -1.0f;

    if (exponent == 255) {
        if (fraction == 0)
            return sign_val * INFINITY;
        else
            return NAN;
    } else if (exponent == 0) {
        // денормалізоване число
        float mantissa = fraction / std::pow(2.0f, 23);
        return sign_val * std::pow(2, -126) * mantissa;
    } else {
        // нормалізоване число
        float mantissa = 1 + (fraction / std::pow(2.0f, 23));
        return sign_val * std::pow(2, exponent - 127) * mantissa;
    }
}

uint32_t encode_float(float value) {
    uint32_t sign     = std::signbit(value) ? 1 : 0;
    uint32_t sign_bit = sign << 31;

    if (std::isnan(value)) {
        return sign_bit | 0x7FC00000;
    }

    if (std::isinf(value)) {
        return sign_bit | 0x7F800000;
    }

    if (value == 0.0f) {
        return sign_bit;
    }

    float abs_val = std::fabs(value);

    int exp;
    float mant = std::frexp(abs_val, &exp);

    uint32_t fraction = static_cast<uint32_t>((mant * 2 - 1) * (1 << 23));

    uint32_t exponent = static_cast<uint32_t>(exp + 126);

    uint32_t exponent_bits = exponent << 23;

    return sign_bit | exponent_bits | (fraction & 0x7FFFFF);
}

double decode_double(uint64_t bits) {
    uint64_t sign     = (bits >> 63) & 0x1;
    uint64_t exponent = (bits >> 52) & 0x7FF;
    uint64_t fraction = bits & 0xFFFFFFFFFFFFF;

    double sign_val = (sign == 0) ? 1.0 : -1.0;

    if (exponent == 0x7FF) {
        if (fraction == 0)
            return sign_val * std::numeric_limits<double>::infinity();
        else
            return std::numeric_limits<double>::quiet_NaN();
    } else if (exponent == 0) {
        // Denormalized number
        double mantissa = fraction / std::pow(2.0, 52);
        return sign_val * std::pow(2, -1022) * mantissa;
    } else {
        // Normalized number
        double mantissa = 1 + (fraction / std::pow(2.0, 52));
        return sign_val * std::pow(2, exponent - 1023) * mantissa;
    }
}

uint64_t encode_double(double value) {
    uint64_t sign = std::signbit(value) ? 1 : 0;
    uint64_t sign_bit = sign << 63;

    if (std::isnan(value)) {
        return sign_bit | 0x7FF0000000000000;
    }

    if (std::isinf(value)) {
        return sign_bit | 0x7FF0000000000000;
    }

    if (value == 0.0) {
        return sign_bit;
    }

    double abs_val = std::fabs(value);

    int exp;
    double mant = std::frexp(abs_val, &exp);

    uint64_t fraction = static_cast<uint64_t>((mant * 2 - 1) * (1ULL << 52));

    uint64_t exponent = static_cast<uint64_t>(exp + 1022);

    uint64_t exponent_bits = exponent << 52;

    return sign_bit | exponent_bits | (fraction & 0xFFFFFFFFFFFFF);
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
   }
      