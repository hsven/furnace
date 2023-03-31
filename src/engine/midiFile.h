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

#ifndef _MIDI_H
#define _MIDI_H

#include "engine.h"
#include "../ta-utils.h"
#include "../ta-log.h"
#include "defines.h"
#include "safeWriter.h"
#include "dataErrors.h"
#include <vector>
#include <MidiFile.h>
#include "Options.h"

// const int MIN_tickSpacement = 32;

struct MIDINote {
    int noteNumber;
    const char* noteCode;
    int startTick;
    int endTick;
    int tickDuration;
    int adjustedStartTick;
    int adjustedEndTick;

    int startPattern;
    int startRow;
    int endPattern;
    int endRow;

    MIDINote(int num, const char* code, int start, int duration, int tickSpacement, int patternLength) {
        noteNumber = num;
        noteCode = code;
        tickDuration= duration;

        startTick = adjustedStartTick = start;
        endTick = adjustedEndTick = start + duration;
        
        startPattern = (adjustedStartTick  / tickSpacement) / patternLength;
        startRow = (adjustedStartTick / tickSpacement) % patternLength;
        
        endPattern = (adjustedEndTick / tickSpacement) / patternLength;
        endRow = ((adjustedEndTick / tickSpacement)) % patternLength;
        // int remain = start % tickSpacement;
        // if (remain != 0) {
        //     adjustedStartTick = start - remain;
        //     if (remain < 15)
        //         adjustedStartTick = start - remain;
        //     else
        //         adjustedStartTick = start - remain + tickSpacement;

        //     adjustedEndTick = adjustedStartTick + duration;
        // }
        // remain = adjustedEndTick % tickSpacement;
        // if (remain != 0) {
        //     adjustedEndTick = endTick + (tickSpacement - remain);
        //     if (remain < 15)
        //         adjustedEndTick = adjustedEndTick - remain;
        //     else
        //         adjustedEndTick = adjustedEndTick - remain + tickSpacement;
        // }
    }
};

//Part of a track, contains one single, non-overlapping string of notes
struct MIDIChannel {
    std::vector<MIDINote*>* notes;

    MIDIChannel() {
        notes = new std::vector<MIDINote*>;
    }
};

struct MIDITrack {
    String instrument = "";
    //Total string of notes
    std::vector<MIDINote*>* notes;
    std::vector<MIDIChannel*>* channels;

    void splitNotes();
};

struct DivMIDI {
    String name;
    int trackCount;
    smf::MidiFile* file;
    std::vector<MIDITrack*>* tracks;

    int trackToImport;
    int trackChannelToImport;
    int selectedSys;
    int selectedChan;
    int tickSpacement;
    bool excludeNotesSmallerThanTick;

    int fileTickSize;
    DivMIDI() {
        trackToImport = 0;
        trackChannelToImport = 0;
        selectedSys = 0;
        selectedChan = 0;
        fileTickSize = 0;
        tickSpacement = 24;
        excludeNotesSmallerThanTick = false;
        tracks = new std::vector<MIDITrack*>;
    }

    DivMIDI(smf::MidiFile* midiFile) : DivMIDI() {
        file = midiFile;
        name = midiFile->getFilename();
        trackCount = midiFile->getTrackCount();

        evaluateTracks(64);

        // int num = 0;
        // auto i = tracks[0][0];
        // logW("-------------TRACK %d-------------", num++);
        // i->splitNotes();
        
        for (auto i : *tracks)
        {
            i->splitNotes();
            // for (auto j : *i->notes)
            // {
            //  logW("Note: %s -- Start=%d / End=%d", j->noteCode, j->adjustedStartTick, j->adjustedEndTick);            
            // }   
        }
    }

    void evaluateTracks(int patternLength);
};



#endif