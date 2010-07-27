#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char *argv[]) {

  struct ifaddrs *ifaddr, *ifa;
  int family, s;
  char host[NI_MAXHOST], broad[NI_MAXHOST], mask[NI_MAXHOST];
  
  if (getifaddrs(&ifaddr) == -1) {
    perror("getifaddrs");
    exit(EXIT_FAILURE);
  }
  
  for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
    if (ifa->ifa_addr) {

      family = ifa->ifa_addr->sa_family;
      
      if (family == AF_INET || family == AF_INET6) {
	
	printf("%s ", ifa->ifa_name);
	
	s = getnameinfo(ifa->ifa_addr,
			(family == AF_INET) ? sizeof(struct sockaddr_in) :
			sizeof(struct sockaddr_in6),
			host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
	if (s != 0) {
	  fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
	  exit(EXIT_FAILURE);
	}
	printf("%s ", host);
	
	s = getnameinfo(ifa->ifa_netmask, (family == AF_INET) ?
			sizeof(struct sockaddr_in) :
			sizeof(struct sockaddr_in6), mask, NI_MAXHOST,
			NULL, 0, NI_NUMERICHOST);
	if (s != 0) {
	  fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
	  exit(EXIT_FAILURE);
	}
	printf("%s ", mask);
	
	if (family == AF_INET && ifa->ifa_flags & IFF_BROADCAST) {
	  s = getnameinfo(ifa->ifa_broadaddr, (family == AF_INET) ?
			  sizeof(struct sockaddr_in) :
			  sizeof(struct sockaddr_in6), broad, NI_MAXHOST,
			NULL, 0, NI_NUMERICHOST);
	  if (s != 0) {
	    fprintf(stderr, "getnameinfo() failed: %s\n", gai_strerror(s));
	    exit(EXIT_FAILURE);
	  }
	  printf("%s\n", broad);	
	} else 
	  printf("X\n");	
      }
    }
  }
  
  freeifaddrs(ifaddr);
  exit(EXIT_SUCCESS);
}
