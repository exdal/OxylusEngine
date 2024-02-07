﻿#include "LuaAssetManagerBindings.h"

#include <sol/state.hpp>

#include "Assets/AssetManager.h"

#include "Audio/AudioSource.h"

#include "Render/Mesh.h"

namespace Ox::LuaBindings {
void bind_asset_manager(const Shared<sol::state>& state) {
  auto asset_table = state->create_table("Assets");
  asset_table.set_function("get_mesh", [](const std::string& path) -> Shared<Mesh> { return AssetManager::get_mesh_asset(path); });
  asset_table.set_function("get_texture", [](const std::string& path) -> Shared<TextureAsset> { return AssetManager::get_texture_asset(path, {.path = path}); });
  asset_table.set_function("get_source", [](const std::string& path) -> Shared<AudioSource> { return AssetManager::get_audio_asset(path); });
}
}