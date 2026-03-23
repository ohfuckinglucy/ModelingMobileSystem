#include "header.hpp"

std::vector<uint32_t> interleaving(std::vector<uint32_t> &hamming_encoded){
    if (hamming_encoded.size() < 1){
        std::cerr << "[ERROR] Недостаточный размер!" << std::endl;
        return {};
    }

    int N = hamming_encoded.size();
    int M = 32;

    std::vector<uint32_t> interleaving_block(N, 0);

    for (int col = 0; col < M; ++col){
        for (int row = 0; row < N; ++row){
            int bit = (hamming_encoded[row] >> (31 - col)) & 1U;

            int i = (col * N + row) / M;
            int bit_pos = 31 - ((col * N + row) % M);

            if (i < N) {
                interleaving_block[i] |= (bit << bit_pos);
            }
        }
    }

    return interleaving_block;
}

std::vector<uint32_t> deinterleaving(std::vector<uint32_t> interleaving_block){
    int N = interleaving_block.size();
    int M = 32;

    std::vector<uint32_t> deinterleaving_block(N, 0);

    for (int col = 0; col < M; ++col) {
        for (int row = 0; row < N; ++row) {
            int in_i = (col * N + row) / M;
            int in_bit = 31 - ((col * N + row) % M);

            int bit = (interleaving_block[in_i] >> in_bit) & 1U;

            deinterleaving_block[row] |= (bit << (31 - col));
        }
    }

    return deinterleaving_block;
}