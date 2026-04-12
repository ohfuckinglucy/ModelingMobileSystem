#include "header.hpp"

#include <complex>
#include <vector>
#include <cmath>
#include <fftw3.h>
#include <algorithm>

int compute_OFDM_params(const OFDMConfig& cfg, int& out_NSYM, int& out_RS, int& out_CP){
    const int guard = static_cast<int>(std::floor(cfg.C * cfg.FFT_SIZE / (1 + 2*cfg.C)));
    const int N_active = cfg.FFT_SIZE - 2*guard;
    
    out_RS = static_cast<int>(std::floor(static_cast<double>(N_active) / cfg.RS));
    
    out_NSYM = N_active - out_RS;
    
    out_CP = cfg.FFT_SIZE / cfg.CP_ratio;
    
    return cfg.FFT_SIZE;
}

std::vector<std::complex<float>> ofdm_modulate(const std::vector<std::complex<float>> &symbols, SharedData &sd){
    int NSYM, RS, CP;
    const int N_fft = compute_OFDM_params(sd.OfdmCfg, NSYM, RS, CP);

    std::vector<std::complex<float>> output;
    size_t num_symbols = (symbols.size() + NSYM - 1) / NSYM;
    output.reserve(num_symbols * (sd.OfdmCfg.FFT_SIZE + CP));

    const std::complex<float> pilot(0.707f, 0.707f);

    size_t pos = 0;
    while (pos < symbols.size()){
        size_t chunk_size = std::min(NSYM, static_cast<int>(symbols.size() - pos));
        std::vector<std::complex<float>> chunk(symbols.begin() + pos, symbols.begin() + pos + chunk_size);
        if (chunk_size < NSYM)
            chunk.resize(NSYM, {0.f, 0.f});

        const int N_pl = NSYM + RS;
        const int NZ = static_cast<int>(std::floor(sd.OfdmCfg.C * N_pl));

        std::vector<std::complex<float>> spectrum(sd.OfdmCfg.FFT_SIZE, {0.f, 0.f});

        for (int i = 0; i < RS; ++i){
            int j = NZ + i * sd.OfdmCfg.RS;
            if (j < sd.OfdmCfg.FFT_SIZE)
                spectrum[j] = pilot;
        }

        int data_ptr = 0;
        for (int i = NZ; i < NZ + N_pl && data_ptr < NSYM; ++i)
            if (spectrum[i] == std::complex<float>{0.f, 0.f})
                spectrum[i] = chunk[data_ptr++];


        std::vector<std::complex<float>> time_sym(sd.OfdmCfg.FFT_SIZE);
        std::rotate(spectrum.begin(), spectrum.begin() + sd.OfdmCfg.FFT_SIZE / 2, spectrum.end());

        std::copy_n(spectrum.begin(), sd.OfdmCfg.FFT_SIZE, reinterpret_cast<std::complex<float>*>(sd.in_ifft));

        fftwf_execute(sd.plan_ifft);

        std::copy_n(reinterpret_cast<std::complex<float>*>(sd.out_ifft), sd.OfdmCfg.FFT_SIZE, time_sym.begin());

        for (auto &s : time_sym)
            s /= static_cast<float>(sd.OfdmCfg.FFT_SIZE);

        for (int i = sd.OfdmCfg.FFT_SIZE - CP; i < sd.OfdmCfg.FFT_SIZE; ++i)
            output.push_back(time_sym[i]);

        output.insert(output.end(), time_sym.begin(), time_sym.end());

        pos += chunk_size;
    }

    return output;
}

std::vector<std::complex<float>> ofdm_demodulate(const std::vector<std::complex<float>> &rx_signal, SharedData &sd) {
    int NSYM, RS_count, CP;
    const int N_fft = compute_OFDM_params(sd.OfdmCfg, NSYM, RS_count, CP);
    const int N_pl = NSYM + RS_count;
    const int NZ = static_cast<int>(std::floor(sd.OfdmCfg.C * N_pl));

    std::vector<std::complex<float>> output;
    size_t symbol_len = N_fft + CP;
    size_t num_syms = rx_signal.size() / symbol_len;
    output.reserve(num_syms * NSYM);

    std::vector<std::complex<float>> freq_buf(N_fft);

    for (size_t s = 0; s < num_syms; ++s) {
        size_t start = s * symbol_len;
        
        std::copy_n(rx_signal.begin() + start + CP, N_fft, reinterpret_cast<std::complex<float>*>(sd.in_fft));

        fftwf_execute(sd.plan_fft);

        std::copy_n(reinterpret_cast<std::complex<float>*>(sd.out_fft), N_fft, freq_buf.begin());

        std::rotate(freq_buf.begin(), freq_buf.begin() + (N_fft + 1) / 2, freq_buf.end());

        int data_count = 0;
        for (int i = 0; i < N_pl && data_count < NSYM; ++i) {
            if (i % sd.OfdmCfg.RS != 0) {
                output.push_back(freq_buf[NZ + i]);
                data_count++;
            }
        }
    }

    return output;
}

std::vector<float> calculate_spectrum(const std::vector<std::complex<float>>& time_domain_signal, SharedData& sd) {
    if (time_domain_signal.empty()) return {};

    int N = sd.OfdmCfg.FFT_SIZE;
    std::vector<float> magnitudes(N);

    std::copy_n(time_domain_signal.begin(), N, reinterpret_cast<std::complex<float>*>(sd.in_fft));

    fftwf_execute(sd.plan_fft);

    std::vector<std::complex<float>> freq_data(N);
    std::copy_n(reinterpret_cast<std::complex<float>*>(sd.out_fft), N, freq_data.begin());

    std::rotate(freq_data.begin(), freq_data.begin() + N/2, freq_data.end());

    for (int i = 0; i < N; ++i) {
        float mag = std::abs(freq_data[i]);
        magnitudes[i] = 20.0f * std::log10(mag + 1e-6f);
    }

    return magnitudes;
}
