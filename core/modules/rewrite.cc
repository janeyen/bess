#include "rewrite.h"

#include <cstdio>

#include "../utils/copy.h"

const Commands Rewrite::cmds = {
    {"add", "RewriteArg", MODULE_CMD_FUNC(&Rewrite::CommandAdd), 0},
    {"clear", "EmptyArg", MODULE_CMD_FUNC(&Rewrite::CommandClear), 0},
};

pb_error_t Rewrite::Init(const bess::pb::RewriteArg &arg) {
  if (arg.templates_size() > 0) {
    pb_cmd_response_t response = CommandAdd(arg);
    return response.error();
  } else {
    return pb_errno(0);
  }
}

pb_cmd_response_t Rewrite::CommandAdd(const bess::pb::RewriteArg &arg) {
  pb_cmd_response_t response;

  size_t curr = num_templates_;

  if (curr + arg.templates_size() > bess::PacketBatch::kMaxBurst) {
    set_cmd_response_error(&response, pb_error(EINVAL,
                                               "max %zu packet templates "
                                               "can be used %zu %d",
                                               bess::PacketBatch::kMaxBurst,
                                               curr, arg.templates_size()));
    return response;
  }

  for (int i = 0; i < arg.templates_size(); i++) {
    const auto &templ = arg.templates(i);

    if (templ.length() > kMaxTemplateSize) {
      set_cmd_response_error(&response,
                             pb_error(EINVAL, "template is too big"));
      return response;
    }

    bess::utils::Copy(templates_[curr + i], 0, kMaxTemplateSize);
    bess::utils::Copy(templates_[curr + i], templ.c_str(), templ.length());
    template_size_[curr + i] = templ.length();
  }

  num_templates_ = curr + arg.templates_size();
  if (num_templates_ == 0) {
    set_cmd_response_error(&response, pb_errno(0));
    return response;
  }

  for (size_t i = num_templates_; i < kNumSlots; i++) {
    size_t j = i % num_templates_;
    bess::utils::Copy(templates_[i], templates_[j], template_size_[j]);
    template_size_[i] = template_size_[j];
  }

  set_cmd_response_error(&response, pb_errno(0));
  return response;
}

pb_cmd_response_t Rewrite::CommandClear(const bess::pb::EmptyArg &) {
  next_turn_ = 0;
  num_templates_ = 0;

  pb_cmd_response_t response;

  set_cmd_response_error(&response, pb_errno(0));
  return response;
}

inline void Rewrite::DoRewriteSingle(bess::PacketBatch *batch) {
  const int cnt = batch->cnt();
  uint16_t size = template_size_[0];
  const void *templ = templates_[0];

  for (int i = 0; i < cnt; i++) {
    bess::Packet *pkt = batch->pkts()[i];
    char *ptr = pkt->buffer<char *>() + SNBUF_HEADROOM;

    pkt->set_data_off(SNBUF_HEADROOM);
    pkt->set_total_len(size);
    pkt->set_data_len(size);

    bess::utils::CopyInlined(ptr, templ, size, true);
  }
}

inline void Rewrite::DoRewrite(bess::PacketBatch *batch) {
  size_t start = next_turn_;
  const size_t cnt = batch->cnt();

  for (size_t i = 0; i < cnt; i++) {
    uint16_t size = template_size_[start + i];
    bess::Packet *pkt = batch->pkts()[i];
    char *ptr = pkt->buffer<char *>() + SNBUF_HEADROOM;

    pkt->set_data_off(SNBUF_HEADROOM);
    pkt->set_total_len(size);
    pkt->set_data_len(size);

    bess::utils::CopyInlined(ptr, templates_[start + i], size, true);
  }

  next_turn_ = start + cnt;
  if (next_turn_ >= bess::PacketBatch::kMaxBurst) {
    next_turn_ -= bess::PacketBatch::kMaxBurst;
  }
}

void Rewrite::ProcessBatch(bess::PacketBatch *batch) {
  if (num_templates_ == 1) {
    DoRewriteSingle(batch);
  } else if (num_templates_ > 1) {
    DoRewrite(batch);
  }

  RunNextModule(batch);
}

ADD_MODULE(Rewrite, "rewrite", "replaces entire packet data")
