#include "ThreadManager.h"

namespace Oxylus {
ThreadManager* ThreadManager::instance = nullptr;

ThreadManager::ThreadManager() {
  instance = this;
}

void ThreadManager::wait_all_threads() {
  asset_thread.wait();
  render_thread.wait();
}
}
