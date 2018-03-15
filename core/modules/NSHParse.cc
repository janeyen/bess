#include "NSHParse.h"

#include "../utils/ether.h"
#include "../utils/nsh.h"

const Commands NSHParse::cmds{
  {"add", "NSHParseArg", MODULE_CMD_FUNC(&NSHParse::CommandAdd),
   Command::Command::THREAD_UNSAFE}};

CommandResponse NSHParse::Init(const bess::pb::NSHParseArg &arg){
  for(const auto &rule : arg.rules()){
    NSHParseRule new_rule = {
/*      .old_SPI = (unsigned)rule.old_SPI(),
      .old_SI = rule.old_SI(),
      .new_SPI = (unsigned)rule.new_SPI(),
      .new_SI = rule.new_SI(),*/
      .old_SPI = (unsigned)stoi(rule.old_spi()),
      .old_SI = (unsigned)stoi(rule.old_si()),
      .new_SPI = (unsigned)stoi(rule.new_spi()),
      .new_SI = (unsigned)stoi(rule.new_si()),
      .ogate = rule.ogate()
    };
    rules_.push_back(new_rule);
  }
  return CommandSuccess();
}

CommandResponse NSHParse::CommandAdd(const bess::pb::NSHParseArg &arg){
  Init(arg);
  return CommandSuccess();
}

void NSHParse::ProcessBatch(bess::PacketBatch *batch){
  using bess::utils::Ethernet;
  using bess::utils::NSH;

  gate_idx_t out_gates[bess::PacketBatch::kMaxBurst];

  int cnt = batch->cnt();
  for(int i = 0; i<cnt; i++){
    bess::Packet *pkt = batch->pkts()[i];

    Ethernet *eth = pkt->head_data<Ethernet *>();
    NSH *nsh = reinterpret_cast<NSH *>(eth+1);

    out_gates[i] = 0;     //default sending out the NSH module

    for(const auto &rule : rules_){
      if(rule.Match((nsh->SPI & 0x00FFFFFF), nsh->SI)){
        nsh->SPI = rule.new_SPI;
        nsh->SI = rule.new_SI;
        out_gates[i] = rule.ogate;
      }
      break;    //stop matching other rules if match
    }
  }
  RunSplit(out_gates,batch);
}

ADD_MODULE(NSHParse, "nsh", "parse SPI and SI to determine function")
