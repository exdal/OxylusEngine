#include "MaterialSerializer.h"
#include "Material.h"
#include <fstream>

#include "AssetManager.h"
#include "Utils/FileUtils.h"
#include "Utils/Log.h"
#include "Utils/Profiler.h"

namespace Oxylus {
void MaterialSerializer::serialize(const std::string& path) const {
  OX_SCOPED_ZONE;
  m_material->path = path;

  ryml::Tree tree;

  ryml::NodeRef node_root = tree.rootref();
  node_root |= ryml::MAP;

  node_root["Material"] << m_material->name;

  auto node_refs = node_root["Parameters"];
  node_refs |= ryml::MAP;

  const auto& parameters = m_material->parameters;
  node_refs["Roughness"] << parameters.roughness;
  node_refs["Metallic"] << parameters.metallic;
  node_refs["Reflectance"] << parameters.reflectance;
  node_refs["Normal"] << parameters.normal;
  node_refs["AO"] << parameters.ao;
  auto colorNode = node_refs["Color"];
  glm::write(&colorNode, parameters.color);
  node_refs["UseAlbedo"] << parameters.use_albedo;
  node_refs["UsePhysicalMap"] << parameters.use_physical_map;
  node_refs["UseNormal"] << parameters.use_normal;
  node_refs["UseAO"] << parameters.use_ao;
  node_refs["UseEmissive"] << parameters.use_ao;
  node_refs["DoubleSided"] << parameters.double_sided;
  node_refs["UVScale"] << parameters.uv_scale;

  auto texture_node = node_root["Textures"];
  texture_node |= ryml::MAP;

  save_if_path_exists(texture_node["Albedo"], m_material->albedo_texture);
  save_if_path_exists(texture_node["Normal"], m_material->normal_texture);
  save_if_path_exists(texture_node["Physical"], m_material->metallic_roughness_texture);
  save_if_path_exists(texture_node["AO"], m_material->ao_texture);
  save_if_path_exists(texture_node["Emissive"], m_material->emissive_texture);

  std::stringstream ss;
  ss << tree;
  std::ofstream filestream(path);
  filestream << ss.str();
}

void MaterialSerializer::deserialize(const std::string& path) const {
  if (path.empty())
    return;
  OX_SCOPED_ZONE;

  m_material->destroy();

  m_material->path = path;

  auto content = FileUtils::read_file(path);
  if (content.empty()) {
    OX_CORE_ERROR("Couldn't read material file: {0}", path);

    // Try to read it again from assets path
    content = FileUtils::read_file(AssetManager::get_asset_file_system_path(path).string());
    if (!content.empty())
      OX_CORE_INFO("Could load the material file from assets path: {0}", path);
    else
      return;
  }

  ryml::Tree tree = ryml::parse_in_arena(c4::to_csubstr(content));

  const ryml::ConstNodeRef node_refs = tree.rootref();

  auto& parameters = m_material->parameters;

  node_refs["Material"] >> m_material->name;

  const auto par_node = node_refs["Parameters"];
  TryLoad(par_node, "Roughness", parameters.roughness);
  TryLoad(par_node, "Metallic", parameters.metallic);
  TryLoad(par_node, "Reflectance", parameters.reflectance);
  TryLoad(par_node, "Normal", parameters.normal);
  TryLoad(par_node, "AO", parameters.ao);
  glm::read(par_node["Color"], &parameters.color);
  TryLoad(par_node, "UseAlbedo", parameters.use_albedo);
  TryLoad(par_node, "UsePhysicalMap", parameters.use_physical_map);
  TryLoad(par_node, "UseNormal", parameters.use_normal);
  TryLoad(par_node, "UseAO", parameters.use_ao);
  TryLoad(par_node, "UseEmissive", parameters.use_ao);
  TryLoad(par_node, "DoubleSided", parameters.double_sided);
  TryLoad(par_node, "UVScale", parameters.uv_scale);

  load_if_path_exists(node_refs["Textures"], "Albedo", m_material->albedo_texture);
  load_if_path_exists(node_refs["Textures"], "Normal", m_material->normal_texture);
  load_if_path_exists(node_refs["Textures"], "Physical", m_material->metallic_roughness_texture);
  load_if_path_exists(node_refs["Textures"], "AO", m_material->ao_texture);
  load_if_path_exists(node_refs["Textures"], "Emmisive", m_material->emissive_texture);
}

void MaterialSerializer::save_if_path_exists(ryml::NodeRef node, const Ref<TextureAsset>& texture) {
  if (!texture->get_path().empty())
    node << texture->get_path();
}

void MaterialSerializer::load_if_path_exists(ryml::ConstNodeRef parent_node,
                                             const char* node_name,
                                             Ref<TextureAsset>& texture) {
  std::string path{};
  if (parent_node.has_child(ryml::to_csubstr(node_name))) {
    parent_node[ryml::to_csubstr(node_name)] >> path;
    texture = AssetManager::get_texture_asset({path});
  }
}
}
