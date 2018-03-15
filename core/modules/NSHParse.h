#ifndef BESS_MODULES_NSHPARSE_H_
#define BESS_MODULES_NSHPARSE_H_

#include <vector>

#include "../module.h"
#include "../pb/module_msg.pb.h"

class NSHParse final: public Module{
  public:
    struct NSHParseRule{
      bool Match(uint32_t spi_long, uint8_t si) const{
        return (old_SPI == (spi_long & 0xFFFFFF))&&(old_SI == si);
      }
      unsigned old_SPI: 24;
      unsigned old_SI: 8;
      unsigned new_SPI: 24;
      unsigned new_SI: 8;
      uint32_t ogate;
    };

    static const gate_idx_t InputGates = MAX_GATES;
    static const gate_idx_t OutputGates = MAX_GATES;

    static const Commands cmds;
    CommandResponse Init(const bess::pb::NSHParseArg &arg);
    CommandResponse CommandAdd(const bess::pb::NSHParseArg &arg);
    
    void ProcessBatch(bess::PacketBatch *batch) override;
  private:
    std::vector<NSHParseRule> rules_;
};

#endif
