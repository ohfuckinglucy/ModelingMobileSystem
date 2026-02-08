#include <iostream>
#include <vector>
#include <cstdint>

using namespace std;

const uint8_t G1 = 0b1111001; // 171 oct
const uint8_t G2 = 0b1011011; // 133 oct
// k/n = 1/2

template<typename T>
void Show_Array(string title, T* array, int len) {
    cout << title << endl;
    for (int i = 0; i < len; i++) {
        cout << array[i];
    }
    cout << "\n";
}

void show_bits(const vector<uint8_t>& bytes){
    printf("Биты: ");
    for (uint8_t byte : bytes){
        for (int i = 7; i >= 0; --i){
            printf("%d", (byte >> i) & 1);
        }
    }
    printf("\n");
    cout << "Длина " << bytes.size() * 8 << "бит\n";
}

vector<uint8_t> str_to_bytes(const string& text) {
    vector<uint8_t> bytes;
    for (auto i : text){
        bytes.push_back((uint8_t)i);
    }
    return bytes;
}

string bytes_to_str(vector<uint8_t> bytes){
    string text = "";
    for (auto i : bytes){
        text += (u_char)i;
    }

    return text;
}

vector<bool> conv_encoder(const vector<uint8_t>& bytes) {
    vector<bool> output;
    uint8_t reg[6] = {0};

    for (uint8_t byte : bytes) {
        for (int i = 7; i >= 0; --i) {
            uint8_t u = (byte >> i) & 1;

            uint8_t x = u ^ reg[5] ^ reg[4] ^ reg[3] ^ reg[0];
            uint8_t y = u ^ reg[4] ^ reg[3] ^ reg[1] ^ reg[0];

            output.push_back(x);
            output.push_back(y);

            for (int j = 5; j > 0; --j) {
                reg[j] = reg[j-1];
            }
            reg[0] = u;
        }
    }

    return output;
}

int main(){
    string input_txt;
    cout << "Введите текст (от 30 до 100 символов): ";
    getline(cin, input_txt);

    cout << "Исходный текст: " << input_txt << "\n";

    vector<uint8_t> bytes = str_to_bytes(input_txt);

    show_bits(bytes);

    vector<bool> enc_bits = conv_encoder(bytes);

    cout << "Сверточные биты: ";
    for (bool bit : enc_bits) {
        cout << bit;
    }
    cout << "\nДлина: " << enc_bits.size() << " бит\n";

    // vector<uint8_t> dec_bits = vit_decoder(enc_bits);

    // string output_txt = bytes_to_str(dec_bits);
    // cout << "Конечный текст: " << output_txt << "\n";

    return 0;
}