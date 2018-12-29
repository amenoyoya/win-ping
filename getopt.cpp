#include "getopt.hpp"

/* 指定文字が設定されたオプション名に合致するか */
// @configs[]: 設定するオプション名の配列
// @num_configs: オプション名配列の要素数
// @name: 検索対象の文字
// @return: 対象のオプション名が存在するなら配列のindexを返す（存在しないオプション名なら-1を返す）
static int search_optconf(optconf_t configs[], int num_configs, char name){
  for(int i = 0; i < num_configs; ++i){
    if(configs[i].short_name == name) return i;
  }
  return -1;
}

/* 指定文字列が設定されたオプション名に合致するか */
// @configs[]: 設定するオプション名の配列
// @num_configs: オプション名配列の要素数
// @name: 検索対象の文字列
// @return: 対象のオプション名が存在するなら配列のindexを返す（存在しないオプション名なら-1を返す）
static int search_optconf(optconf_t configs[], int num_configs, const string &name){
  for(int i = 0; i < num_configs; ++i){
    if(configs[i].long_name == name) return i;
  }
  return -1;
}

/* 指定したコマンドライン引数を解析し、設定されたオプション名配列のindexを返す */
// @argv: 解析対象のコマンドライン引数
// @configs[]: 設定するオプション名の配列
// @num_configs: オプション名配列の要素数
// @flag: optflag_t構造体へのポインタを指定（引数をとるオプションの引数は flag.arg で取得可能）
// @return: 対象のオプション名が存在するなら配列のindexを返す
//          <Error code>
//          -1: コマンドラインオプションではないコマンドライン引数
//          -2: 設定されていないオプション名
//          -3: 引数不要オプションに対し、引数が設定された
static int judge_opt(const string &argv, optconf_t configs[], int num_configs, optflag_t *flag){
  char *p = (char*)argv.c_str();
  
  if(*p == '-'){
    if(*(++p) == '-'){ // long-style option
      char *top = ++p;
      // "--option=argment"形式か確認するため'='を検索
      for(; *p != '='; ++p){
        if(*p == 0){ // '='がなかった場合
          int index = search_optconf(configs, num_configs, top); // long-style option 検索
          
          if(index == -1) return -2; // Unknown long-style option Error
          
          if(configs[index].require_arg){ // 引数をとるオプションの場合、次のコマンドライン引数を引数とする
            flag->next = optflag_t::ARG;
          }else{ // 引数不要オプションの場合、解析完了
            flag->next = optflag_t::NONE;
          }
          return index;
        }
      }
      // '='を'\0'にして、オプション名と引数を分ける
      *(p++) = 0;
      int index = search_optconf(configs, num_configs, top); // long-style option 検索
      
      if(index == -1) return -2; // Unknown long-style option Error
      
      if(configs[index].require_arg){ // 引数をとるオプションの場合、'='以降の文字列を引数とする
        // if(*p == 0) // 空引数を許容しない場合は条件分岐
        flag->arg = p; // 空引数も許容する
        flag->next = optflag_t::NONE;
        return index;
      }else{ // 引数不要オプションの場合、構文エラー
        return -3; // Grammar Error
      }
    }else{ // short-style option
      int index = search_optconf(configs, num_configs, *p); // short-style option 検索
      
      if(index == -1) return -2; // Unknown short-style option Error
      
      if(configs[index].require_arg){ // 引数をとるオプションの場合
        if(*(++p) == 0){ // これ以上文字がないなら次のコマンドライン引数を引数とする
          flag->next = optflag_t::ARG;
        }else{ // 残りの文字列を引数とする
          flag->arg = p;
          flag->next = optflag_t::NONE;
        }
      }else{ // 引数不要オプションの場合
        if(*(++p) == 0){ // 残りの文字列がなければ、そのまま解析終了
          flag->next = optflag_t::NONE;
        }else{ // 残りの文字列がまだあれば、short-styleオプションが続いていると仮定
          // "-abc" => "-a", "-bc"
          flag->next = optflag_t::OPT;
          flag->arg = string("-") + p; // 次の解析では flag.arg を解析対象とする
        }
      }
      return index;
    }
  }
  return -1;
}

/* Windows版getopt */
char getopt(int argc, char *argv[], optconf_t configs[], int num_configs, optflag_t *flag){
  int index = -1;
  
  // short-styleコマンドが続いておらず、indexがオプション配列の範囲外に行ったら解析終了
  if(argc <= flag->index && flag->next != optflag_t::OPT){
    return flag->next == optflag_t::ARG? GETOPT_ERROR: GETOPT_END; 
  }
  // 解析
  switch(flag->next){
  case optflag_t::ARG: // 現在のコマンドライン引数を前のコマンドラインオプションの引数とする
    flag->next = optflag_t::NONE; // 次のコマンドライン引数は通常解析する
    flag->arg = argv[flag->index++];
    return flag->opt; // 前のオプション名を返す
  case optflag_t::OPT: // flag.arg を解析対象とする
    flag->next = optflag_t::NONE; // 次のコマンドライン引数は通常解析する
    index = judge_opt(flag->arg, configs, num_configs, flag);
    if(index < 0) return index; // オプションではない or エラー
    flag->opt = configs[index].short_name;
    // 次のコマンドライン引数をオプション引数としないなら、取得したオプション名を返す
    if(flag->next != optflag_t::ARG) return flag->opt;
    break;
  default: // 通常解析
    index = judge_opt(argv[flag->index++], configs, num_configs, flag);
    if(index < 0) return index; // オプションではない or エラー
    flag->opt = configs[index].short_name;
    // 次のコマンドライン引数をオプション引数としないなら、取得したオプション名を返す
    if(flag->next != optflag_t::ARG) return flag->opt;
    break;
  }
  return getopt(argc, argv, configs, num_configs, flag); // 次のコマンドライン引数を処理
}
