#include "header.hpp"

#include <iostream>

std::vector<uint32_t> hamming_encoder(std::vector<uint8_t> &bytes)
{
    if (bytes.size() < 1)
    {
        std::cerr << "[ERROR] Недостаточно байт!" << std::endl;
        return {};
    }

    while (bytes.size() % 3 != 0)
    {
        bytes.push_back(0);
    }

    std::vector<uint32_t> encoded_bytes;
    encoded_bytes.reserve(bytes.size() / 3);

    for (size_t i = 0; i + 2 < bytes.size(); i += 3)
    {
        uint32_t data24 = (static_cast<uint32_t>(bytes[i]) << 16) | (static_cast<uint32_t>(bytes[i + 1])) << 8 | bytes[i + 2];

        uint32_t block = 0;
        uint8_t checksum = 0;
        int data_bit_pos = 23;

        for (int j = 1; j <= 30; ++j)
        {
            if ((j & (j - 1)) == 0)
            {
                continue;
            }
            if (data_bit_pos >= 0)
            {
                if ((data24 >> data_bit_pos) & 1)
                {
                    block |= (1 << (j - 1));
                    checksum ^= j;
                }
                data_bit_pos--;
            }
        }

        for (int k = 0; k < 5; ++k)
        {
            if ((checksum >> k) & 1)
            {
                block |= (1 << ((1 << k) - 1));
            }
        }

        encoded_bytes.push_back(block);
    }

    return encoded_bytes;
}

std::vector<uint8_t> hamming_decoder(std::vector<uint32_t> &encoded_bytes)
{
    if (encoded_bytes.size() < 1)
    {
        std::cerr << "[ERROR] Недостаточно байт!" << std::endl;
        return {};
    }

    std::vector<uint8_t> decoded_bytes;
    decoded_bytes.reserve(encoded_bytes.size() * 3);

    for (size_t i = 0; i < encoded_bytes.size(); ++i)
    {
        uint8_t syndrome = 0;
        uint32_t block = 0;

        int data_bit_pos = 23;

        for (int j = 1; j <= 30; ++j)
        {
            if ((encoded_bytes[i] >> (j - 1)) & 1)
            {
                syndrome ^= j;
            }
        }

        if (syndrome == 0)
        {
            // std::cerr << "[DEBUG] OK" << std::endl;
        }
        else
        {
            encoded_bytes[i] ^= (1U << (syndrome - 1));
        }

        for (int j = 1; j <= 30; ++j)
        {
            if ((j & (j - 1)) == 0)
                continue;

            if (data_bit_pos < 0)
                break;

            if ((encoded_bytes[i] >> (j - 1)) & 1)
            {
                block |= (1U << data_bit_pos);
            }
            data_bit_pos--;
        }

        decoded_bytes.push_back((block >> 16) & 0xFF);
        decoded_bytes.push_back((block >> 8) & 0xFF);
        decoded_bytes.push_back(block & 0xFF);
    }

    return decoded_bytes;
}