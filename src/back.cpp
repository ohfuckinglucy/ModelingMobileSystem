#include "header.hpp"

#include <chrono>
#include <iostream>
#include <random>

std::vector<uint8_t> generate_random_data(size_t size)
{
    static std::mt19937 gen(std::chrono::steady_clock::now().time_since_epoch().count());
    static std::uniform_int_distribution<uint8_t> dis(0, 255);

    std::vector<uint8_t> data(size);
    for (auto &byte : data)
        byte = dis(gen);
    return data;
}

void back(SharedData &sd)
{
    while (sd.back_running.load())
    {
        bool process_frame = false;
        std::vector<uint8_t> current_tx_data;

        if (sd.is_realtime.load())
        {
            process_frame = true;
            current_tx_data = generate_random_data(PACKET_SIZE);

            sd.tx_buf.assign(current_tx_data.begin(), current_tx_data.end());
        }
        else
        {
            if (sd.input_flag.load())
            {
                if (sd.tx_buf.length() < 1 || sd.tx_buf.length() >= 101)
                {
                    std::cout << "[WARNING] Некоректная длина сообщения! " << sd.tx_buf.length() << std::endl;
                    sd.input_flag.store(false);
                    continue;
                }

                if (sd.experiment.load())
                {
                    sd.experiment.store(false);
                    run_experiment(sd);
                }

                process_frame = true;
            }
        }

        if (process_frame)
        {
            sd.input_flag.store(false);

            size_t original_size = sd.tx_buf.size();

            sd.bytes = encoder(sd.tx_buf);
            sd.encoded_bytes = hamming_encoder(sd.bytes);
            sd.interleavin_block = interleaving(sd.encoded_bytes);
            sd.symbols = QPSK_modulator(sd.interleavin_block);
            sd.ofdm_symbols = ofdm_modulator(sd.symbols, std::ref(sd));

            sd.signal = chanel_model(sd.ofdm_symbols, std::ref(sd));
            sd.rx_spectrum = spectrum_calculate(sd.signal, std::ref(sd));

            sd.symbols_rx = ofdm_demodulator(sd.signal, std::ref(sd));
            sd.words = QPSK_demodulator(sd.symbols_rx);
            sd.deinterleavin_block = deinterleaving(sd.words);
            sd.decoded_bytes = hamming_decoder(sd.deinterleavin_block);
            sd.rx_buf = decoder(sd.decoded_bytes);

            sd.rx_buf.resize(original_size);

            std::vector<uint8_t> tx(sd.tx_buf.begin(), sd.tx_buf.end());
            std::vector<uint8_t> rx(sd.rx_buf.begin(), sd.rx_buf.end());

            sd.BER = BER(tx, rx);
            sd.BER_vec[sd.ber_vec_offset] = sd.BER;
            sd.ber_vec_offset = (sd.ber_vec_offset + 1) % sd.ber_vec_size;
            sd.ber_frames_processed++;

            if (!sd.is_realtime.load())
                sd.input_flag.store(false);
        }
    }
}