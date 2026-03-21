#pragma once

#include <iostream>
#include <vector>
#include <cstring>
#include <cstdint>

template<typename T>
void Show_Text(std::string title, T* array, int len) {
    std::cout << title;
    for (int i = 0; i < len; i++) {
        std::cout << (array[i]);
    }
    std::cout << "\n";
}

template<typename T>
void Show_Bytes(std::string title, T* array, int len) {
    std::cout << title;
    for (int i = 0; i < len; i++) {
        std::cout << static_cast<int16_t>(array[i]);
    }
    std::cout << "\n";
}

template<typename T>
void Show_Bits(std::string title, T* array, int len, int r) {
    std::cout << title;
    for (int i = 0; i < len; i++) {
        for (int j = r - 1; j >= 0; --j){
            std::cout << static_cast<int16_t>((array[i] >> j) & 1);
        }
    }
    std::cout << "\n";
    std::cout << "Длина бит: " << len * r << "\n";
}

std::vector<uint8_t> encoder(std::string text);
std::string decoder(std::vector<uint8_t> &bytes);

std::vector<uint32_t> hamming_encoder(std::vector<uint8_t> &bytes);
std::vector<uint8_t> hamming_decoder(std::vector<uint32_t> &encoded_bytes);