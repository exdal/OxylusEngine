#pragma once

#include <filesystem>
#include <stack>
#include <vector>

#include <imgui.h>
#include <mutex>
#include <unordered_map>

#include "EditorPanel.h"

#include "Core/Base.h"

namespace Oxylus {
class TextureAsset;

enum class FileType {
    Unknown = 0,
    Scene,
    Prefab,
    Shader,
    Texture,
    Cubemap,
    Model,
    Audio,
    Material
  };

  class ContentPanel : public EditorPanel {
  public:
    ContentPanel();

    ~ContentPanel() override = default;

    ContentPanel(const ContentPanel& other) = delete;
    ContentPanel(ContentPanel&& other) = delete;
    ContentPanel& operator=(const ContentPanel& other) = delete;
    ContentPanel& operator=(ContentPanel&& other) = delete;

    void init();
    void on_update() override;
    void on_imgui_render() override;

    void invalidate();

  private:
    std::pair<bool, uint32_t> directory_tree_view_recursive(const std::filesystem::path& path, uint32_t* count, int* selectionMask, ImGuiTreeNodeFlags flags);
    void render_header();
    void render_side_view();
    void render_body(bool grid);
    void update_directory_entries(const std::filesystem::path& directory);
    void refresh() { update_directory_entries(m_current_directory); }

    void draw_context_menu_items(const std::filesystem::path& context, bool isDir);

  private:
    struct File {
      std::string name;
      std::string file_path;
      std::string extension;
      std::filesystem::directory_entry directory_entry;
      Ref<TextureAsset> thumbnail = nullptr;
      bool is_directory = false;

      FileType type;
      std::string_view file_type_string;
      ImVec4 file_type_indicator_color;
    };

    std::filesystem::path m_assets_directory;
    std::filesystem::path m_current_directory;
    std::stack<std::filesystem::path> m_back_stack;
    std::vector<File> m_directory_entries;
    std::mutex m_directory_mutex;
    uint32_t m_currently_visible_items_tree_view = 0;
    float m_thumbnail_size = 128.0f;
    ImGuiTextFilter m_filter;
    float m_elapsed_time = 0.0f;
    bool m_texture_previews = true;

    std::unordered_map<std::string, Ref<TextureAsset>> thumbnail_cache;

    Ref<TextureAsset> m_white_texture;
    std::filesystem::path m_directory_to_delete;
  };
}
