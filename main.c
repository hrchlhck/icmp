#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"
#include "icmp.h"

typedef struct packet_t {
    struct iphdr* ip;
    struct icmphdr* icmp;
} packet_t;


struct sockaddr_in create_addr(const char* addr, uint16_t port) {
    struct sockaddr_in ret;
    memset(&ret, 0, sizeof(struct sockaddr_in));

    ret.sin_family = AF_INET;
    ret.sin_port = htons(port);
    ret.sin_addr.s_addr = inet_addr(addr);

    return ret;
}

struct icmphdr* extract_icmp(unsigned char* buffer) {
    return (struct icmphdr*)(buffer + 20);
}

int main(int argc, char const *argv[]) {

    int rc = check_args(argc, argv);

    if (rc != 0) {
        perror("Argument error");
        return rc;
    }

    args_t* args = create_args(argv[1], argv[2]);

    int sockfd;

    // Atribuíndo porta 0, pois não estamos trabalhando na camada de transporte e o conceito de porta não existe
    struct sockaddr_in saddr = create_addr(args->src_addr, 0);
    struct sockaddr_in daddr = create_addr(args->dst_addr, 0);

    struct sockaddr* s_saddr = (struct sockaddr*)&saddr;
    struct sockaddr* s_daddr = (struct sockaddr*)&daddr;

    socklen_t addrlen = sizeof(struct sockaddr);

    // Criando socket RAW permitindo reconstruir o cabeçalho ICMP (IPPROTO_ICMP)
    if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1) {
        perror("Error creating socket\n");
        return 1;
    }
    
    // Associo o socket ao endereço s_saddr
    if(bind(sockfd, s_saddr, sizeof(saddr)) == -1) {
        perror("Error binding to address");
        return 1;
    }

    int counter = 1;

    for(;;) {
        // Cria cabeçalho ICMP com o ID sendo o PID do processo
        struct icmphdr* p = create_echo_request(getpid(), counter);
        p = add_payload(p);
        show_icmp(p);
    
        int data;
        
        // Tamanho do pacote é 56 bytes (8 bytes do cabeçalho + 48 bytes de payload)
        if((data = sendto(sockfd, p, 56, 0, s_daddr, addrlen)) == -1) {
            perror("Error sending data");
            return 1;
        }
    
        size_t bufflen = 76;
        unsigned char* buff = (unsigned char*) malloc(sizeof(unsigned char) * bufflen);
    
        int recvdata = recvfrom(sockfd, buff, bufflen, 0, s_daddr, &addrlen);
        if (recvdata == -1) {
            perror("Error receiving data");
            return 1;
        }
        
        // Ignora cabeçalho IP
        struct icmphdr* reply = extract_icmp(buff);
        
        // Filtra pacotes recebidos do kernel
        if(reply->type == ICMP_ECHO && reply->checksum == p->checksum) continue;

        show_icmp(reply);

        counter++;

        printf("Pressione ENTER para enviar outro pacote ou digite 0 para sair.\n");
        int c = getchar();

        if (c == '0') {
            break;
        }
    }
    return 0;
}
