#include "NSHdecap.h"

#include "../utils/ether.h"
#include "../utils/nsh.h"

enum{
//  ATTR_W_ETHER_SRC,
//  ATTR_W_ETHER_DST,
//  ATTR_W_ETHER_TYPE,
  ATTR_W_NSH_SPI,
  ATTR_W_NSH_SI,
};

CommandResponse NSHdecap::Init(const bess::pb::NSHdecapArg &arg[[maybe_unused]]){
  using AccessMode = bess::metadata::Attribute::AccessMode;

//    new_SPI_.value = 100;
//    new_SI_ = 1;
  new_SPI_.value = (unsigned)stoi(arg.new_spi());
  new_SI_ = (unsigned)stoi(arg.new_si());

//  AddMetadataAttr("ether_src", sizeof(Ethernet::Address), AccessMode::kWrite);
//  AddMetadataAttr("ether_dst", sizeof(Ethernet::Address), AccessMode::kWrite);
//  AddMetadataAttr("ether_type", 2, AccessMode::kWrite);
  AddMetadataAttr("nsh_spi", 3, AccessMode::kWrite);
  AddMetadataAttr("nsh_si", 1, AccessMode::kWrite);

  return CommandSuccess();
}

void NSHdecap::ProcessBatch(bess::PacketBatch *batch){
  using bess::utils::Ethernet;
  using bess::utils::NSH;
  using bess::utils::be16_t;

  int cnt = batch->cnt();

  for(int i = 0; i<cnt; i++){
    std::cout<<i<<"\n";
    bess::Packet *pkt = batch->pkts()[i];

    Ethernet *eth = pkt->head_data<Ethernet *>();
    NSH *nsh = reinterpret_cast<NSH *>(eth+1);

    Ethernet::Address ether_src = eth->src_addr;
    Ethernet::Address ether_dst = eth->dst_addr;
//    bess::utils::be16_t ether_type = eth->ether_type;
    uint24_t spi_copy;
    spi_copy.value = nsh->SPI;
    uint8_t si_copy = nsh->SI;

    pkt->adj(sizeof(*eth)+sizeof(*nsh));
//    pkt->adj(sizeof(*nsh));

    Ethernet *eth_copy = static_cast<Ethernet *>(pkt->prepend(sizeof(*eth_copy)));

    eth_copy->src_addr = ether_src;
    eth_copy->dst_addr = ether_dst;
    eth_copy->ether_type = be16_t(Ethernet::Type::kIpv4);

    set_attr<uint24_t>(this, ATTR_W_NSH_SPI, pkt, spi_copy);
    set_attr<uint8_t>(this, ATTR_W_NSH_SI, pkt, si_copy);
  }
  RunNextModule(batch);
}

ADD_MODULE(NSHdecap, "NSHdecap", "decap NSH header and set metadata for SPI & SI")
