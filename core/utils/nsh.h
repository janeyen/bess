#ifndef BESS_UTILS_NSH_H_
#define BESS_UTILS_NSH_H_

namespace bess{
  namespace utils{
    struct[[gnu::packed]] NSH{
      unsigned version: 2;
      unsigned oBit: 1;
      unsigned uBit: 1;
      unsigned ttl: 6;
      unsigned totalLength: 6;
      unsigned unsign: 4;
      unsigned MD: 4;
      uint8_t nextProto;
      unsigned SPI: 24;
      uint8_t SI;
      uint64_t context1;
      uint64_t context2;
    };

    static_assert(sizeof(NSH)==24, "struct NSH is incorrect");
  }
}

#endif
