﻿#include "LuaManager.h"

#include <sol/sol.hpp>

#include "LuaECSBindings.h"
#include "LuaInputBindings.h"
#include "LuaMathBindings.h"

#include "Scene/Scene.h"
#include "Core/Entity.h"
#include "Core/Input.h"

namespace Oxylus {
void LuaManager::init() {
  m_state = create_ref<sol::state>();
  m_state->open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table, sol::lib::os, sol::lib::string);

  bind_log();
  LuaBindings::bind_lua_math(m_state);
  LuaBindings::bind_lua_ecs(m_state);
  LuaBindings::bind_lua_input(m_state);
}

LuaManager* LuaManager::get() {
  static LuaManager manager = {};
  return &manager;
}

void LuaManager::bind_log() const {
  auto log = m_state->create_table("Log");

  log.set_function("trace", [&](sol::this_state s, const std::string_view message) { OX_CORE_TRACE("{}", message); });
  log.set_function("info", [&](sol::this_state s, const std::string_view message) { OX_CORE_INFO("{}", message); });
  log.set_function("warn", [&](sol::this_state s, const std::string_view message) { OX_CORE_WARN("{}", message); });
  log.set_function("error", [&](sol::this_state s, const std::string_view message) { OX_CORE_ERROR("{}", message); });
}
}
