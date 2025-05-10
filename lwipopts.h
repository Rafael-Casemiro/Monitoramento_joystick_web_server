// Esse arquivo contém configurações específicas do lwIP(lightweight IP stack)
// uma pilha TCP/IP leve usada em sistemas embarcados
#ifndef LWIPOPTS_H
#define LWIPOPTS_H

// Configuração mínima para lwIP
#define NO_SYS 1 // Define que não está usando sistema operacional(bare-metal)

// Desativa as APIs de sockets e netconn - são usadas com RTOS
// Ganha desempenho e reduz consumo de memória, ideal para Pico W
#define LWIP_SOCKET 0 
#define LWIP_NETCONN 0

// Habilita suporte TCP e UDP para comunicação em rede
#define LWIP_TCP 1
#define LWIP_UDP 1

// Configuração de memória
#define MEM_ALIGNMENT 4 // Alinhamento de memória em 4 bytes(recomendado para ARM Cortex-M0+)
#define MEM_SIZE 4096 // Quantidade total de memória reservada para o lwIP(4 KB aqui)
#define MEMP_NUM_PBUF 16

// Tamanhos de buffers e estruturas internas
#define PBUF_POOL_SIZE 16               // Ajuste conforme necessário
#define MEMP_NUM_UDP_PCB 4
#define MEMP_NUM_TCP_PCB 4
#define MEMP_NUM_TCP_SEG 16

// Endereçamento e protocolos
#define LWIP_IPV4 1
#define LWIP_ICMP 1
#define LWIP_RAW 1
#define LWIP_DHCP 1
#define LWIP_AUTOIP 1
#define LWIP_DNS 1
#define LWIP_HTTPD 1
#define LWIP_HTTPD_SSI              1  // Habilita SSI
#define LWIP_HTTPD_SUPPORT_POST     1  // Habilita suporte a POST, se necessário
#define LWIP_HTTPD_DYNAMIC_HEADERS 1
#define HTTPD_USE_CUSTOM_FSDATA 0
#define LWIP_HTTPD_CGI 0           // Desative CGI para economizar memória
#define LWIP_NETIF_HOSTNAME 1


#endif /* LWIPOPTS_H */
