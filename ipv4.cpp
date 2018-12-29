#include "ipv4.hpp"

/*** class IPv4 ***/
vector<string> IPv4::s_vendors;

bool IPv4::ping(unsigned char ip[4], unsigned char mac[6], string *hostname, icmp_echo_reply *reply, unsigned int timeout){
  bool res;
  char data[] = {'a'}, cReply[64]; // ���M�f�[�^, ��M�f�[�^
  IPAddr ipaddr; // IP�A�h���X�f�[�^
  
  memcpy(&ipaddr, ip, sizeof(ip));
  if(res = (0 != IcmpSendEcho(m_handle, ipaddr, (void*)data, 1, nullptr, cReply, 64, timeout))){
    struct hostent* hostp;
    unsigned long len = 6;
    
    reply = (PICMP_ECHO_REPLY)cReply; // ��M�f�[�^�擾
    if(res = (NO_ERROR == SendARP(ipaddr, (IPAddr)nullptr, mac, &len))) // IP�A�h���X����mac�A�h���X���擾
    if(nullptr != (hostp = gethostbyaddr((const char*)ip, 4, AF_INET))) // IP�A�h���X����z�X�g�����擾
      *hostname = hostp->h_name;
  }
  return res;
}

bool IPv4::downloadOUI(const string &oui_txt){
  // ���W�X�g���ɕێ�����Ă���ݒ�𗘗p����WININET������
  HINTERNET hUrl, hInet = InternetOpen("oui-downloader", INTERNET_OPEN_TYPE_PRECONFIG, nullptr, nullptr, 0);
  
  if(hInet == nullptr) return false;
  // IEEE��oui.txt�ւ�URL�w��
  if(nullptr != (hUrl = InternetOpenUrl(hInet, "http://standards.ieee.org/develop/regauth/oui/oui.txt", nullptr, 0, 0, 0))){
    char buf[1024];
    unsigned long readed = 0;
    // �ǂݏo�������Ȃ��Ȃ�܂œǂݏo���{�t�@�C����������
    for(FILE *fp = fopen(oui_txt.c_str(), "wb"); fp; fwrite(buf,1, readed, fp)){
      InternetReadFile(hUrl, buf, sizeof(buf), &readed);
      // �ǂݏo�������Ȃ��Ȃ�����I��
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
    // �ȉ��̌`���ŋL�q���ꂽ�s����Amac�A�h���X�ƃx���_�[���̑Ή��\���쐬
    // XX-XX-XX   (hex)   Vendor Name
    if(isxdigit(*top) && *(top + 2) == '-'){
      s_vendors.push_back("");
      string &vendor = *(s_vendors.end() - 1);
      vendor.resize(512);
      // �擪3�o�C�g��mac�A�h���X�i�[
      vendor[0] = (unsigned char)strtol(top, nullptr, 16);
      vendor[1] = (unsigned char)strtol(top+3, nullptr, 16);
      vendor[2] = (unsigned char)strtol(top+6, nullptr, 16);
      // �󔒂�"(hex)"���X�L�b�v
      for(top += 8; *top == ' '; ++top);
      for(; *top && *top != ')'; ++top);
      for(top++; *top == '\t'; ++top);
      // �c��̕������x���_�[���i���s�����͔����j
      for(char *p = top; *p; ++p) if(*p == '\r' || *p == '\n') *p = '\0';
      strcpy((char*)vendor.c_str() + 3, top);
    }
  }
  return true;
}

string IPv4::getVendorName(unsigned char *mac){
  for(auto it = s_vendors.begin(), end = s_vendors.end(); it != end; ++it){
    unsigned char *p = (unsigned char*)it->c_str();
    // ��3�o�C�g��mac�A�h���X�Ɠ��������̂�����
    if(*p == mac[0] && *(p + 1) == mac[1] && *(p + 2) == mac[2]) return (char*)(p + 3);
  }
  return "Unknown";
}
