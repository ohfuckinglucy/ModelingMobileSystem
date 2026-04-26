#include "header.hpp"

std::vector<std::complex<float>> QPSK_modulator(const std::vector<uint32_t> &bits)
{
    const float inv_sqrt2 = 1.0f / sqrtf(2.0f);
    std::vector<std::complex<float>> symbols;
    symbols.reserve(bits.size() * 15);

    for (uint32_t val : bits)
    {
        for (int j = 0; j < 30; j += 2)
        {
            uint32_t b1 = (val >> (29 - j)) & 1U;
            uint32_t b2 = (val >> (28 - j)) & 1U;

            float I = b1 ? -inv_sqrt2 : inv_sqrt2;
            float Q = b2 ? -inv_sqrt2 : inv_sqrt2;

            symbols.emplace_back(I, Q);
        }
    }

    return symbols;
}

std::vector<uint32_t> QPSK_demodulator(std::vector<std::complex<float>> &symbols)
{
    std::vector<uint32_t> words;
    words.reserve(symbols.size() / 15);

    for (size_t i = 0; i + 15 <= symbols.size(); i += 15)
    {
        uint32_t word = 0;

        for (int j = 0; j < 15; ++j)
        {
            float I = symbols[i + j].real();
            float Q = symbols[i + j].imag();

            uint32_t bit_I = (I > 0) ? 0U : 1U;
            uint32_t bit_Q = (Q > 0) ? 0U : 1U;

            word |= (bit_I << (29 - 2 * j));
            word |= (bit_Q << (28 - 2 * j));
        }

        words.push_back(word);
    }

    return words;
}