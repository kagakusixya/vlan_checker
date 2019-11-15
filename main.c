#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <wiringPiI2C.h>
#include <unistd.h>

#define PROMISC_MODE 1
#define DPCP_NOLIMIT_LOOP  -1

int gpio(int vlan);
void display(char str,int fd );
void analyze( u_char *,const struct pcap_pkthdr * ,const u_char *);

struct	ether_header_q {
	u_char	ether_dhost[6];
	u_char	ether_shost[6];
  u_short ether_type_vlan;
  u_short vlan_info;
	u_short	ether_type;
};

int main(void){
  pcap_t *pd = NULL;
  char ebuf[PCAP_ERRBUF_SIZE];

  pd = pcap_open_live( "eth0",68,PROMISC_MODE,1000,ebuf );
  if(pd == NULL ){
  exit(-1);
  }


  if(pcap_loop(pd,DPCP_NOLIMIT_LOOP,analyze,NULL) < 0 ){
    // error
    exit(-1);
  }
  return 1;
}

void analyze(u_char *user,const struct pcap_pkthdr *h ,const u_char *p ){
  struct ether_header_q *ether_q;
  ether_q = (struct ether_header_q*)p;
	if (ntohs(ether_q->ether_type_vlan) != 0x8100) {
			printf("TYPE:%04x\n",ntohs(ether_q->ether_type_vlan));
		return;
	}
  printf("VLAN_TYPE:%04x\n",ntohs(ether_q->ether_type_vlan));
  printf("INFOMATION:%d\n",(ntohs(ether_q->vlan_info)));
	int x =  ntohs(ether_q->vlan_info);
	int y[16];
	int sum = 0;
	int z = 1;

	memset(&y,0,sizeof(y));
	for (int i = 0; i < 16; i++) {
		y[i] = x%2;
		x = x/2;
	}
	for (int i = 0; i < 13; i++) {
		sum= (y[i]*z)+sum;
		z = 2*z;
	}
	printf("VLANID:%d\n",sum);

  printf("TYPE:%04x\n",ntohs(ether_q->ether_type));
  gpio(sum);

  printf("\n-----------------------------------------------\n");
  sleep(1);
}


int gpio(int vlan){
  int fd;
  char str[8];
  char title[8] = "VLANID: ";

  fd = wiringPiI2CSetup(0x3e);

  wiringPiI2CWriteReg8(fd,0x00,0x38);
  wiringPiI2CWriteReg8(fd,0x00,0x39);
  wiringPiI2CWriteReg8(fd,0x00,0x14);
  wiringPiI2CWriteReg8(fd,0x00,0x70);
  wiringPiI2CWriteReg8(fd,0x00,0x56);
  wiringPiI2CWriteReg8(fd,0x00,0x6c);

  wiringPiI2CWriteReg8(fd,0x00,0x38);
  wiringPiI2CWriteReg8(fd,0x00,0x0d);
  wiringPiI2CWriteReg8(fd,0x00,0x01);

  memset(&str,' ',sizeof(str));
  sprintf(str,"%4d",vlan);
  wiringPiI2CWriteReg8(fd,0x00,0x38);
  for (int i = 0; i < sizeof(title); i++) {
    display(title[i],fd);
  }
  wiringPiI2CWriteReg8(fd,0x00,0xc0);
  for (int i = 0; i < sizeof(str); i++) {
    display(str[i],fd);
  }
  sleep(1);
  return 0;
}


void display(char str,int fd ){
  wiringPiI2CWriteReg8(fd,0x40,str);
}
