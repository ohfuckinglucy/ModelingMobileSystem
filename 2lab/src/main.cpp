#include "header.hpp"

int main(){
    std::string input_txt;
    
    std::cout << "Введите текст (до 100 символов)" << std::endl;

    std::cin >> input_txt;

    if (input_txt.size() < 1 || input_txt.size() >= 100){
        std::cout << "Некоректная длина сообщения!" << std::endl;
        return 1;
    }
    
    Show_Text("Набранный текст: ", input_txt.data(), input_txt.size());
    
    std::vector<uint8_t> bytes = encoder(input_txt);
    Show_Bits("Биты: ", bytes.data(), bytes.size(), 8);

    std::vector<uint32_t> encoded_bytes = hamming_encoder(bytes);
    Show_Bits("Кодированные биты: ", encoded_bytes.data(), encoded_bytes.size(), 32);

    std::vector<uint8_t> decoded_bytes = hamming_decoder(encoded_bytes);
    Show_Bits("Декодированные биты: ", decoded_bytes.data(), decoded_bytes.size(), 8);

    std::string output_txt = decoder(decoded_bytes);

    Show_Text("Декодированный текст: ", output_txt.data(), output_txt.size());

    return 0;
}