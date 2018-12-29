#pragma once

#include <string>
#include <vector>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <wininet.h>

#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wininet.lib")

using namespace std;

/*** IPv4 ping���M�N���X ***/
/* �}�b�N�A�h���X, �x���_�[��, �z�X�g���擾 */
class IPv4{
public:
  IPv4(): m_handle(nullptr) {}
  ~IPv4(){
    close();
  }
  
  /* IPv4�C���^�[�t�F�C�X���� */
  bool open(){
    close();
    return nullptr != (m_handle = IcmpCreateFile());
  }
  
  /* IPv4�C���^�[�t�F�C�X�j�� */
  void close(){
    if(m_handle){
      IcmpCloseHandle(m_handle);
      m_handle = nullptr;
    }
  }
  
  /* �w��IP�A�h���X��ping���M */
  // �� ���̃��\�b�h�����s����O�� initialize() ��WinSocket���J�n���Ă����K�v������
  // @ip[4]: IP�A�h���X (��){192,168,0,1}
  // @mac[6]: <GET> mac�A�h���X
  // @hostname: <GET> �z�X�g��
  // @reply: <GET>���X�|���X�̓��e
  //         .RoundTripTime: ping���M���Ă��牞���܂łɂ�����������(ms)
  //         .Options.Ttl: 1�P�ʂ̃f�[�^�̗L����������(ms)
  // @timeout: �^�C���A�E�g�̎���(ms)
  // @return: ping���M�ɉ��������������ǂ���
  bool ping(unsigned char ip[4], unsigned char mac[6], string *hostname, icmp_echo_reply *reply, unsigned int timeout=500);
  
  
  /* WinSocket�J�n */
  static bool initialize(){
    WSADATA wsaData;
    return 0 == WSAStartup(MAKEWORD(2, 2), &wsaData);
  }
  
  /* WinSocket�I�� */
  static void finalize(){
    WSACleanup();
  }
  
  /* IEEE����oui�Ή��\�_�E�����[�h */
  // oui: mac�A�h���X�̑O��3�o�C�g�iOrganizationally Unique Identifier�j
  //      IEEE�ŊǗ������x���_�[ID
  // @oui_txt: �_�E�����[�h��̃p�X
  // @return: �_�E�����[�h�������������ǂ���
  static bool downloadOUI(const string &oui_txt);
  
  /* mac�A�h���X�ƃx���_�[���̑Ή��\�쐬 */
  // @oui_txt: IEEE����_�E�����[�h����oui�Ή��\�̃e�L�X�g�t�@�C���ւ̃p�X
  // @return: oui_txt��ǂݍ��߂����ǂ���
  static bool createVendorsTable(const string &oui_txt);
  
  /* mac�A�h���X����x���_�[�����擾 */
  // �� ���̃��\�b�h�͐�� createVendorsTable() �����s���Ă����K�v������
  // @mac[3�`6]: mac�A�h���X (��){0xE0, 0x43, 0xDB, ...}
  // @return: �x���_�[���imac�A�h���X�̈�v����x���_�[���Ȃ����"Unknown"�j
  static string getVendorName(unsigned char *mac);
private:
  HANDLE m_handle;
  static vector<string> s_vendors;
};