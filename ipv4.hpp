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

/*** IPv4 ping送信クラス ***/
/* マックアドレス, ベンダー名, ホスト名取得 */
class IPv4{
public:
  IPv4(): m_handle(nullptr) {}
  ~IPv4(){
    close();
  }
  
  /* IPv4インターフェイス生成 */
  bool open(){
    close();
    return nullptr != (m_handle = IcmpCreateFile());
  }
  
  /* IPv4インターフェイス破棄 */
  void close(){
    if(m_handle){
      IcmpCloseHandle(m_handle);
      m_handle = nullptr;
    }
  }
  
  /* 指定IPアドレスにping送信 */
  // ※ このメソッドを実行する前に initialize() でWinSocketを開始しておく必要がある
  // @ip[4]: IPアドレス (例){192,168,0,1}
  // @mac[6]: <GET> macアドレス
  // @hostname: <GET> ホスト名
  // @reply: <GET>レスポンスの内容
  //         .RoundTripTime: ping送信してから応答までにかかった時間(ms)
  //         .Options.Ttl: 1単位のデータの有効生存時間(ms)
  // @timeout: タイムアウトの時間(ms)
  // @return: ping送信に応答があったかどうか
  bool ping(unsigned char ip[4], unsigned char mac[6], string *hostname, icmp_echo_reply *reply, unsigned int timeout=500);
  
  
  /* WinSocket開始 */
  static bool initialize(){
    WSADATA wsaData;
    return 0 == WSAStartup(MAKEWORD(2, 2), &wsaData);
  }
  
  /* WinSocket終了 */
  static void finalize(){
    WSACleanup();
  }
  
  /* IEEEからoui対応表ダウンロード */
  // oui: macアドレスの前半3バイト（Organizationally Unique Identifier）
  //      IEEEで管理されるベンダーID
  // @oui_txt: ダウンロード先のパス
  // @return: ダウンロードが成功したかどうか
  static bool downloadOUI(const string &oui_txt);
  
  /* macアドレスとベンダー名の対応表作成 */
  // @oui_txt: IEEEからダウンロードしたoui対応表のテキストファイルへのパス
  // @return: oui_txtを読み込めたかどうか
  static bool createVendorsTable(const string &oui_txt);
  
  /* macアドレスからベンダー名を取得 */
  // ※ このメソッドは先に createVendorsTable() を実行しておく必要がある
  // @mac[3～6]: macアドレス (例){0xE0, 0x43, 0xDB, ...}
  // @return: ベンダー名（macアドレスの一致するベンダーがなければ"Unknown"）
  static string getVendorName(unsigned char *mac);
private:
  HANDLE m_handle;
  static vector<string> s_vendors;
};