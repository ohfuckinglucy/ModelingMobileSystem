#pragma once

#include <vector>
#include <cstring>
#include <cstdint>
#include <cmath>
#include <atomic>
#include <complex>
#include <mutex>
#include <fftw3.h>

#include <GL/glew.h>
#include <SDL2/SDL.h>

struct OFDMConfig {
    int FFT_SIZE = 128;
    int RS = 6;
    float C = 0.25;
    int CP_ratio = 8;
};

struct SharedData{
    struct OFDMConfig OfdmCfg;

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
    std::vector<std::complex<float>> ofdm_symbols;
    std::vector<std::complex<float>> symbols_rx;

    std::vector<float> rx_spectrum;

    std::string hamming_log;

    std::atomic<bool> back_running = true;
    std::atomic<bool> input_flag = false;

    fftwf_plan plan_ifft;
    fftwf_complex *in_ifft;
    fftwf_complex *out_ifft;

    fftwf_plan plan_fft;
    fftwf_complex *in_fft;
    fftwf_complex *out_fft;
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

std::vector<std::complex<float>> ofdm_modulate(const std::vector<std::complex<float>> &symbols, SharedData &sd);
std::vector<std::complex<float>> ofdm_demodulate(const std::vector<std::complex<float>> &rx_signal, SharedData &sd);

std::vector<float> calculate_spectrum(const std::vector<std::complex<float>>& time_domain_signal, SharedData& sd);