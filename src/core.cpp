#include "faux_engine/core.h"
#include "internals.h"
#include "easylogging++.h"
#include <stdio.h>

nx::Core::Core() {}
nx::Core::~Core() {}

nx::Core& nx::Core::Instance() {
  static std::unique_ptr<Core> core;
  if (nullptr == core.get()) core = std::unique_ptr<Core>(new Core());
  return *core.get();
}

INITIALIZE_EASYLOGGINGPP

nx::Result nx::Core::init(Settings s) {
  remove("../data/logger.log");
  el::Configurations conf("../data/logger.conf");
  el::Loggers::reconfigureAllLoggers(conf);

  if (window_.init(s.width_, s.height_, s.name_) != nx::Result::Success) {
    return nx::Result::Error;
  }
  if (renderer_.init(s.backend_) != nx::Result::Success) {
    return nx::Result::Error;
  }
  if (mmanager_.init() != nx::Result::Success) {
    return nx::Result::Error;
  }

  NXInternal.InternalStartup(s.data_);
  return nx::Result::Success;
}

