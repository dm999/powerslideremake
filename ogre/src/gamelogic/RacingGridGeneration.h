#ifndef RACINGGRIDGENERATION_H
#define RACINGGRIDGENERATION_H

#include <string>
#include <vector>

#include "../GameState.h"

class RacingGridGeneration
{
public:
    RacingGridGeneration(){}

    std::vector<std::string> generate(GameState& gameState, const std::vector<std::string>& playersCharacters, std::vector<size_t>& resAISlot) const;

private:

    std::vector<size_t> getLinearIndexes(size_t amount) const;
    std::vector<size_t> getLinearInsaneIndexes(size_t amount) const;

    std::vector<size_t> getEasyIndexes(const GameState& gameState) const;
    std::vector<size_t> getMediumIndexes(const GameState& gameState) const;
    std::vector<size_t> getHardIndexes(const GameState& gameState) const;

    size_t getSlotIndex(size_t index) const;
    size_t getSlotInsaneIndex(size_t index) const;
};

#endif