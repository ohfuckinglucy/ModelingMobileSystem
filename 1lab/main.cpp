#include <iostream>
#include <vector>
#include <bitset>

using namespace std;

template<typename T>
void Show_Array(string title, T* array, int len) {
    cout << title << endl;
    for (int i = 0; i < len; i++) {
        cout << array[i];
    }
    cout << "\n";
}

vector<bitset<8>> encoder(string text){
    vector<bitset<8>> bits;
    bits.reserve(bits.size());

    for (unsigned char let : text) {
        bits.push_back(bitset<8>(let));
    }
    
    return bits;
}

string decoder(vector<bitset<8>> bits){
    string text;
    for (const auto& i : bits){
        text += static_cast<char>(i.to_ulong());
    }

    return text;
}

int main(){
    string input_txt;
    
    cout << "Введите текст (от 30 до 100 символов)" << endl;

    cin >> input_txt;

    if (input_txt.size() < 0 || input_txt.size() >= 100){
        cout << "Некоректная длина сообщения!" << endl;
        return 1;
    }

    Show_Array("Набранный текст: ", input_txt.data(), input_txt.size());

    vector<bitset<8>> bits = encoder(input_txt);

    Show_Array("Биты: ", bits.data(), bits.size());

    string output_txt = decoder(bits);

    Show_Array("Декодированный текст: ", output_txt.data(), output_txt.size());

    return 0;
}