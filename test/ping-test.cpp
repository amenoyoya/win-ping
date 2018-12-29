#include "../ipv4.hpp"

int main(){
  if(!IPv4::initialize()) return puts("WinINet could not be initialized");
  if(!IPv4::createVendorsTable("oui.txt")){
    puts("oui.txt could not be found\nStart downloading from http://standards.ieee.org/develop/regauth/oui/oui.txt");
    if(!IPv4::downloadOUI("oui.txt")){
      IPv4::finalize();
      return puts("oui.txt could not be downloaded");
    }
  }
  
  IPv4 ipv4;
  if(!ipv4.open()){
    IPv4::finalize();
    return puts("IPv4 Interface could not be initialized");
  }
  puts("IPv4 address will be searched for 192.168.0.0 to 192.168.0.244");
  puts("--\n     IP Address     MAC Address     time     TTL     Vendor Name     Host Name\n--");
  for(unsigned char i = 0; i < 245; ++i){
    unsigned char ip[4] = {192, 168, 0, i}, mac[6];
    string hostname;
    icmp_echo_reply reply;
    
    printf("%3i.%3i.%3i.%3i\r", 192, 168, 0, i);
    if(ipv4.ping(ip, mac, &hostname, &reply)){
      printf("%3i.%3i.%3i.%3i  %02x:%02x:%02x:%02x:%02x:%02x  %4d  %4d  %s  %s\n",
        192, 168, 0, i, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
        reply.RoundTripTime, reply.Options.Ttl, IPv4::getVendorName(mac).c_str(), hostname.c_str());
    }else{
      printf("\r");
    }
  }
  IPv4::finalize();
  return 0;
}
