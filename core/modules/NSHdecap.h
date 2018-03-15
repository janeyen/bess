#ifndef BESS_MODULES_NSHDECAP_H_
#define BESS_MODULES_NSHDECAP_H_

#include "../module.h"
#include "../pb/module_msg.pb.h"

struct uint24_t{
  unsigned value: 24;
};

class NSHdecap final: public Module{
  public:
    NSHdecap() : Module(){ max_allowed_workers_ =  Worker::kMaxWorkers; }

    CommandResponse Init(const bess::pb::NSHdecapArg &arg[[maybe_unused]]);

    void ProcessBatch(bess::PacketBatch *batch) override;
  private:
    uint24_t new_SPI_;
    uint8_t new_SI_;
};

#endif
