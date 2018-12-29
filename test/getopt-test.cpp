#include "../getopt.hpp"

int print_usage(){
  return printf(
    "Usage: getopt [options]\n"
    "\t-h --help: Show usage\n"
    "\t-v --version: Show version\n"
    "\t-H --head \"string\": Set string to 'Head'. This program will output \"'Head'+'Tail'\"\n"
    "\t-T --tail \"string\": Set string to 'Tail'. This program will output \"'Head'+'Tail'\"\n"
  );
}

int main(int argc, char *argv[]){
  if(argc > 1){
    optflag_t flag = {0, "", 0, 0};
    optconf_t configs[] = {
      {'h', "help", false},
      {'v', "version", false},
      {'H', "head", true},
      {'T', "tail", true},
    };
    bool help = false, version = false;
    string head, tail;
    
    for(char opt = getopt(argc - 1, &argv[1], configs, 4, &flag); opt; opt = getopt(argc - 1, &argv[1], configs, 4, &flag)){
      switch(opt){
      case 'h':
        help = true;
        break;
      case 'v':
        version = true;
        break;
      case 'H':
        head = flag.arg;
        break;
      case 'T':
        tail = flag.arg;
        break;
      case GETOPT_UNKNOWN:
        printf("CommandLine option: Unknown option error in '%s'\n", argv[flag.index]);
        return print_usage();
      case GETOPT_ERROR:
        printf("CommandLine option: Grammer error in '%s'\n", argv[flag.index]);
        return print_usage();
      }
    }
    
    if(help){
      return print_usage();
    }else{
      if(version) printf("getopt - v.1.1.3 / Copyright (C) yoya 2018.\n");
      if(head != "" || tail != "") printf("%s %s\n", head.c_str(), tail.c_str());
    }
  }else{
    print_usage();
  }
  return 0;
}
