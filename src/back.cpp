#include "header.hpp"
#include <iostream>

void back(SharedData &sd){
    while (sd.back_running.load()){
        if (sd.input_flag.load()){
            if (sd.tx_buf.length() < 1 || sd.tx_buf.length() >= 101){
                std::cout << "[WARNING] Некоректная длина сообщения! "  << sd.tx_buf.length() << std::endl;
                sd.input_flag.store(false);
                continue;
            }

            sd.input_flag.store(false);

            sd.bytes = encoder(sd.tx_buf);
            sd.encoded_bytes = hamming_encoder(sd.bytes);
            sd.interleavin_block = interleaving(sd.encoded_bytes);
            sd.symbols = QPSK_modulator(sd.interleavin_block);
            sd.ofdm_symbols = ofdm_modulate(sd.symbols, std::ref(sd));

            sd.rx_spectrum = calculate_spectrum(sd.ofdm_symbols, std::ref(sd));

            sd.symbols_rx = ofdm_demodulate(sd.ofdm_symbols, std::ref(sd));
            sd.words = QPSK_demodulator(sd.symbols_rx);
            sd.deinterleavin_block = deinterleaving(sd.words);
            sd.decoded_bytes = hamming_decoder(sd.words);
            sd.rx_buf = decoder(sd.decoded_bytes);
        }
    }
}