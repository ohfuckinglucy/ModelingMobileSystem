#include "header.hpp"

int main(int argc, char *argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);

    SDL_Window* window = SDL_CreateWindow(
        "Backend start", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1920, 1080, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);

    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.f;
    style.FrameRounding = 8.f;
    style.ChildRounding = 8.f;
    style.ScrollbarRounding = 10.f;
    style.TabRounding = 8.f;
    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.PopupBorderSize = 0.0f;
    style.WindowPadding = ImVec2(10, 10);
    style.FramePadding = ImVec2(4, 4);
    style.ItemSpacing = ImVec2(8, 8);
    style.ItemInnerSpacing = ImVec2(4, 4);

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 330");

    SharedData sd;

    std::thread Back;
    Back = std::thread(back, std::ref(sd));

    char local_buffer[101] = "";

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) {
                running = false;
                sd.back_running.store(false, std::memory_order_release);
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
        ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_None);

        ImGui::Begin("Log");

        ImGui::Text("Input text:");
        if (ImGui::InputText("##input", local_buffer, sizeof(local_buffer), 
                            ImGuiInputTextFlags_EnterReturnsTrue)) {

            
            sd.tx_buf = local_buffer;

            sd.input_flag.store(true, std::memory_order_release);
            memset(local_buffer, 0, sizeof(local_buffer)); 
        }

        ImGui::Text("Message: %s\nLen: %ld", sd.tx_buf.data(), sd.tx_buf.length());
        ImGui::Text("Rx message: %s\nLen: %ld", sd.rx_buf.data(), sd.rx_buf.length());

        ImGui::SeparatorText("Log");
        std::string bytes_str;
        for (size_t i = 0; i < sd.bytes.size(); ++i) {
            char buf[16];
            snprintf(buf, sizeof(buf), "0x%02X ", sd.bytes[i]);
            bytes_str += buf;
        }
        ImGui::Text("Input bytes: %s", bytes_str.c_str());

        std::string bits_str;
        for (size_t i = 0; i < sd.bytes.size(); ++i) {
            for (int j = 7; j >= 0; --j){
                bits_str += std::to_string((sd.bytes[i] >> j) & 1U);
            }
            bits_str += " ";
        }
        ImGui::Text("Input bits: %s", bits_str.c_str());

        std::string encoded_bytes_str;
        for (size_t i = 0; i < sd.encoded_bytes.size(); ++i) {
            char buf[32];
            snprintf(buf, sizeof(buf), "0x%08X ", sd.encoded_bytes[i]);
            encoded_bytes_str += buf;
        }
        ImGui::Text("Encoded bytes: %s", encoded_bytes_str.c_str());

        std::string decoded_bytes_str;
        for (size_t i = 0; i < sd.decoded_bytes.size(); ++i) {
            char buf[16];
            snprintf(buf, sizeof(buf), "0x%02X ", sd.decoded_bytes[i]);
            decoded_bytes_str += buf;
        }
        ImGui::Text("Decoded bytes: %s", decoded_bytes_str.c_str());

        ImGui::End();

        ImGui::Begin("Plots");

        if (ImPlot::BeginPlot("Rx Bits", ImVec2(-1, 200))){
            std::vector<int16_t> bits;
            bits.reserve(sd.bytes.size() * 8);
            for (size_t i = 0; i < sd.bytes.size(); ++i){
                for (int j = 7; j >= 0; --j){
                    bits.push_back((sd.bytes[i] >> j) & 1U);
                }
            }
            ImPlot::PlotLine("Биты", bits.data(), bits.size());
            ImPlot::EndPlot();
        }

        if (ImPlot::BeginPlot("I/Q Symbols", ImVec2(-1, 200))){
            std::vector<float> I;
            std::vector<float> Q;

            I.reserve(sd.symbols.size());
            Q.reserve(sd.symbols.size());

            for (size_t i = 0; i < sd.symbols.size(); i ++){
                I.push_back(sd.symbols[i].real());
                Q.push_back(sd.symbols[i].imag());
            }

            ImPlot::PlotLine("Real", I.data(), I.size());
            ImPlot::PlotLine("Imag", Q.data(), Q.size());
            ImPlot::EndPlot();
        }

        ImGui::End();

        ImGui::Begin("Constellation");

        if (ImPlot::BeginPlot("I/Q", ImVec2(600, 600))){
            std::vector<float> I;
            std::vector<float> Q;

            I.reserve(sd.symbols.size());
            Q.reserve(sd.symbols.size());

            for (size_t i = 0; i < sd.symbols.size(); i ++){
                I.push_back(sd.symbols[i].real());
                Q.push_back(sd.symbols[i].imag());
            }

            ImPlot::SetupAxesLimits(-1.5, 1.5, -1.5, 1.5);

            ImPlot::SetupFinish();

            ImPlot::PlotScatter("IQ", I.data(), Q.data(), I.size());

            ImPlot::EndPlot();
        }

        ImGui::End();

        ImGui::Render();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window);
    }

    if (Back.joinable()){
        Back.join();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}