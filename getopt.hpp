#pragma once

#include <string>

using namespace std;

/* オプション名設定用構造体 */
struct optconf_t{
  char short_name;
  string long_name;
  bool require_arg;
};

/* 解析用フラグ管理構造体 */
struct optflag_t{
  enum{
    NONE, OPT, ARG
  };
  
  char opt;   // 前回解析時のshort-styleオプション名
  string arg; // 引数
  int next;   // 次のコマンドライン引数をどう解析するか
  int index;  // getopt関数呼び出しの度にインクリメントされる
};


/* Windows版getopt */
// 戻り値
enum{
  GETOPT_END = 0, GETOPT_NONE = -1, GETOPT_UNKNOWN = -2, GETOPT_ERROR = -3
};

// コマンドライン引数の配列を解析し、コマンドラインオプションを取得
// @argc: コマンドライン引数の数
// @argv[]: コマンドライン引数の配列
// @configs[]: 設定するオプション名の配列
// @flag: optflag_t構造体へのポインタを指定（引数をとるオプションの引数は flag.arg で取得可能）
//        最初の getopt 実行前に .next, .index を0クリアしておくこと
// @return: 対象のオプション名が存在するならshort-styleオプション名を返す
//          <Error code>
//          GETOPT_NONE: コマンドラインオプションではないコマンドライン引数
//          GETOPT_UNKNOWN: 設定されていないオプション名
//          GETOPT_ERROR: オプション指定の構文エラー
char getopt(int argc, char *argv[], optconf_t configs[], int num_configs, optflag_t *flag);
