#include "header.hpp"

std::vector<uint32_t> interleaving(std::vector<uint32_t> &hamming_encoded){
    if (hamming_encoded.empty())
        return {};
    constexpr size_t B = 32;
    const size_t N = hamming_encoded.size();

    std::vector<uint32_t> output(N, 0);
    for (size_t g = 0; g < N * B; ++g)
    {
        size_t sw = g / B, sb = B - 1 - (g % B);
        uint32_t bit = (hamming_encoded[sw] >> sb) & 1U;
        size_t col = g / N, row = g % N;
        size_t dg = col * N + row;
        size_t dw = dg / B;
        size_t db = B - 1 - (dg % B);
        output[dw] |= (bit << db);
    }
    return output;
}

std::vector<uint32_t> deinterleaving(std::vector<uint32_t> interleaving_block){
    if (interleaving_block.empty())
        return {};
    constexpr size_t B = 32;
    const size_t N = interleaving_block.size();

    std::vector<uint32_t> output(N, 0);
    for (size_t g = 0; g < N * B; ++g)
    {
        size_t col = g / N, row = g % N;

        size_t sg = col * N + row;
        size_t sw = sg / B, sb = B - 1 - (sg % B);

        uint32_t bit = (interleaving_block[sw] >> sb) & 1U;

        size_t dw = g / B, db = B - 1 - (g % B);
        output[dw] |= (bit << db);
    }
    return output;

}