#include "header.hpp"

std::vector<uint8_t> encoder(std::string text)
{
    std::vector<uint8_t> bits;
    bits.reserve(text.size());

    for (unsigned char let : text)
    {
        bits.push_back(uint8_t(let));
    }

    return { bits };
}

std::string decoder(std::vector<uint8_t> &bytes)
{
    std::string text;
    text.reserve(bytes.size());

    for (const auto &i : bytes)
    {
        text += static_cast<char>(i);
    }

    return text;
}