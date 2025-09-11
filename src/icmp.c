#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <stdint.h>
#include <arpa/inet.h>


#include "icmp.h"

// Mostra os bytes do payload na representação hexadecimal
void show_payload(char* payload) {
    for (int i = 0; i < strlen(payload); i++) {
        
        if (i % 16 == 0 && i != 0) {
            printf("\n");
        }

        printf("%02X ", (unsigned char)payload[i]);
    }
    printf("\n");
}

// Cria payload 
char* create_payload() {
    char* payload = (char*) malloc(sizeof(char) * PAYLOAD_SIZE);

    char start = '\x08';
    for(int i = start; i < PAYLOAD_SIZE + start; ++i) {
        payload[i - start] = (char) i;
    }

    payload[PAYLOAD_SIZE - 1] = '\0';

    return payload;
}

// Checksum IP (RFC 1071)
/*
uint16_t checksum(struct icmphdr* buf, int len) {
    uint32_t sum = 0;
    uint16_t* pacote = (uint16_t *) buf;

    while(len > 1) {
        sum += *pacote++;
        len -= 2;
    }

    if (len > 0) {
        sum += *((char*)pacote);
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}
*/
uint16_t checksum(struct icmphdr* buf, int len) {

    // fazendo um ponteiro nao mutavel e transforma em &[u8]
    const uint8_t* pacote = (const uint8_t*)buf;
    uint32_t sum = 0;
    int i;  

    for (i = 0; i < len - 1; i += 2) {
//                     bitwise shift left
//                         0xA8 vira 0xA800
//                0xA800 | 0xA dai vira 0xA8A9
        uint16_t word = (pacote[i] << 8) | pacote[i+1];
        sum += word;
    }

    if (len % 2 == 1) {
        sum += pacote[len - 1];
    }

    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    return (uint16_t)(~sum);
}

// Cria estrutura base para Echo request
struct icmphdr* create_echo_request(uint16_t id, uint16_t seq) {
    struct icmphdr* echo = malloc(sizeof(struct icmphdr));

    echo->type = ICMP_ECHO;
    echo->code = 0;
    echo->checksum = 0;
    echo->un.echo.id = htons(id);
    echo->un.echo.sequence = htons(seq);
    
    return echo;
}

// Mostra o cabeçalho ICMP
void show_icmp(struct icmphdr* p) {
    int id = ntohs(p->un.echo.id);
    int seq = ntohs(p->un.echo.sequence);
    printf("Type: %i\tCode: %i\tChecksum: 0x%x\tID: %i\tSeq: %i\n", p->type, p->code, ntohs(p->checksum), ntohs(id), ntohs(seq));
    char* payload = (char*)p + sizeof(struct icmphdr);

    printf("Payload:\n");
    show_payload(payload);
    printf("\n");
}

// Adiciona payload ao cabeçalho
struct icmphdr* add_payload(struct icmphdr* p) {
    char* payload = create_payload();

    size_t header_size = sizeof(struct icmphdr);
    size_t payload_size = PAYLOAD_SIZE;

    // Cria buffer para armazenar o cabeçalho + payload
    char* new_packet = (char*) malloc(sizeof(char) * (header_size + payload_size));

    memcpy(new_packet, p, header_size);

    // Copia os dados do payload depois (+ header_size) do cabeçalho ICMP
    memcpy(new_packet + header_size, payload, payload_size);

    struct icmphdr* ret_pkt = (struct icmphdr*) new_packet;
    ret_pkt->checksum = 0;
    ret_pkt->checksum = htons(checksum(ret_pkt, header_size + payload_size));
    //ret_pkt->checksum = checksum(ret_pkt, header_size + payload_size);
    //ret_pkt->checksum = htons(0x1234);
    return ret_pkt;
}