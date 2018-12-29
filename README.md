# win-ping
*WindowsでIPv4 ping送信テスト*

## 概要
WindowsでローカルIPアドレス（IPv4）にping送信テストをするプログラム

自宅の無線LAN環境で、NAS や Raspberry Pi が、どのIPアドレスに振り分けられているのか確認するために作成

## コンパイル
Visual Studio 2015 Community Edition でコンパイル確認済み

1. **Developer Command Prompt for VS2015** 起動
2. `call build.bat` 実行
3. binフォルダに実行ファイルが作成される
    - win-ping.exe: 本プログラム
    - getopt-test.exe: getoptライブラリのテスト用プログラム
    - ping-test.exe: ipv4ライブラリのテスト用プログラム

## 使用方法
`Usage: win-ping.exe [--help] [--version] [--update] [--yes] [--start=N] [--end=N] [--timeout=N] [-hvuyset]`

win-pingを実行すると、まず同一フォルダ内にある`oui.txt`を読み込んで、macアドレスとベンダー名の対応表を作成する

もし`oui.txt`がない場合は、 http://standards.ieee.org/develop/regauth/oui/oui.txt からダウンロードしようとするため、`(Y/n) > y`で許可する

その後、`192.168.0.0`～`192.168.0.244`のアドレスに対して、ping送信して、macアドレス、ベンダー名、ホスト名を取得する

## コマンドラインオプション
- -h --help: Usageを表示する
- -v --version: バージョン情報を表示する
- -u --update: oui.txt（macアドレスとベンダー名の対応表）を最新のものに更新する
- -y --yes: oui.txtをダウンロードする時、確認をスキップする
- -sN --start=N: Ping送信テストする時のIPアドレスの開始アドレスを `192.168.0.N` にする (default=0)
- -eN --end=N: Ping送信テストする時のIPアドレスの終了アドレスを `192.168.0.N` にする (default=244)
- -tN --timeout=N: Ping送信テストする時のタイムアウト時間(ミリ秒)を設定 (default=500)

### 使用例
```
> win-ping.exe -vuy
win-ping ver.1.0.2 / Copyright (C) yoya(@amenoyoya) 2018
Info: Downloading oui.txt ...
Success: oui.txt was downloaded

> win-ping -s0 --end 10 --timeout=200
IPv4 address will be searched for 192.168.0.0 to 192.168.0.10 (timeout: 200 ms)
--
    IP Address     MAC Address     time     TTL     Vendor Name     Host Name
--
192.168.  0.  1  34:76:c5:XX:XX:XX   829     3  I-O DATA DEVICE, INC.
192.168.  0.  2  34:76:c5:XX:XX:XX   829     3  I-O DATA DEVICE, INC.  DESKTOP-XXXXXX.airport
192.168.  0.  9  d8:49:2f:XX:XX:XX   829     3  CANON INC.             XXXXXXXXXX
192.168.  0. 10
```