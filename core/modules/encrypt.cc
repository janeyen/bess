#include "encrypt.h"
#include <iostream>

const Commands Encrypt::cmds{
  {"change", "EncryptArg", MODULE_CMD_FUNC(&Encrypt::CommandChange),Command::Command::THREAD_UNSAFE}
};

CommandResponse Encrypt::Init(const bess::pb::EncryptArg &arg){
  key_ = arg.key();
  return CommandSuccess();
}

CommandResponse Encrypt::CommandChange(const bess::pb::EncryptArg &arg){
  Init(arg);
  return CommandSuccess();
}

void Encrypt::ProcessBatch(bess::PacketBatch *batch){
  int cnt = batch->cnt();
  for(int i = 0; i<cnt; i++){
    bess::Packet *pkt = batch->pkts()[i];

    char *content = pkt->data();
    std::cout<< *content<<"\n";
    std::string tmp(key_);
    while(tmp.size()<SNBUF_DATA){
      tmp+=key_;
    }
    for(int j = 0; j<SNBUF_DATA; j++){
      content[j] = content[j]^tmp[j];
    }
  }
  RunNextModule(batch);
}

ADD_MODULE(Encrypt, "encrypt", "encrypt packet payload")
