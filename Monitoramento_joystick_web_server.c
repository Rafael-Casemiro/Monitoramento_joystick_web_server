// Declaração de bibliotecas
#include <stdio.h> // Biblioteca padrão de entrada e saída
#include "pico/stdlib.h" // Biblioteca padrão do Raspberry Pi Pico
#include "hardware/adc.h" // Biblioteca para manipulação do ADC 
#include "pico/cyw43_arch.h" // Controla o chip Wi-Fi CYW43 do Pico W
#include "string.h" // Biblioteca para manipulação de strings
#include "lwip/pbuf.h" // Estrutura de buffers usada para armazenar pacotes de rede
#include "lwip/tcp.h" // Biblioteca que gerencia conexões TCP/IP
#include "lwip/netif.h" // Para acessar netif_default e IP
#include "joystick.h" // Arquivo para configuração do joystick

// Configuração da rede Wi-Fi
#define WIFI_SSID "RAFAEL"
#define WIFI_PASSWORD "rafael120905!"

uint eixo_x; // Variável para monitoramento do eixo x
uint eixo_y; // Variável para monitoramento do eixo y

char resposta_http[1024]; // Buffer para resposta HTTP
char posicao_bussola[50]; // Mensagem para passar a posição da rosa dos ventos

// Função para criar a resposta HTTP
void criar_resposta_http() {
    snprintf(resposta_http, sizeof(resposta_http),
        "HTTP/1.1 200 OK\r\n"
        "Cache-Control: no-cache, no-store, must-revalidate, max-age=0\r\n"
        "Pragma: no-cache\r\n"
        "Expires: 0\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n\r\n"
        "<!DOCTYPE html>"
        "<html>\n"
        "<head>\n"
        "<title>Servidor</title>\n"
        "<meta http-equiv='refresh' content='1'/>\n"
        "<style>\n"
        "body { font-family: Arial, Helvetica, sans-serif; text-align: center }\n"
        "p { font-size: x-large; }\n"
        "</style>\n"
        "</head>\n"
        "<body>\n"
        "<h1>Monitoramento Joystick:</h1>\n"
        "<p>Eixo X:  %u</p>\n"
        "<p>Eixo Y: %u</p>\n"
        "<p>Bussola: %s</p>\n"
        "</body>\n"
        "<script>\n"
        "setTimeout(() => { window.location.href = \"/\"; }, 1000);\n"
        "</script>\n"
        "</html>\r\n",
        eixo_x, eixo_y, posicao_bussola
    ); 
}

// Função de callback para processar requisições HTTP
static err_t http_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
    if(!p) {
        // Cliente fechou a conexão
        tcp_close(tpcb);
        return ERR_OK;
    }

    // Atualiza o conteúdo da página
    criar_resposta_http();

    // Envia a resposta HTTP
    tcp_write(tpcb, resposta_http, strlen(resposta_http), TCP_WRITE_FLAG_COPY);


    // Libera o buffer recebido
    pbuf_free(p);

    // Retorna que a operação foi bem-sucedida
    return ERR_OK;


}

// Callback de conexão: associa o http_callback à conexão
static err_t connection_callback(void *arg, struct tcp_pcb *newpcb, err_t err) {
    tcp_recv(newpcb, http_callback); // Associa o callback HTTP
    // Retorna que a operação foi bem-sucedida
    return ERR_OK;
}

// Função que interpreta a posição analógica do joystick como uma direção cardinal (rosa dos ventos)
void rosa_dos_ventos() {

    int zona_morta = 400;

    if(eixo_x > 3400 - zona_morta && eixo_y < 100 + zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Noroeste");
    }
    else if(eixo_x > 3500 - zona_morta && eixo_y > 3500 - zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Nordeste");
    }
    else if(eixo_x < 1200 + zona_morta && eixo_y < 50 + zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Sudoeste");
    } 
    else if(eixo_x < 50 + zona_morta && eixo_y > 3000 - zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Sudeste");
    }
    else if(eixo_x > 4060 - zona_morta && eixo_y < 1900 + zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Norte");
    }
    else if(eixo_x < 50 + zona_morta && eixo_y < 2000 + zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Sul");
    }
    else if(eixo_x > 1400 - zona_morta && eixo_y < 100 + zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Oeste");
    }
    else if(eixo_x > 1500 - zona_morta && eixo_y > 4000 - zona_morta) {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "Leste");
    }
    else {
        snprintf(posicao_bussola, sizeof(posicao_bussola), "");
    }
}


// Função para monitorar e ler os valores dos eixos x e y do joystick
void posicao_joystick() {

    adc_select_input(ADC_CHANNEL_0); // Seleciona o canal ADC para o eixo X
    sleep_ms(2); // Delay para estabilidade
    eixo_x = adc_read(); // Lê o eixo X (0 - 4095)

    adc_select_input(ADC_CHANNEL_1); // Seleciona o canal ADC para o eixo Y
    sleep_ms(2); // Delay para estabilidade
    eixo_y = adc_read(); // Lê o eixo Y (0 - 4095)
}



int main()
{
    stdio_init_all(); // inicializa o sistema de saída padrão
    setup_joystick(); // Configurações do joystick
    sleep_ms(10000);
    printf("Iniciando servidor HTTP\n");


    // Inicializa o Wi-Fi
    if(cyw43_arch_init()) {
        printf("Erro ao inicializar o wifi\n");
        return 1;
    }

    // Ativa o modo cliente para conectar a uma rede Wi-Fi existente
    cyw43_arch_enable_sta_mode();
    printf("Conectando ao Wi-Fi...\n");

    // Tenta conectar ao Wi-Fi com o SSID e senha fornecidos
    if(cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 20000)) {
        printf("Falha ao conectar ao Wi-Fi\n");
        sleep_ms(100);
        return -1;
    }

    // Se a conexão foi feita com sucesso, exibe o IP
    if(netif_default) {
        printf("IP do dispositivo: %s\n", ipaddr_ntoa(&netif_default->ip_addr));
    }

    // Cria uma nova estrutura para o servidor
    struct tcp_pcb *server = tcp_new();

    // Verifica se houve erro na criação da estrutura do servidor
    if(!server) {
        printf("Falha ao criar servidor TCP\n");
        return -1;
    }

    // Associa o servidor à porta 80
    if(tcp_bind(server, IP_ADDR_ANY, 80) != ERR_OK) {
        printf("Falha ao associar servidor a porta 80\n");
        return -1;
    }

    server = tcp_listen(server); // Coloca o servidor em modo "escuta"
    tcp_accept(server, connection_callback); // Define a função de callback para quando uma conexão for aceita

    printf("Servidor ouvindo na porta 80\n");


    // Loop principal 
    while (true) {
        cyw43_arch_poll(); // Necessário para manter o Wi-Fi ativo
        posicao_joystick(); // Monitoramento do joystick
        rosa_dos_ventos(); // Atualiza a direção correspondente com base na posição do joystick
        sleep_ms(100); // Reduz o uso da CPU
    }

    cyw43_arch_deinit(); // Desliga o Wi-Fi (não será chamado, pois o loop é infinito)
    return 0;

}
