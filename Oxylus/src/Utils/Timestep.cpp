﻿#include "Timestep.h"

#include "Profiler.h"
#include "Timer.h"

namespace Oxylus {
Timestep::Timestep()
  : m_timestep(0.0)
    , m_last_time(0.0)
    , m_elapsed(0.0) {
  m_Timer = new Timer();
}

Timestep::~Timestep() {
  delete m_Timer;
}

void Timestep::on_update() {
  double currentTime = m_Timer->get_elapsed_msd();
  double maxFrameTime = -1.0;
  double dt = currentTime - m_last_time;

  {
    OX_SCOPED_ZONE_N("Sleep TimeStep to target fps");
    while (dt < maxFrameTime) {
      currentTime = m_Timer->get_elapsed_msd();
      dt = currentTime - m_last_time;
    }
  }

  m_timestep = currentTime - m_last_time;
  m_last_time = currentTime;
  m_elapsed += m_timestep;
}
}
