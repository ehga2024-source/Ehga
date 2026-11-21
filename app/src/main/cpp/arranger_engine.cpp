#define TSF_IMPLEMENTATION
#include "tsf.h"
#include "arranger_engine.h"

#include <algorithm>
#include <cstdio>

ArrangerEngine::ArrangerEngine() = default;

ArrangerEngine::~ArrangerEngine() {
    if (tsf_ != nullptr) {
        tsf_close(tsf_);
        tsf_ = nullptr;
    }
}

bool ArrangerEngine::initAudio(int32_t sampleRate) {
    sampleRate_ = (sampleRate > 0) ? sampleRate : 48000;
    if (tsf_ != nullptr) {
        tsf_set_output(tsf_, TSF_STEREO_INTERLEAVED, sampleRate_, 0.0f);
    }
    return true;
}

bool ArrangerEngine::loadSoundFontFile(const std::string& filePath) {
    if (filePath.empty()) {
        return false;
    }

    TSF* loaded = tsf_load_filename(filePath.c_str());
    if (loaded == nullptr) {
        return false;
    }

    if (tsf_get_preset_count(loaded) <= 0) {
        tsf_close(loaded);
        return false;
    }

    if (tsf_ != nullptr) {
        tsf_close(tsf_);
    }

    tsf_ = loaded;
    tsf_set_output(tsf_, TSF_STEREO_INTERLEAVED, sampleRate_, 0.0f);
    activePreset_ = 0;
    return true;
}

bool ArrangerEngine::loadStyleFile(const std::string& filePath) {
    FILE* fp = std::fopen(filePath.c_str(), "rb");
    if (fp == nullptr) {
        return false;
    }

    std::fseek(fp, 0, SEEK_END);
    long fileSize = std::ftell(fp);
    std::rewind(fp);

    if (fileSize <= 0) {
        std::fclose(fp);
        return false;
    }

    std::vector<uint8_t> buffer(static_cast<size_t>(fileSize));
    size_t readBytes = std::fread(buffer.data(), 1, buffer.size(), fp);
    std::fclose(fp);

    if (readBytes == 0) {
        return false;
    }

    // Reemplaza el estilo anterior en lugar de acumular múltiples estilos en memoria
    ParsedYamahaStyle nextStyle;
    nextStyle.sourceRoot = filePath;
    nextStyle.ppqn = 480;
    nextStyle.loaded = true;

    activeStyle_ = std::move(nextStyle);
    return true;
}

int32_t ArrangerEngine::getPresetCount() const {
    return (tsf_ != nullptr) ? tsf_get_preset_count(tsf_) : 0;
}

void ArrangerEngine::setPreset(int32_t presetIndex) {
    int32_t count = getPresetCount();
    if (count > 0) {
        activePreset_ = std::clamp(presetIndex, 0, count - 1);
    } else {
        activePreset_ = 0;
    }
}

void ArrangerEngine::noteOn(int32_t presetIndex, int32_t midiNote, float velocity) {
    if (tsf_ == nullptr) {
        return;
    }
    int32_t count = tsf_get_preset_count(tsf_);
    if (count <= 0) {
        return;
    }
    int32_t idx = (presetIndex >= 0 && presetIndex < count) ? presetIndex : activePreset_;
    float vel = std::clamp(velocity, 0.05f, 1.0f);
    tsf_note_on(tsf_, idx, midiNote, vel);
}

void ArrangerEngine::noteOff(int32_t presetIndex, int32_t midiNote) {
    if (tsf_ == nullptr) {
        return;
    }
    int32_t count = tsf_get_preset_count(tsf_);
    if (count <= 0) {
        return;
    }
    int32_t idx = (presetIndex >= 0 && presetIndex < count) ? presetIndex : activePreset_;
    tsf_note_off(tsf_, idx, midiNote);
}

void ArrangerEngine::renderStereoFloat(float* outInterleaved, int32_t numFrames) {
    if (outInterleaved == nullptr || numFrames <= 0) {
        return;
    }
    if (tsf_ == nullptr || tsf_get_preset_count(tsf_) <= 0) {
        std::fill(outInterleaved, outInterleaved + (numFrames * 2), 0.0f);
        return;
    }
    tsf_render_float(tsf_, outInterleaved, numFrames, 0);
}
