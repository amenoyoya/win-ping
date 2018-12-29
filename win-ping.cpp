#include "getopt.hpp"
#include "ipv4.hpp"

/* oui.txtをダウンロード */
bool download_oui(bool confirm){
  char c = 0;
  
  if(confirm){ // ダウンロードするかどうか確認
    printf("Start downloading from http://standards.ieee.org/develop/regauth/oui/oui.txt ?\n(Y/n) > ");
    scanf("%c", &c);
  }
  if(!confirm || c == 'y' || c == 'Y'){
    puts("Info: Downloading oui.txt ...");
    if(!IPv4::downloadOUI("oui.txt")){
      puts("Error: oui.txt could not be downloaded");
      return false;
    }
    puts("Success: oui.txt was downloaded");
    return true;
  }else{
    puts("Cancel: oui.txt could not be downloaded");
    return false;
  }
}

/* ip1.ip2.ip3.start ～ ip1.ip2.ip3.end までping送信テスト */
bool ping(bool confirm, unsigned char ip1, unsigned char ip2, unsigned char ip3, unsigned char start, unsigned char end, unsigned int timeout){
  if(!IPv4::initialize()){
    puts("Error: WinINet could not be initialized");
    return false;
  }
  /* macアドレスとベンダー名の対応表を作成 */
  if(!IPv4::createVendorsTable("oui.txt")){
    puts("Error: oui.txt could not be found");
    if(!download_oui(confirm)){
      IPv4::finalize();
      puts("Cancel: oui.txt could not be downloaded");
      return false;
    }
    // 再試行
    IPv4::createVendorsTable("oui.txt");
  }
  /* ping送信 */
  IPv4 ipv4;
  if(!ipv4.open()){
    IPv4::finalize();
    puts("Error: IPv4 Interface could not be initialized");
    return false;
  }
  printf("IPv4 address will be searched for 192.168.0.%d to 192.168.0.%d (timeout: %d ms)\n", start, end, timeout);
  puts("--\n     IP Address     MAC Address     time     TTL     Vendor Name     Host Name\n--");
  for(unsigned char i = start; i <= end; ++i){
    unsigned char ip[4] = {ip1, ip2, ip3, i}, mac[6];
    string hostname;
    icmp_echo_reply reply;
    
    printf("%3i.%3i.%3i.%3i\r", ip1, ip2, ip3, i);
    if(ipv4.ping(ip, mac, &hostname, &reply)){
      printf("%3i.%3i.%3i.%3i  %02x:%02x:%02x:%02x:%02x:%02x  %4d  %4d  %s  %s\n",
        ip1, ip2, ip3, i, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5],
        reply.RoundTripTime, reply.Options.Ttl, IPv4::getVendorName(mac).c_str(), hostname.c_str());
    }else{
      printf("\r");
    }
  }
  IPv4::finalize();
  return true;
}

/* Usage表示 */
void show_usage(){
  puts("Usage: win-ping.exe [--help] [--version] [--update] [--yes] [--ip1=N] [--ip2=N] [--ip3=N] [--start=N] [--end=N] [--timeout=N] [-hvuy123set]\n"
    "  -h --help: Show this Usage\n"
    "  -v --version: Show version info\n"
    "  -u --update: Download oui.txt from http://standards.ieee.org/develop/regauth/oui/oui.txt\n"
    "  -y --yes: Always reply 'yes' when downloading oui.txt\n"
    "  -1N --ip1=N: Ping test IP address N.ip2.ip3.start - N.ip2.ip3.end (default=192)\n"
    "  -2N --ip2=N: Ping test IP address ip1.N.ip3.start - ip1.N.ip3.end (default=168)\n"
    "  -3N --ip3=N: Ping test IP address ip1.ip2.N.start - ip1.ip2.N.end (default=0)\n"
    "  -sN --start=N: Ping test IP address ip1.ip2.ip3.N - ip1.ip2.ip3.end (default=0)\n"
    "  -eN --end=N: Ping test IP address ip1.ip2.ip3.start - ip1.ip2.ip3.N (default=244)\n"
    "  -tN --timeout=N: Set timeout millseconds for ping (default=500)\n"
  );
}

/* コマンドラインオプション解析 */
struct flag_t{
  bool help, version, update, yes;
  unsigned char ip1, ip2, ip3, start, end;
  unsigned int timeout;
};

bool analyze_opt(int argc, char *argv[], flag_t *flag){
  optflag_t optflag = {0, "", 0, 0};
  optconf_t configs[] = {
    {'h', "help", false}, {'v', "version", false}, {'u', "update", false}, {'y', "yes", false},
    {'1', "ip1", true}, {'2', "ip2", true}, {'3', "ip3", true},
    {'s', "start", true}, {'e', "end", true}, {'t', "timeout", true},
  };
  
  for(char opt = getopt(argc, argv, configs, 10, &optflag); opt; opt = getopt(argc, argv, configs, 10, &optflag)){
    switch(opt){
    case 'h':
      flag->help = true;
      break;
    case 'v':
      flag->version = true;
      break;
    case 'u':
      flag->update = true;
      break;
    case 'y':
      flag->yes = true;
      break;
    case '1':
      flag->ip1 = strtol(optflag.arg.c_str(), nullptr, 10);
      break;
    case '2':
      flag->ip2 = strtol(optflag.arg.c_str(), nullptr, 10);
      break;
    case '3':
      flag->ip3 = strtol(optflag.arg.c_str(), nullptr, 10);
      break;
    case 's':
      flag->start = strtol(optflag.arg.c_str(), nullptr, 10);
      break;
    case 'e':
      flag->end = strtol(optflag.arg.c_str(), nullptr, 10);
      break;
    case 't':
      flag->timeout = strtol(optflag.arg.c_str(), nullptr, 10);
      break;
    case GETOPT_NONE:
      printf("Warning: Ignore '%s'\n", argv[optflag.index - 1]);
      break;
    case GETOPT_UNKNOWN:
      printf("Error: Unknown option '%s'\n", argv[optflag.index - 1]);
      return false;
    case GETOPT_ERROR:
      printf("Error: Incorrect option grammar '%s'\n", argv[optflag.index - 1]);
      return false;
    }
  }
  return true;
}

int main(int argc, char *argv[]){
  flag_t flag = {false, false, false, false, 192, 168, 0, 0, 244, 500};
  
  if(argc < 2 || !analyze_opt(argc - 1, &argv[1], &flag)){
    show_usage();
    return 1;
  }
  
  if(flag.help){ // Usage表示
    show_usage();
  }else{ // Usageを表示しない場合、メイン処理を行う
    if(flag.version) puts("win-ping ver.1.1.0 / Copyright (C) yoya(@amenoyoya) 2018"); // バージョン表示
    if(flag.update){ // oui.txtのダウンロードを行う
      if(!download_oui(!flag.yes)) return 1;
    }else{ // ping送信テスト実行
      if(!ping(!flag.yes, flag.ip1, flag.ip2, flag.ip3, flag.start, flag.end, flag.timeout)) return 1;
    }
  }
  return 0;
}