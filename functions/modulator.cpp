#include "header.hpp"

std::vector<std::complex<float>> QPSK_modulator(const std::vector<uint32_t> &bits) {
    const float inv_sqrt2 = 1.0f / sqrtf(2.0f);
    std::vector<std::complex<float>> symbols;
    symbols.reserve(bits.size() * 16);

    for (uint32_t val : bits) {
        for (int j = 30; j >= 0; j -= 2) {
            float I = (val >> (j + 1) & 1U) ? -inv_sqrt2 : inv_sqrt2;
            float Q = (val >> j & 1U) ? -inv_sqrt2 : inv_sqrt2;
            
            symbols.emplace_back(I, Q);
        }
    }
    return symbols;
}

std::vector<uint32_t> QPSK_demodulator(std::vector<std::complex<float>> &symbols){
    if (symbols.empty()){
        std::cerr << "[ERROR] Symbols empty!" << std::endl;
    }
    std::vector<uint32_t> words;
    words.reserve(symbols.size() / 16);
    
    for (size_t i = 0; i + 15 < symbols.size(); i += 16){
        uint32_t word = 0;

        for (int j = 0; j < 16; ++j){
            float I = symbols[i + j].real();
            float Q = symbols[i + j].imag();

            uint32_t bit_I = (I > 0) ? 0U : 1U;
            uint32_t bit_Q = (Q > 0) ? 0U : 1U;

            int pos_I = 31 - (j * 2);
            int pos_Q = 30 - (j * 2);

            word |= (bit_I << pos_I);
            word |= (bit_Q << pos_Q);
        }

        words.push_back(word);
    }

    return words;
}