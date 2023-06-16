﻿#include "src/oxpch.h"
#include "RuntimeConsole.h"

#include <icons/IconsMaterialDesignIcons.h>

#include "ExternalConsoleSink.h"
#include "Core/Application.h"
#include "Utils/ImGuiScoped.h"
#include "Utils/StringUtils.h"

namespace Oxylus {
  static ImVec4 GetColor(const spdlog::level::level_enum level) {
    switch (level) {
      case SPDLOG_LEVEL_INFO: return {1, 1, 1, 1};
      case SPDLOG_LEVEL_WARN: return {0.2f, 0, 0, 1};
      case SPDLOG_LEVEL_ERROR: return {1, 0, 0, 1};
      default: return {1, 1, 1, 1};
    }
  }

  static const char8_t* GetLevelIcon(spdlog::level::level_enum level) {
    switch (level) {
      case spdlog::level::trace: return ICON_MDI_MESSAGE_TEXT;
      case spdlog::level::debug: return ICON_MDI_BUG;
      case spdlog::level::info: return ICON_MDI_INFORMATION;
      case spdlog::level::warn: return ICON_MDI_ALERT;
      case spdlog::level::err: return ICON_MDI_CLOSE_OCTAGON;
      case spdlog::level::critical: return ICON_MDI_ALERT_OCTAGRAM;
      default: ;
    }

    return u8"Unknown name";
  }

  RuntimeConsole::RuntimeConsole() {
    ExternalConsoleSink::SetConsoleSink_HandleFlush([this](std::string_view message,
                                                           const char*,
                                                           const char*,
                                                           int32_t,
                                                           spdlog::level::level_enum level) {
      AddLog(message.data(), level);
    });

    // Default commands
    RegisterCommand("quit", [] { Application::Get().Close(); });
    RegisterCommand("clear", [this] { ClearLog(); });
  }

  void RuntimeConsole::RegisterCommand(const std::string& command, const std::function<void()>& action) {
    m_CommandMap.emplace(command, action);
  }

  void RuntimeConsole::AddLog(const char* fmt, spdlog::level::level_enum level) {
    m_TextBuffer.emplace_back(fmt);
  }

  void RuntimeConsole::ClearLog() {
    m_TextBuffer.clear();
  }

  void RuntimeConsole::OnImGuiRender() {
    constexpr ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_MenuBar;
    const auto name = fmt::format(" {} {}\t\t###{}{}", StringUtils::FromChar8T(ICON_MDI_CONSOLE), PanelName, 0, PanelName);
    if (ImGui::Begin(name.c_str(), &Visible, windowFlags | ImGuiWindowFlags_NoCollapse)) {
      if (RenderMenuBar) {
        if (ImGui::BeginMenuBar()) {
          if (ImGui::MenuItem(StringUtils::FromChar8T(ICON_MDI_TRASH_CAN))) {
            ClearLog();
          }
          ImGui::EndMenuBar();
        }
      }
      ImGui::Separator();

      constexpr ImGuiTableFlags tableFlags = ImGuiTableFlags_RowBg | ImGuiTableFlags_ContextMenuInBody |
                                             ImGuiTableFlags_ScrollY;

      float width = 0;
      if (ImGui::BeginChild("TextTable", ImVec2(0, -35))) {
        ImGuiScoped::StyleVar style1(ImGuiStyleVar_CellPadding, {1, 1});
        if (ImGui::BeginTable("ScrollRegionTable", 1, tableFlags)) {
          width = ImGui::GetWindowSize().x;
          for (auto& text : m_TextBuffer) {
            text.Render();
          }
          if (m_RequestScrollToBottom && ImGui::GetScrollMaxY() > 0) {
            ImGui::SetScrollY(ImGui::GetScrollMaxY());
            m_RequestScrollToBottom = false;
          }
          ImGui::EndTable();
        }
        ImGui::EndChild();
      }

      ImGui::Separator();
      ImGui::PushItemWidth(width - 10);
      constexpr ImGuiInputTextFlags inputFlags = ImGuiInputTextFlags_EnterReturnsTrue |
                                                 ImGuiInputTextFlags_CallbackHistory |
                                                 ImGuiInputTextFlags_EscapeClearsAll;
      static char s_InputBuf[256];
      if (ImGui::InputText("##",
        s_InputBuf,
        OX_ARRAYSIZE(s_InputBuf),
        inputFlags,
        [](ImGuiInputTextCallbackData* data) {
          const auto panel = (RuntimeConsole*)data->UserData;
          return panel->InputTextCallback(data);
        },
        this)) {
        ProcessCommand(s_InputBuf);
        m_InputLog.emplace_back(s_InputBuf);
        memset(s_InputBuf, 0, sizeof s_InputBuf);
      }
      ImGui::PopItemWidth();

      ImGui::End();
    }
  }

  void RuntimeConsole::ConsoleText::Render() const {
    ImGui::TableNextRow();
    ImGui::TableNextColumn();

    ImGuiTreeNodeFlags flags = 0;
    flags |= ImGuiTreeNodeFlags_OpenOnArrow;
    flags |= ImGuiTreeNodeFlags_SpanFullWidth;
    flags |= ImGuiTreeNodeFlags_FramePadding;
    flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;

    ImGui::PushID(Text.c_str());
    ImGui::PushStyleColor(ImGuiCol_Text, GetColor(Level));
    const auto levelIcon = GetLevelIcon(Level);
    ImGui::TreeNodeEx(Text.c_str(), flags, "%s  %s", StringUtils::FromChar8T(levelIcon), Text.c_str());
    ImGui::PopStyleColor();

    if (ImGui::BeginPopupContextItem("Popup")) {
      if (ImGui::MenuItem("Copy"))
        ImGui::SetClipboardText(Text.c_str());

      ImGui::EndPopup();
    }
    ImGui::PopID();
  }

  void RuntimeConsole::ProcessCommand(const std::string& command) {
    if (m_CommandMap.contains(command)) {
      m_CommandMap[command]();
      AddLog(command.c_str(), spdlog::level::level_enum::info);
    }
    else {
      AddLog("Non existent command.", spdlog::level::level_enum::err);
    }
  }

  int RuntimeConsole::InputTextCallback(ImGuiInputTextCallbackData* data) {
    if (data->EventFlag == ImGuiInputTextFlags_CallbackHistory) {
      const int prevHistoryPos = m_HistoryPosition;
      if (data->EventKey == ImGuiKey_UpArrow) {
        if (m_HistoryPosition == -1)
          m_HistoryPosition = (int32_t)m_InputLog.size() - 1;
        else if (m_HistoryPosition > 0)
          m_HistoryPosition--;
      }
      else if (data->EventKey == ImGuiKey_DownArrow) {
        if (m_HistoryPosition != -1)
          if (++m_HistoryPosition >= (int32_t)m_InputLog.size())
            m_HistoryPosition = -1;
      }

      if (prevHistoryPos != m_HistoryPosition) {
        const char* historyStr = m_HistoryPosition >= 0 ? m_InputLog[m_HistoryPosition] : "";
        data->DeleteChars(0, data->BufTextLen);
        data->InsertChars(0, historyStr);
      }
    }

    return 0;
  }
}