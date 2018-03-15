#ifndef BESS_MODULE_NSHENCAP_H_
#define BESS_MODULE_NSHENCAP_H_

#include "../module.h"
#include "../pb/module_msg.pb.h"

struct uint24_t{
  unsigned value: 24;
};

class NSHencap final: public Module{
  public:
    NSHencap() : Module() { max_allowed_workers_ = Worker::kMaxWorkers; }

    CommandResponse Init(const bess::pb::NSHencapArg &arg);

    void ProcessBatch(bess::PacketBatch *batch) override;
  private:
    uint24_t new_SPI_;
    uint8_t new_SI_;
};

#endif
