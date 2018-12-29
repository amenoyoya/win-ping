#include "ipv4.hpp"

/*** class IPv4 ***/
vector<string> IPv4::s_vendors;

bool IPv4::ping(unsigned char ip[4], unsigned char mac[6], string *hostname, icmp_echo_reply *reply, unsigned int timeout){
  bool res;
  char data[] = {'a'}, cReply[64]; // 送信データ, 受信データ
  IPAddr ipaddr; // IPアドレスデータ
  
  memcpy(&ipaddr, ip, sizeof(ip));
  if(res = (0 != IcmpSendEcho(m_handle, ipaddr, (void*)data, 1, nullptr, cReply, 64, timeout))){
    struct hostent* hostp;
    unsigned long len = 6;
    
    reply = (PICMP_ECHO_REPLY)cReply; // 受信データ取得
    if(res = (NO_ERROR == SendARP(ipaddr, (IPAddr)nullptr, mac, &len))) // IPアドレスからmacアドレスを取得
    if(nullptr != (hostp = gethostbyaddr((const char*)ip, 4, AF_INET))) // IPアドレスからホスト名を取得
      *hostname = hostp->h_name;
  }
  return res;
}

bool IPv4::downloadOUI(const string &oui_txt){
  // レジストリに保持されている設定を利用してWININET初期化
  HINTERNET hUrl, hInet = InternetOpen("oui-downloader", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
  
  if(hInet == nullptr) return false;
  // IEEEのoui.txtへのURL指定
  if(nullptr != (hUrl = InternetOpenUrl(hInet, "http://standards.ieee.org/develop/regauth/oui/oui.txt", nullptr, 0, 0, 0))){
    char buf[1024];
    unsigned long readed = 0;
    // 読み出す物がなくなるまで読み出し＋ファイル書き込み
    for(FILE *fp = fopen(oui_txt.c_str(), "wb"); fp; fwrite(buf,1, readed, fp)){
      InternetReadFile(hUrl, buf, sizeof(buf), &readed);
      // 読み出す物がなくなったら終了
      if(readed == 0){
        fclose(fp);
        InternetCloseHandle(hUrl);
        InternetCloseHandle(hInet);
        return true;
      }
    }
    InternetCloseHandle(hUrl);
  }
  InternetCloseHandle(hInet);
  return false;
}

bool IPv4::createVendorsTable(const string &oui_txt){
  char buf[512];
  FILE *fp = fopen(oui_txt.c_str(), "r");
  
  if(nullptr == fp) return false;
  while(fgets(buf, 512, fp)){
    char *top = buf;
    // 以下の形式で記述された行から、macアドレスとベンダー名の対応表を作成
    // XX-XX-XX   (hex)   Vendor Name
    if(isxdigit(*top) && *(top + 2) == '-'){
      s_vendors.push_back("");
      string &vendor = *(s_vendors.end() - 1);
      vendor.resize(512);
      // 先頭3バイトにmacアドレス格納
      vendor[0] = (unsigned char)strtol(top, nullptr, 16);
      vendor[1] = (unsigned char)strtol(top+3, nullptr, 16);
      vendor[2] = (unsigned char)strtol(top+6, nullptr, 16);
      // 空白と"(hex)"をスキップ
      for(top += 8; *top == ' '; ++top);
      for(; *top && *top != ')'; ++top);
      for(top++; *top == '\t'; ++top);
      // 残りの部分がベンダー名（改行文字は抜く）
      for(char *p = top; *p; ++p) if(*p == '\r' || *p == '\n') *p = '\0';
      strcpy((char*)vendor.c_str() + 3, top);
    }
  }
  return true;
}

string IPv4::getVendorName(unsigned char *mac){
  for(auto it = s_vendors.begin(), end = s_vendors.end(); it != end; ++it){
    unsigned char *p = (unsigned char*)it->c_str();
    // 頭3バイトがmacアドレスと等しいものを検索
    if(*p == mac[0] && *(p + 1) == mac[1] && *(p + 2) == mac[2]) return (char*)(p + 3);
  }
  return "Unknown";
}
