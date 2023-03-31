/**
 * Furnace Tracker - multi-system chiptune tracker
 * Copyright (C) 2021-2023 tildearrow and contributors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "gui.h"
#include "../ta-log.h"
#include "plot_nolerp.h"
#include "IconsFontAwesome4.h"
#include "misc/cpp/imgui_stdlib.h"
#include <fmt/printf.h>
#include <math.h>
#include <imgui.h>
#include <MidiFile.h>

void FurnaceGUI::drawMIDIImport() {
  if (nextWindow==GUI_WINDOW_MIDI_IMPORT) {
    midiImportOpen=true;
    ImGui::SetNextWindowFocus();
    nextWindow=GUI_WINDOW_NOTHING;
  }
  if (!midiImportOpen) return;
  if (ImGui::Begin("MIDI Import",&midiImportOpen,globalWinFlags)) {
    if (pendingMIDI == NULL) {
      if (ImGui::BeginTable("noAssetCenter",3)) {
          ImGui::TableSetupColumn("c0",ImGuiTableColumnFlags_WidthStretch,0.5f);
          ImGui::TableSetupColumn("c1",ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableSetupColumn("c2",ImGuiTableColumnFlags_WidthStretch,0.5f);

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TableNextColumn();

          if (ImGui::Button("Open")) {
            doAction(GUI_ACTION_MIDI_IMPORT_OPEN);
          }

          ImGui::TableNextColumn();
          ImGui::EndTable();
        }
    } else {
      if (ImGui::BeginTable("midiImport", 3)) {
          ImGui::TableSetupColumn("c0",ImGuiTableColumnFlags_WidthStretch,0.5f);
          ImGui::TableSetupColumn("c1",ImGuiTableColumnFlags_WidthFixed);
          ImGui::TableSetupColumn("c2",ImGuiTableColumnFlags_WidthStretch,0.5f);

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("Song Name: %s", pendingMIDI->name.c_str());
          ImGui::TableNextColumn();
          ImGui::Text("Track Count: %d", pendingMIDI->trackCount);
          ImGui::TableNextColumn();
          if (ImGui::Button("ReImport")) {
            doAction(GUI_ACTION_MIDI_IMPORT_OPEN);
          }

          ImGui::TableNextRow();
          ImGui::TableNextColumn();

          ImGui::TableNextColumn();
          ImGui::Text("Track Tick: %d", pendingMIDI->tickSpacement);
          ImGui::SameLine();
          if (ImGui::InputInt("##TrackTick",&pendingMIDI->tickSpacement,1,1)) {
            if(pendingMIDI->tickSpacement < 0) pendingMIDI->tickSpacement = 0;             
            if(pendingMIDI->tickSpacement >= 1024) pendingMIDI->tickSpacement = 1023; 
          }
          ImGui::TableNextColumn();

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("Target Track:");
          ImGui::SameLine();
          if (ImGui::InputInt("##TargetTrack",&pendingMIDI->trackToImport,1,1)) {
            pendingMIDI->trackChannelToImport = 0;
            if(pendingMIDI->trackToImport < 0) pendingMIDI->trackToImport = 0;             
            if(pendingMIDI->trackToImport >= pendingMIDI->trackCount) pendingMIDI->trackToImport = pendingMIDI->trackCount - 1; 
          }
          ImGui::SameLine();
          ImGui::Text("%s", pendingMIDI->tracks[0][pendingMIDI->trackToImport]->instrument.c_str());
          ImGui::TableNextColumn();
          ImGui::Text("Target Channel:");
          ImGui::SameLine();
          if (ImGui::InputInt("##TargetChannel",&pendingMIDI->trackChannelToImport,1,2)) {
            if(pendingMIDI->trackChannelToImport < 0) 
              pendingMIDI->trackChannelToImport = 0;             
            if(pendingMIDI->trackChannelToImport >=pendingMIDI->tracks[0][pendingMIDI->trackToImport]->channels->size()) 
              pendingMIDI->trackChannelToImport = pendingMIDI->tracks[0][pendingMIDI->trackToImport]->channels->size() - 1; 
          }

          ImGui::TableNextColumn();
          ImGui::Text("Notes: %llu ", pendingMIDI->tracks[0][pendingMIDI->trackToImport]->channels[0][pendingMIDI->trackChannelToImport]->notes->size());

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::Text("(%llu notes in %llu channels)",
            pendingMIDI->tracks[0][pendingMIDI->trackToImport]->notes->size(),
            pendingMIDI->tracks[0][pendingMIDI->trackToImport]->channels->size()
          );
          // if (ImGui::InputInt("##SelectedSys",&pendingMIDI->selectedSys,1,2)) {
          //   if(pendingMIDI->selectedSys < 0) pendingMIDI->selectedSys = 0;             
          //   if(pendingMIDI->selectedSys >= e->song.systemLen) pendingMIDI->selectedSys = e->song.systemLen; 
          // }
          // for (int i=0; i<e->song.systemLen; i++) {
          //   song
          // }
          ImGui::TableNextColumn();
          ImGui::Text("Channel: ");
          ImGui::SameLine();

          if (ImGui::InputInt("##SelectedChan",&pendingMIDI->selectedChan,1,2)) {
            if(pendingMIDI->selectedChan < 0) 
              pendingMIDI->selectedChan = 0;             
            if(pendingMIDI->selectedChan >= e->getTotalChannelCount()) 
              pendingMIDI->selectedChan = e->getTotalChannelCount(); 
          }
          ImGui::TableNextColumn();

          ImGui::TableNextRow();
          ImGui::TableNextColumn();
          ImGui::TableNextColumn();
          if (ImGui::Button("Import")) {
            doAction(GUI_ACTION_MIDI_IMPORT_TO_CHANNEL);
          }
          ImGui::TableNextColumn();

          ImGui::EndTable();
      }
    }
  }
  if (ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows)) curWindow=GUI_WINDOW_MIDI_IMPORT;
  ImGui::End();
}
