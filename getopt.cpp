#include "getopt.hpp"

/* �w�蕶�����ݒ肳�ꂽ�I�v�V�������ɍ��v���邩 */
// @configs[]: �ݒ肷��I�v�V�������̔z��
// @num_configs: �I�v�V�������z��̗v�f��
// @name: �����Ώۂ̕���
// @return: �Ώۂ̃I�v�V�����������݂���Ȃ�z���index��Ԃ��i���݂��Ȃ��I�v�V�������Ȃ�-1��Ԃ��j
static int search_optconf(optconf_t configs[], int num_configs, char name){
  for(int i = 0; i < num_configs; ++i){
    if(configs[i].short_name == name) return i;
  }
  return -1;
}

/* �w�蕶���񂪐ݒ肳�ꂽ�I�v�V�������ɍ��v���邩 */
// @configs[]: �ݒ肷��I�v�V�������̔z��
// @num_configs: �I�v�V�������z��̗v�f��
// @name: �����Ώۂ̕�����
// @return: �Ώۂ̃I�v�V�����������݂���Ȃ�z���index��Ԃ��i���݂��Ȃ��I�v�V�������Ȃ�-1��Ԃ��j
static int search_optconf(optconf_t configs[], int num_configs, const string &name){
  for(int i = 0; i < num_configs; ++i){
    if(configs[i].long_name == name) return i;
  }
  return -1;
}

/* �w�肵���R�}���h���C����������͂��A�ݒ肳�ꂽ�I�v�V�������z���index��Ԃ� */
// @argv: ��͑Ώۂ̃R�}���h���C������
// @configs[]: �ݒ肷��I�v�V�������̔z��
// @num_configs: �I�v�V�������z��̗v�f��
// @flag: optflag_t�\���̂ւ̃|�C���^���w��i�������Ƃ�I�v�V�����̈����� flag.arg �Ŏ擾�\�j
// @return: �Ώۂ̃I�v�V�����������݂���Ȃ�z���index��Ԃ�
//          <Error code>
//          -1: �R�}���h���C���I�v�V�����ł͂Ȃ��R�}���h���C������
//          -2: �ݒ肳��Ă��Ȃ��I�v�V������
//          -3: �����s�v�I�v�V�����ɑ΂��A�������ݒ肳�ꂽ
static int judge_opt(const string &argv, optconf_t configs[], int num_configs, optflag_t *flag){
  char *p = (char*)argv.c_str();
  
  if(*p == '-'){
    if(*(++p) == '-'){ // long-style option
      char *top = ++p;
      // "--option=argment"�`�����m�F���邽��'='������
      for(; *p != '='; ++p){
        if(*p == 0){ // '='���Ȃ������ꍇ
          int index = search_optconf(configs, num_configs, top); // long-style option ����
          
          if(index == -1) return -2; // Unknown long-style option Error
          
          if(configs[index].require_arg){ // �������Ƃ�I�v�V�����̏ꍇ�A���̃R�}���h���C�������������Ƃ���
            flag->next = optflag_t::ARG;
          }else{ // �����s�v�I�v�V�����̏ꍇ�A��͊���
            flag->next = optflag_t::NONE;
          }
          return index;
        }
      }
      // '='��'\0'�ɂ��āA�I�v�V�������ƈ����𕪂���
      *(p++) = 0;
      int index = search_optconf(configs, num_configs, top); // long-style option ����
      
      if(index == -1) return -2; // Unknown long-style option Error
      
      if(configs[index].require_arg){ // �������Ƃ�I�v�V�����̏ꍇ�A'='�ȍ~�̕�����������Ƃ���
        // if(*p == 0) // ����������e���Ȃ��ꍇ�͏�������
        flag->arg = p; // ����������e����
        flag->next = optflag_t::NONE;
        return index;
      }else{ // �����s�v�I�v�V�����̏ꍇ�A�\���G���[
        return -3; // Grammar Error
      }
    }else{ // short-style option
      int index = search_optconf(configs, num_configs, *p); // short-style option ����
      
      if(index == -1) return -2; // Unknown short-style option Error
      
      if(configs[index].require_arg){ // �������Ƃ�I�v�V�����̏ꍇ
        if(*(++p) == 0){ // ����ȏ㕶�����Ȃ��Ȃ玟�̃R�}���h���C�������������Ƃ���
          flag->next = optflag_t::ARG;
        }else{ // �c��̕�����������Ƃ���
          flag->arg = p;
          flag->next = optflag_t::NONE;
        }
      }else{ // �����s�v�I�v�V�����̏ꍇ
        if(*(++p) == 0){ // �c��̕����񂪂Ȃ���΁A���̂܂܉�͏I��
          flag->next = optflag_t::NONE;
        }else{ // �c��̕����񂪂܂�����΁Ashort-style�I�v�V�����������Ă���Ɖ���
          // "-abc" => "-a", "-bc"
          flag->next = optflag_t::OPT;
          flag->arg = string("-") + p; // ���̉�͂ł� flag.arg ����͑ΏۂƂ���
        }
      }
      return index;
    }
  }
  return -1;
}

/* Windows��getopt */
char getopt(int argc, char *argv[], optconf_t configs[], int num_configs, optflag_t *flag){
  int index = -1;
  
  // short-style�R�}���h�������Ă��炸�Aindex���I�v�V�����z��͈̔͊O�ɍs�������͏I��
  if(argc <= flag->index && flag->next != optflag_t::OPT){
    return flag->next == optflag_t::ARG? GETOPT_ERROR: GETOPT_END; 
  }
  // ���
  switch(flag->next){
  case optflag_t::ARG: // ���݂̃R�}���h���C��������O�̃R�}���h���C���I�v�V�����̈����Ƃ���
    flag->next = optflag_t::NONE; // ���̃R�}���h���C�������͒ʏ��͂���
    flag->arg = argv[flag->index++];
    return flag->opt; // �O�̃I�v�V��������Ԃ�
  case optflag_t::OPT: // flag.arg ����͑ΏۂƂ���
    flag->next = optflag_t::NONE; // ���̃R�}���h���C�������͒ʏ��͂���
    index = judge_opt(flag->arg, configs, num_configs, flag);
    if(index < 0) return index; // �I�v�V�����ł͂Ȃ� or �G���[
    flag->opt = configs[index].short_name;
    // ���̃R�}���h���C���������I�v�V���������Ƃ��Ȃ��Ȃ�A�擾�����I�v�V��������Ԃ�
    if(flag->next != optflag_t::ARG) return flag->opt;
    break;
  default: // �ʏ���
    index = judge_opt(argv[flag->index++], configs, num_configs, flag);
    if(index < 0) return index; // �I�v�V�����ł͂Ȃ� or �G���[
    flag->opt = configs[index].short_name;
    // ���̃R�}���h���C���������I�v�V���������Ƃ��Ȃ��Ȃ�A�擾�����I�v�V��������Ԃ�
    if(flag->next != optflag_t::ARG) return flag->opt;
    break;
  }
  return getopt(argc, argv, configs, num_configs, flag); // ���̃R�}���h���C������������
}
