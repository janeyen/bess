#ifndef BESS_MODULE_ENCRYPT_H_
#define BESS_MODULE_ENCRYPT_H_

#include "module.h"
#include "../pb/module_msg.pb.h"
#include <string>

class Encrypt final: public Module{
  public:
    static const Commands cmds;
    CommandResponse Init(const bess::pb::EncryptArg &arg);
    CommandResponse CommandChange(const bess::pb::EncryptArg &arg);
    
    void ProcessBatch(bess::PacketBatch *batch) override;
  private:
    std::string key_;
};

#endif
