#include "header.hpp"

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
            sd.deinterleavin_block = deinterleaving(sd.interleavin_block);
            sd.symbols = QPSK_modulator(sd.deinterleavin_block);
            sd.words = QPSK_demodulator(sd.symbols);
            sd.decoded_bytes = hamming_decoder(sd.words);
            sd.rx_buf = decoder(sd.decoded_bytes);
        }
    }
}