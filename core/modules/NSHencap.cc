#include "NSHencap.h"

#include "../utils/ether.h"
#include "../utils/nsh.h"

using bess::utils::Ethernet;
using bess::utils::NSH;
using bess::utils::be16_t;

enum {
//  ATTR_R_NSH_NEXTPROTO,
  ATTR_R_NSH_SPI,
  ATTR_R_NSH_SI,
//  ATTR_W_ETHER_TYPE,
};

/*struct uint24_t{
  unsigned int value : 24;
};*/

struct uint6_t{
  unsigned int value : 6;
};

CommandResponse NSHencap::Init(const bess::pb::NSHencapArg &arg[[maybe_unused]]){
//  using AccessMode = bess::metadata::Attribute::AccessMode;

  new_SPI_.value = (unsigned)stoi(arg.new_spi());
  new_SI_ = (unsigned)stoi(arg.new_si());
//  AddMetadataAttr("nextProto",1, AccessMode::kRead);
//  AddMetadataAttr("nsh_spi", 3, AccessMode::kRead);
//  AddMetadataAttr("nsh_si", 1, AccessMode::kRead);

  return CommandSuccess();
}

void NSHencap::ProcessBatch(bess::PacketBatch *batch){
  int cnt = batch->cnt();

  for(int i = 0; i<cnt; i++){
    bess::Packet *pkt = batch->pkts()[i];

//    uint8_t NSH_nextProto = get_attr<uint8_t>(this, ATTR_R_NSH_NEXTPROTO, pkt);
//    uint24_t NSH_SPI = get_attr<uint24_t>(this, ATTR_R_NSH_SPI, pkt);
//    uint8_t NSH_SI = get_attr<uint8_t>(this, ATTR_R_NSH_SI, pkt);

    Ethernet *eth = pkt->head_data<Ethernet *>();
    Ethernet::Address ether_src = eth->src_addr;
    Ethernet::Address ether_dst = eth->dst_addr;
    bess::utils::be16_t ether_type = eth->ether_type;

    pkt->adj(sizeof(*eth));

    NSH *nsh;
//    uint16_t total_len = pkt->total_len() + sizeof(*nsh);
    nsh = static_cast<NSH *>(pkt->prepend(sizeof(*nsh)));

    if(unlikely(!nsh)){
      continue;
    }

    nsh->version = 0x0;
    nsh->oBit = 0;
    nsh->uBit = 0;
    nsh->ttl = 0;
    nsh->totalLength = 0;
    nsh->unsign = 1;
    nsh->MD = 0;
    nsh->nextProto = ((ether_type == be16_t(0x0800) )? 0x1 : 0xFF);
    nsh->SPI = new_SPI_.value & 0xFFFFFF;
    nsh->SI = new_SI_;
    nsh->context1 = 0;
    nsh->context2 = 0; 

    Ethernet *eth_copy = static_cast<Ethernet *>(pkt->prepend(sizeof(*eth_copy)));
    eth_copy->src_addr = ether_src;
    eth_copy->dst_addr = ether_dst;
    eth_copy->ether_type = be16_t(Ethernet::Type::kNSH);
//    set_attr<be16_t>(this, ATTR_W_ETHER_TYPE, pkt, be16_t(Ethernet::Type::kNSH));
  }
  RunNextModule(batch);
}

ADD_MODULE(NSHencap, "NSHencap", "add NSH header")
