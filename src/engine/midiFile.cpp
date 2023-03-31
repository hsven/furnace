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

#include "engine.h"
#include "song.h"
#include "../ta-log.h"
#include "dataErrors.h"
#include "midiFile.h"
#include "../gui/gui.h"
#include <vector>
#include <algorithm>

// Table to convert MIDI note numbers into decodeable names
const char *intToNote[132] = {
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-0", "C#0", "D-0", "D#0", "E-0", "F-0", "F#0", "G-0", "G#0", "A-0", "A#0", "B-0",
    "C-1", "C#1", "D-1", "D#1", "E-1", "F-1", "F#1", "G-1", "G#1", "A-1", "A#1", "B-1",
    "C-2", "C#2", "D-2", "D#2", "E-2", "F-2", "F#2", "G-2", "G#2", "A-2", "A#2", "B-2",
    "C-3", "C#3", "D-3", "D#3", "E-3", "F-3", "F#3", "G-3", "G#3", "A-3", "A#3", "B-3",
    "C-4", "C#4", "D-4", "D#4", "E-4", "F-4", "F#4", "G-4", "G#4", "A-4", "A#4", "B-4",
    "C-5", "C#5", "D-5", "D#5", "E-5", "F-5", "F#5", "G-5", "G#5", "A-5", "A#5", "B-5",
    "C-6", "C#6", "D-6", "D#6", "E-6", "F-6", "F#6", "G-6", "G#6", "A-6", "A#6", "B-6",
    "C-7", "C#7", "D-7", "D#7", "E-7", "F-7", "F#7", "G-7", "G#7", "A-7", "A#7", "B-7",
    "C-8", "C#8", "D-8", "D#8", "E-8", "F-8", "F#8", "G-8", "G#8", "A-8", "A#8", "B-8",
    "C-9", "C#9", "D-9", "D#9", "E-9", "F-9", "F#9", "G-9", "G#9", "A-9", "A#9", "B-9"};

bool inRange(int low, int high, int x, bool isInclusive)         
{         
  if(isInclusive) return (low <= x && x <= high);         
  else return (low <= x && x < high);         
}  

// bool isNoteInRange(MIDINote prevNote, MIDINote nextNote) {
//   if(inRange(prevNote.adjustedStartTick, prevNote.adjustedEndTick, nextNote.adjustedStartTick)) return true;

//   bool 
//   return false;
// }

void DivMIDI::evaluateTracks(int patternLength) {
  smf::MidiFile fileObj = *this->file;


  fileObj.doTimeAnalysis();
  fileObj.linkNotePairs();

  this->fileTickSize = fileObj.getFileDurationInTicks();
  this->tickSpacement = fileObj.getTicksPerQuarterNote() / 4;
  // std::vector<int> activeNoteVec;
  // size_t maxQueueSize = 0;

  for (int track = 0; track < fileObj.getTrackCount() - 1; track++) {
    MIDITrack* trackObj = new MIDITrack;
    // trackObj->instrument = fileObj.getGMInstrumentName(track).c_str();
    trackObj->notes = new std::vector<MIDINote *>;
    // std::vector<MIDINote *> *noteVec = ;
    // trackVec->push_back
    // logW("Track: %d", track);

    for (int event = 0; event < fileObj[track].size() - 1; event++) {
      int tick = fileObj[track][event].tick;
      int tickDur = fileObj[track][event].getTickDuration();
      int noteNumber = fileObj[track][event].getKeyNumber();

      if (fileObj[track][event].isPatchChange()) {
        trackObj->instrument = fileObj.getGMInstrumentName(fileObj[track][event].getP1());
      }

      if (fileObj[track][event].isNoteOn()) {
        if (this->excludeNotesSmallerThanTick && tickDur < this->tickSpacement) {
          continue;  
        }
        
        trackObj->notes->push_back(new MIDINote(noteNumber, intToNote[noteNumber], tick, tickDur, this->tickSpacement, patternLength));
      }
    }
    this->tracks->push_back(trackObj);
    // logW("Max Notes in Track: %d", maxQueueSize);
  }
}

bool DivEngine::importToChannel(DivMIDI* midi) {
  auto trackToImport = (*midi->tracks)[midi->trackToImport];

  auto selectedChannel = (*trackToImport->channels)[midi->trackChannelToImport];
  // auto note = tempChannel->notes[0][0];
  DivChannelData* channel = &song.subsong[0]->pat[midi->selectedChan];
  int patternLength = song.subsong[0]->patLen;
  // stompChannel(midi->selectedChan);

  int patternCount = (midi->fileTickSize / midi->tickSpacement) / patternLength;
  
  for (int j = 0; j < patternCount; j++)
  {
    if (channel->data[j]==NULL) {
      addOrder(curOrder, false, true);

      for (int i = 0; i < getTotalChannelCount(); i++)
      {
        auto tempChan = &song.subsong[0]->pat[i];
        // for (int w=0; w<chans; w++) {
          tempChan->getPattern(curOrders->ord[i][j], true);
        // }

        // patCache[i]=e->curPat[i].getPattern(e->curOrders->ord[i][ord+1],true);
      }
    }
  }

  // for (int i = 0; i < patternCount; i++)
  // {
  //   if (channel->data[i]==NULL) {
  //     addOrder(false, true);
  //   }
  //   for (int j=0; j<chans; j++) {
  //     channel->getPattern(curOrders->ord[j][i], true);
  //   }

  //   // patCache[i]=e->curPat[i].getPattern(e->curOrders->ord[i][ord+1],true);
  // }
  

  // channel.wipePatterns();
  for (auto note : *selectedChannel->notes)
  {
    short noteNumber = 0;
    short octave = 0;
    if(FurnaceGUI::decodeNote(note->noteCode, noteNumber, octave)) {
      // int ord = curOrders
      int currentPattern = (note->adjustedStartTick  / midi->tickSpacement) / patternLength;
      int dataRow = (note->adjustedStartTick / midi->tickSpacement) % patternLength;

      if (currentPattern >= DIV_MAX_PATTERNS) {
        return false;
      }

      DivPattern* pat=channel->getPattern(currentPattern,false);

      pat->data[dataRow][0] = noteNumber;
      pat->data[dataRow][1] = octave;


      //End notes
      currentPattern = (note->adjustedEndTick / midi->tickSpacement) / patternLength;
      dataRow = ((note->adjustedEndTick / midi->tickSpacement)) % patternLength;
      pat=channel->getPattern(currentPattern,false);

      pat->data[dataRow][0] = 100;
      pat->data[dataRow][1] = 0;
    }
  }

  return false; 
}

void MIDITrack::splitNotes() {
  this->channels = new std::vector<MIDIChannel*>;
  // int currentTick = 0;
  // int currentEndTick = 0;


  // while (!this->notes->empty())
  // {
    // MIDINote* currentNote = NULL;
    // MIDIChannel* chan = new MIDIChannel;
    std::vector<MIDINote*> currentNotes = std::vector<MIDINote*>();
    std::vector<MIDIChannel*> chans = std::vector<MIDIChannel*>({new MIDIChannel});
    // chans[0]->notes = new std::vector<MIDINote*>;

    for (auto note : *this->notes)
    {
      if(currentNotes.empty()) {
        currentNotes.push_back(note);
        // currentNote = note;
        chans[0]->notes->push_back(note);
        // this->notes->erase(this->notes->begin() + pos);
        continue;
      }
      else {
        int channelPos = 0;
        bool isNoteInRange = false;
        for (auto&& curNote : currentNotes)
        {
          if(!inRange(curNote->startPattern, curNote->endPattern, note->startPattern, true)) {
            curNote = note;
            chans[channelPos]->notes->push_back(note);
            // this->notes->erase(this->notes->begin() + pos);
            isNoteInRange = true;
            break;
          }
          else if(!inRange(curNote->startRow, curNote->endRow, note->startRow, false)) {
            curNote = note;
            chans[channelPos]->notes->push_back(note);
            // this->notes->erase(this->notes->begin() + pos);
            isNoteInRange = true;
            break;
          }
          channelPos++;
        }
        if (isNoteInRange) continue;

        chans.push_back(new MIDIChannel);
        currentNotes.push_back(note);
        chans[channelPos]->notes->push_back(note);
      }
    }
    for (auto chan : chans)
    {
      this->channels->push_back(chan);
    }
    
  // }
  

  int num = 0;
  for (auto channel : *this->channels)
  {
    logW("--------CHANNEL: %d ---------", num++);    

    for (auto note : *channel->notes)
    {
      logW("Note: %s -- Start=%d / End=%d", note->noteCode, note->adjustedStartTick, note->adjustedEndTick);    
    }
  }
  
  
  logW("Channel Count: %d", this->channels->size());

}

