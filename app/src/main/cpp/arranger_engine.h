#ifndef EHG_ARRANGER_ENGINE_H
#define EHG_ARRANGER_ENGINE_H

#include <cstdint>
#include <string>
#include <vector>

struct TSF;

struct StyleSectionRange {
    std::string name;
    int32_t startTick = 0;
    int32_t endTick = 0;
};

struct CasmChannelRule {
    int32_t channel = 0;
    int32_t bank = 0;
    int32_t program = 0;
    bool isDrum = false;
    bool transposeEnabled = true;
};

struct ParsedYamahaStyle {
    std::string title;
    std::string sourceRoot;
    int32_t ppqn = 480;
    std::vector<StyleSectionRange> sections;
    std::vector<CasmChannelRule> channelRules;
    bool loaded = false;
};

class ArrangerEngine {
public:
    ArrangerEngine();
    ~ArrangerEngine();

    bool initAudio(int32_t sampleRate);
    bool loadSoundFontFile(const std::string& filePath);
    bool loadStyleFile(const std::string& filePath);

    int32_t getPresetCount() const;
    void setPreset(int32_t presetIndex);
    void noteOn(int32_t presetIndex, int32_t midiNote, float velocity);
    void noteOff(int32_t presetIndex, int32_t midiNote);
    void renderStereoFloat(float* outInterleaved, int32_t numFrames);

    const ParsedYamahaStyle& getActiveStyle() const { return activeStyle_; }

private:
    TSF* tsf_ = nullptr;
    int32_t sampleRate_ = 48000;
    int32_t activePreset_ = 0;
    ParsedYamahaStyle activeStyle_;
};

#endif // EHG_ARRANGER_ENGINE_H
