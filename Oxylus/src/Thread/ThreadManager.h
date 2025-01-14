#pragma once

#include "Thread.h"

namespace Oxylus {
class ThreadManager {
public:
  Thread asset_thread;
  Thread render_thread;

  ThreadManager();

  ~ThreadManager() = default;

  void wait_all_threads();

  static ThreadManager* get() { return instance; }

private:
  static ThreadManager* instance;
};
}
