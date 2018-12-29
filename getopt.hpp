#pragma once

#include <string>

using namespace std;

/* �I�v�V�������ݒ�p�\���� */
struct optconf_t{
  char short_name;
  string long_name;
  bool require_arg;
};

/* ��͗p�t���O�Ǘ��\���� */
struct optflag_t{
  enum{
    NONE, OPT, ARG
  };
  
  char opt;   // �O���͎���short-style�I�v�V������
  string arg; // ����
  int next;   // ���̃R�}���h���C���������ǂ���͂��邩
  int index;  // getopt�֐��Ăяo���̓x�ɃC���N�������g�����
};


/* Windows��getopt */
// �߂�l
enum{
  GETOPT_END = 0, GETOPT_NONE = -1, GETOPT_UNKNOWN = -2, GETOPT_ERROR = -3
};

// �R�}���h���C�������̔z�����͂��A�R�}���h���C���I�v�V�������擾
// @argc: �R�}���h���C�������̐�
// @argv[]: �R�}���h���C�������̔z��
// @configs[]: �ݒ肷��I�v�V�������̔z��
// @flag: optflag_t�\���̂ւ̃|�C���^���w��i�������Ƃ�I�v�V�����̈����� flag.arg �Ŏ擾�\�j
//        �ŏ��� getopt ���s�O�� .next, .index ��0�N���A���Ă�������
// @return: �Ώۂ̃I�v�V�����������݂���Ȃ�short-style�I�v�V��������Ԃ�
//          <Error code>
//          GETOPT_NONE: �R�}���h���C���I�v�V�����ł͂Ȃ��R�}���h���C������
//          GETOPT_UNKNOWN: �ݒ肳��Ă��Ȃ��I�v�V������
//          GETOPT_ERROR: �I�v�V�����w��̍\���G���[
char getopt(int argc, char *argv[], optconf_t configs[], int num_configs, optflag_t *flag);
