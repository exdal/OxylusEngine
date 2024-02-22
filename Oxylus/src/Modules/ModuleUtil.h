﻿#pragma once

#include <string>

namespace Ox {
class ModuleUtil {
public:
  static void load_module(const std::string& name, const std::string& path);
  static void unload_module(const std::string& module_name);
};
}

