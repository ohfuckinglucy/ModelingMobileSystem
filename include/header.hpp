#pragma once

#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <atomic>
#include <complex>
#include <mutex>

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_sdl2.h"
#include "imgui.h"
#include "implot.h"

struct SharedData{
    std::mutex mtx;

    std::string tx_buf;
    std::string rx_buf;
    std::vector<uint8_t> bytes;
    std::vector<uint32_t> encoded_bytes;
    std::vector<uint8_t> decoded_bytes;
    std::vector<uint32_t> interleavin_block;
    std::vector<uint32_t> deinterleavin_block;
    std::vector<std::complex<float>> symbols;
    std::vector<uint32_t> words;

    std::string hamming_log;

    std::atomic<bool> back_running = true;
    std::atomic<bool> input_flag = false;
};

void back(SharedData &sd);

std::vector<uint8_t> encoder(std::string text);
std::string decoder(std::vector<uint8_t> &bytes);

std::vector<uint32_t> hamming_encoder(std::vector<uint8_t> &bytes);
std::vector<uint8_t> hamming_decoder(std::vector<uint32_t> &encoded_bytes);

std::vector<uint32_t> interleaving(std::vector<uint32_t> &hamming_encoded);
std::vector<uint32_t> deinterleaving(std::vector<uint32_t> interleaving_block);

std::vector<std::complex<float>> QPSK_modulator(const std::vector<uint32_t> &bits);
std::vector<uint32_t> QPSK_demodulator(std::vector<std::complex<float>> &symbols);