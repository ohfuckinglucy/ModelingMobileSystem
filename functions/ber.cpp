#include "header.hpp"
#include <cmath>
#include <random>

#include <iostream>

float BER(const std::vector<uint8_t> &tx, const std::vector<uint8_t> &rx)
{
    if (tx.size() != rx.size())
    {
        std::cout << "[WARNING] Длины сообщений не соответствуют!\n";
        return -1.0f;
    }

    size_t err = 0;
    size_t total_bits = tx.size() * 8;

    for (size_t i = 0; i < tx.size(); ++i)
    {
        uint8_t diff = tx[i] ^ rx[i];

        err += __builtin_popcount(diff);
    }

    return static_cast<float>(err) / static_cast<float>(total_bits);
}

void run_experiment(SharedData &sd)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> bit(0, 1);

    std::vector<float> snr_db_vals;
    std::vector<float> ber_vals;
    std::vector<float> ber_theory;

    for (float snr_db = -100; snr_db <= 0; snr_db += 1)
    {
        sd.ChannelCfg.N_0 = snr_db;

        size_t errors = 0;
        size_t trials = 200;

        for (size_t t = 0; t < trials; ++t)
        {
            std::string tx;
            for (int i = 0; i < 100; ++i)
                tx += (bit(gen) ? '1' : '0');

            sd.tx_buf = tx;

            sd.bytes = encoder(sd.tx_buf);
            sd.encoded_bytes = hamming_encoder(sd.bytes);
            sd.interleavin_block = interleaving(sd.encoded_bytes);
            sd.symbols = QPSK_modulator(sd.interleavin_block);
            sd.ofdm_symbols = ofdm_modulator(sd.symbols, sd);

            sd.signal = chanel_model(sd.ofdm_symbols, sd);
            sd.rx_spectrum = spectrum_calculate(sd.signal, std::ref(sd));

            sd.symbols_rx = ofdm_demodulator(sd.signal, sd);
            sd.words = QPSK_demodulator(sd.symbols_rx);
            sd.deinterleavin_block = deinterleaving(sd.words);
            sd.decoded_bytes = hamming_decoder(sd.deinterleavin_block);
            sd.rx_buf = decoder(sd.decoded_bytes);

            sd.rx_buf.resize(sd.tx_buf.size());

            std::vector<uint8_t> txb(sd.tx_buf.begin(), sd.tx_buf.end());
            std::vector<uint8_t> rxb(sd.rx_buf.begin(), sd.rx_buf.end());

            float ber = BER(txb, rxb);
            errors += ber * txb.size() * 8;
        }

        float ber_avg = (float)errors / (trials * 100 * 8);

        snr_db_vals.push_back(snr_db);
        ber_vals.push_back(ber_avg);
    }

    sd.ber_curve = ber_vals;
}