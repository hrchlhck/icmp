#ifndef ICMP_H
#define ICMP_H
#define PAYLOAD_SIZE 49

typedef uint16_t;
typedef struct icmphdr;

void show_payload(char* payload);
char* create_payload();
uint16_t checksum(struct icmphdr* buf, int len);
struct icmphdr* create_echo_request(uint16_t id, uint16_t seq);
void show_icmp(struct icmphdr* p);
struct icmphdr* add_payload(struct icmphdr* p);

#endif // ICMP_H