﻿#pragma once
#include <vector>

#include "EditorPanel.h"

namespace Oxylus {
  class StatisticsPanel : public EditorPanel {
  public:
    StatisticsPanel();
    void on_imgui_render() override;

  private:
    float m_FpsValues[50] = {};
    std::vector<float> m_FrameTimes{};

    void MemoryTab() const;
    void RendererTab();
  };
}
