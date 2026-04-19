#include "header.hpp"

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <fftw3.h>
#include <vector>

int compute_OFDM_params(const OFDMConfig &cfg, int &out_NSYM, int &out_RS, int &out_CP)
{
    const int guard = static_cast<int>(std::floor(cfg.C * cfg.FFT_SIZE / (1 + 2 * cfg.C)));
    const int N_active = cfg.FFT_SIZE - 2 * guard;

    out_RS = static_cast<int>(std::floor(static_cast<double>(N_active) / cfg.RS));

    out_NSYM = N_active - out_RS;
    out_CP = cfg.FFT_SIZE / cfg.CP_ratio;

    return cfg.FFT_SIZE;
}

std::vector<std::complex<float>> ofdm_modulator(const std::vector<std::complex<float>> &symbols, SharedData &sd)
{
    int NSYM, RS, CP;
    const int N_fft = compute_OFDM_params(sd.OfdmCfg, NSYM, RS, CP);
    const int N_pl = NSYM + RS;
    const int NZ = static_cast<int>(std::floor(sd.OfdmCfg.C * N_pl));

    std::vector<std::complex<float>> output;
    size_t num_symbols = (symbols.size() + NSYM - 1) / NSYM;
    output.reserve(num_symbols * (N_fft + CP));

    const std::complex<float> pilot(0.707f, 0.707f);

    size_t pos = 0;
    while (pos < symbols.size())
    {
        size_t chunk_size = std::min(NSYM, static_cast<int>(symbols.size() - pos));
        std::vector<std::complex<float>> chunk(symbols.begin() + pos, symbols.begin() + pos + chunk_size);
        if (chunk_size < NSYM)
            chunk.resize(NSYM, { 0.f, 0.f });

        std::vector<bool> is_pilot(N_fft, false);
        std::vector<std::complex<float>> freq(N_fft, { 0.f, 0.f });
        for (int i = 0; i < RS; ++i)
        {
            int j = NZ + i * sd.OfdmCfg.RS;
            if (j < N_fft)
            {
                freq[j] = pilot;
                is_pilot[j] = true;
            }
        }

        int data_ptr = 0;
        for (int i = NZ; i < NZ + N_pl && data_ptr < NSYM; ++i)
        {
            if (!is_pilot[i])
            {
                freq[i] = chunk[data_ptr++];
            }
        }

        std::vector<std::complex<float>> time_sym(N_fft);

        std::copy_n(freq.begin(), N_fft, reinterpret_cast<std::complex<float> *>(sd.in_ifft));
        fftwf_execute(sd.plan_ifft);
        std::copy_n(reinterpret_cast<std::complex<float> *>(sd.out_ifft), N_fft, time_sym.begin());

        for (auto &s : time_sym)
            s /= static_cast<float>(N_fft);

        for (int i = sd.OfdmCfg.FFT_SIZE - CP; i < sd.OfdmCfg.FFT_SIZE; ++i)
            output.push_back(time_sym[i]);

        output.insert(output.end(), time_sym.begin(), time_sym.end());

        pos += chunk_size;
    }

    return output;
}

std::vector<std::complex<float>> ofdm_demodulator(const std::vector<std::complex<float>> &rx_signal, SharedData &sd)
{
    int NSYM, RS, CP;
    const int N_fft = compute_OFDM_params(sd.OfdmCfg, NSYM, RS, CP);
    const int N_pl = NSYM + RS;
    const int NZ = static_cast<int>(std::floor(sd.OfdmCfg.C * N_pl));

    std::vector<std::complex<float>> output;
    size_t symbol_len = N_fft + CP;
    size_t num_symbols = rx_signal.size() / symbol_len;
    output.reserve(num_symbols * NSYM);

    std::vector<bool> is_pilot(N_fft, false);
    for (int i = 0; i < RS; ++i)
    {
        int j = NZ + i * sd.OfdmCfg.RS;
        if (j < N_fft)
            is_pilot[j] = true;
    }

    std::vector<std::complex<float>> freq(N_fft);

    for (size_t sym = 0; sym < num_symbols; ++sym)
    {
        size_t pos = sym * symbol_len;
        std::copy_n(rx_signal.begin() + pos + CP, N_fft, reinterpret_cast<std::complex<float> *>(sd.in_fft));

        fftwf_execute(sd.plan_fft);

        std::copy_n(reinterpret_cast<std::complex<float> *>(sd.out_fft), N_fft, freq.begin());

        int data_pos = 0;
        for (int i = NZ; i < NZ + N_pl && data_pos < NSYM; ++i)
        {
            if (!is_pilot[i])
            {
                output.push_back(freq[i]);
                data_pos++;
            }
        }
    }

    return output;
}

std::vector<float> spectrum_calculate(const std::vector<std::complex<float>> &signal, SharedData &sd) {
    std::vector<std::complex<float>> local_signal = signal;
    local_signal.resize(1024, { 0.f, 0.f });

    std::copy_n(local_signal.data(), 1024, reinterpret_cast<std::complex<float> *>(sd.in_spectrum));

    fftwf_execute(sd.plan_spectrum);

    std::vector<float> spectrum_db(1024);
    std::complex<float>* out_ptr = reinterpret_cast<std::complex<float> *>(sd.out_spectrum);

    for (int i = 0; i < 1024; ++i) {
        float power = std::norm(out_ptr[i]);
        spectrum_db[i] = 10.0f * std::log10(power + 1e-12f);
    }

    return spectrum_db;
}
