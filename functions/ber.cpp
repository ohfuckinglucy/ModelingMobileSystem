#include "header.hpp"
#include <cmath>

#include <iostream>

float BER(const std::vector<uint8_t> &tx, const std::vector<uint8_t> &rx)
{
    if (tx.size() != rx.size())
    {
        std::cout << "[WARNING] Длины сообщений не соответствуют!\n";
        return 0.0f;
    }

    size_t err = 0;
    size_t total_bits = tx.size() * 8;

    for (size_t i = 0; i < tx.size(); ++i)
    {
        uint8_t diff = tx[i] ^ rx[i];

        err += __builtin_popcount(diff);
    }

    return static_cast<float>(err) / static_cast<float>(total_bits);
}