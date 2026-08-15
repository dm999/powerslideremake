
#include "RacingGridGeneration.h"

#include "../GameState.h"

namespace{
    //dword_4C3970
    size_t slotRemapper[]={0, 1, 5, 1, 2, 6, 4, 2, 3, 4, 7, 7, 0, 3, 8, 9, 3, 4, 9, 1, 6, 10, 11, 5, 8, 2, 7, 8, 9, 6, 5, 0};
}


std::vector<std::string> RacingGridGeneration::generate(GameState& gameState, const std::vector<std::string>& playersCharacters, std::vector<size_t>& resAISlot) const
{
    std::vector<std::string> res;
    resAISlot.clear();

    std::vector<size_t> aiLinearIndexes;
    std::vector<size_t> aiIndexes;

    switch(gameState.getAIStrength())
    {
    case Medium :
        aiLinearIndexes = getLinearIndexes(gameState.getAICountInRace());
        aiIndexes = getMediumIndexes(gameState);
        break;
    case Hard :
        aiLinearIndexes = getLinearIndexes(gameState.getAICountInRace());
        aiIndexes = getHardIndexes(gameState);
        break;
    case Insane : case UltraInsane:
        aiLinearIndexes = getLinearInsaneIndexes(gameState.getAICountInRace());
        aiIndexes = getHardIndexes(gameState);
        break;
    default:
        aiLinearIndexes = getLinearIndexes(gameState.getAICountInRace());
        aiIndexes = getEasyIndexes(gameState);
    }
    std::vector<std::string> availableCharacters = gameState.getSTRPowerslide().getArrayValue("", "available characters");

    //The "available characters" roster includes the supercar driver as its last
    //entry. The original game never lets AI drive the supercar, so drop it from
    //the AI pool. The normal-mode roster permutations only index [0,31], so this
    //is a no-op for them; for massacre the field is built from an identity 0..N-1
    //index map, so filtering keeps the supercar out of the batched AI launch
    //(matches fun's `% 32 //not include supercar`, without the hardcoded 32).
    std::vector<std::string> aiCharactersPool;
    aiCharactersPool.reserve(availableCharacters.size());
    for(size_t q = 0; q < availableCharacters.size(); ++q)
    {
        if(gameState.getSTRPowerslide().getCarFromCharacter(availableCharacters[q]) != "supercar")
            aiCharactersPool.push_back(availableCharacters[q]);
    }
    if(aiCharactersPool.empty()) aiCharactersPool = availableCharacters;//safety fallback

    //massacre builds a field larger than the roster, so index into the pool
    //modulo its size to stay in bounds.
    for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
    {
        res.push_back(aiCharactersPool[aiIndexes[aiLinearIndexes[gameState.getAICountInRace() - q - 1]] % aiCharactersPool.size()]);
    }

    //slot index must stay in [0, mRaceGridCarsMax) so the 12 AI neural-net
    //datasets (loaded per slot by AILoader) are shared across all cars. For
    //fields >= mRaceGridCarsMax use the insane slot mapper (its default wraps
    //mod mRaceGridCarsMax), regardless of difficulty.
    const bool useInsaneSlots = gameState.getAIStrength() == Insane
        || gameState.getAIStrength() == UltraInsane
        || gameState.getAICountInRace() >= GameState::mRaceGridCarsMax;

    for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
    {
        if(useInsaneSlots)
            resAISlot.push_back(getSlotInsaneIndex(aiIndexes[aiLinearIndexes[gameState.getAICountInRace() - q - 1]]));
        else
            resAISlot.push_back(getSlotIndex(aiIndexes[aiLinearIndexes[gameState.getAICountInRace() - q - 1]]));
    }

    //solve collision
    for(size_t q = 0; q < playersCharacters.size(); ++q)
    {
        std::string playerCharacter = playersCharacters[q];

        std::vector<std::string>::iterator found = std::find(res.begin(), res.end(), playerCharacter);
        if(found != res.end())
        {
            size_t newCharIndex = 0;
            while(newCharIndex < GameState::mRaceGridCarsMax)
            {
                std::string newCharName = aiCharactersPool[aiIndexes[newCharIndex] % aiCharactersPool.size()];
                if(newCharName != playerCharacter)
                {
                    std::vector<std::string>::const_iterator foundMore = std::find(res.begin(), res.end(), newCharName);
                    if(foundMore == res.end())
                    {
                        (*found) = newCharName;

                        size_t foundIndex = found - res.begin();
                        if(useInsaneSlots)
                            resAISlot[foundIndex] = getSlotInsaneIndex(aiIndexes[newCharIndex]);
                        else
                            resAISlot[foundIndex] = getSlotIndex(aiIndexes[newCharIndex]);

                        break;
                    }
                }

                ++newCharIndex;
            }
        }
    }

    return res;
}

std::vector<size_t> RacingGridGeneration::getEasyIndexes(const GameState& gameState) const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C4A0
    if(gameState.getAICountInRace() < GameState::mRaceGridCarsMax)
    {
        aiIndexes.push_back(8);
        aiIndexes.push_back(20);
        aiIndexes.push_back(10);
        aiIndexes.push_back(9);
        aiIndexes.push_back(14);
        aiIndexes.push_back(15);
        aiIndexes.push_back(2);
        aiIndexes.push_back(21);
        aiIndexes.push_back(22);
        aiIndexes.push_back(1);
        aiIndexes.push_back(0);
        aiIndexes.push_back(7);
    }
    else
    {
        //massacre: more cars than the 12-entry roster permutation, so just emit
        //a 0..N-1 identity mapping (characters are picked modulo roster size).
        for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
            aiIndexes.push_back(q);
    }

    return aiIndexes;
}

std::vector<size_t> RacingGridGeneration::getMediumIndexes(const GameState& gameState) const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C530
    if(gameState.getAICountInRace() < GameState::mRaceGridCarsMax)
    {
        aiIndexes.push_back(16);
        aiIndexes.push_back(5);
        aiIndexes.push_back(3);
        aiIndexes.push_back(11);
        aiIndexes.push_back(24);
        aiIndexes.push_back(18);
        aiIndexes.push_back(4);
        aiIndexes.push_back(17);
        aiIndexes.push_back(23);
        aiIndexes.push_back(12);
        aiIndexes.push_back(8);
        aiIndexes.push_back(20);
    }
    else
    {
        for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
            aiIndexes.push_back(q);
    }

    return aiIndexes;
}

std::vector<size_t> RacingGridGeneration::getHardIndexes(const GameState& gameState) const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C5C0                - hard
    //sub_48C650 -> sub_48C5C0  - insane
    if(gameState.getAICountInRace() < GameState::mRaceGridCarsMax)
    {
        aiIndexes.push_back(29);
        aiIndexes.push_back(26);
        aiIndexes.push_back(6);
        aiIndexes.push_back(13);
        aiIndexes.push_back(27);
        aiIndexes.push_back(25);
        aiIndexes.push_back(28);
        aiIndexes.push_back(19);
        aiIndexes.push_back(30);
        aiIndexes.push_back(31);
        aiIndexes.push_back(16);
        aiIndexes.push_back(5);
    }
    else
    {
        for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
            aiIndexes.push_back(q);
    }

    return aiIndexes;
}

size_t RacingGridGeneration::getSlotIndex(size_t index) const
{
    return slotRemapper[index];
}

size_t RacingGridGeneration::getSlotInsaneIndex(size_t index) const
{
    size_t ret = 0;

    //sub_40F2A0
    switch(index)
    {
    case 29: ret = 6; break;
    case 26: ret = 7; break;
    case 6: ret = 4; break;
    case 13: ret = 3; break;
    case 27: ret = 8; break;
    case 25: ret = 2; break;
    case 28: ret = 9; break;
    case 19: ret = 1; break;
    case 30: ret = 5; break;
    case 16: ret = 10; break;
    case 5: ret = 11; break;
    default : ret = index % GameState::mRaceGridCarsMax; //massacre: wrap large indices into the 12 AI slots
    }

    return ret;
}

std::vector<size_t> RacingGridGeneration::getLinearIndexes(size_t aiAmount) const
{
    std::vector<size_t> aiLinearIndexes;

    switch(aiAmount)
    {
    case 3:
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(10);
        break;
    case 4:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(10);
        break;
    case 5:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(11);
        break;
    case 6:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(10);
        aiLinearIndexes.push_back(11);
        break;
    case 7:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(8);
        aiLinearIndexes.push_back(10);
        break;
    case 8:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(10);
        aiLinearIndexes.push_back(11);
        break;
    case 9:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(9);
        aiLinearIndexes.push_back(11);
        break;
    case 10:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(8);
        aiLinearIndexes.push_back(9);
        aiLinearIndexes.push_back(11);
        break;
    case 11:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(8);
        aiLinearIndexes.push_back(9);
        aiLinearIndexes.push_back(10);
        break;
    default:
        //massacre: aiAmount > 11, emit 0..aiAmount-1 (no fixed permutation).
        for(size_t q = 0; q < aiAmount; ++q) aiLinearIndexes.push_back(q);
    }

    return aiLinearIndexes;
}

std::vector<size_t> RacingGridGeneration::getLinearInsaneIndexes(size_t aiAmount) const
{
    std::vector<size_t> aiLinearIndexes;

    switch(aiAmount)
    {
    case 3:
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(8);
        break;
    case 4:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(8);
        break;
    case 5:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(8);
        break;
    case 6:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(9);
        break;
    case 7:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(9);
        break;
    case 8:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(8);
        break;
    case 9:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(8);
        aiLinearIndexes.push_back(9);
        break;
    case 10:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(8);
        aiLinearIndexes.push_back(10);
        break;
    case 11:
        aiLinearIndexes.push_back(0);
        aiLinearIndexes.push_back(1);
        aiLinearIndexes.push_back(2);
        aiLinearIndexes.push_back(3);
        aiLinearIndexes.push_back(4);
        aiLinearIndexes.push_back(5);
        aiLinearIndexes.push_back(6);
        aiLinearIndexes.push_back(7);
        aiLinearIndexes.push_back(8);
        aiLinearIndexes.push_back(9);
        aiLinearIndexes.push_back(10);
        break;
    default:
        //massacre: aiAmount > 11, emit 0..aiAmount-1 (no fixed permutation).
        for(size_t q = 0; q < aiAmount; ++q) aiLinearIndexes.push_back(q);
    }

    return aiLinearIndexes;
}

/*
//easy/medium/hard
dd 0//3ai
dd 4
dd 9
dd 0
dd 1//
dd 3
dd 0Ah
dd 0

dd 0//4ai
dd 1
dd 4
dd 0Bh
dd 0//
dd 2
dd 5
dd 0Ah

dd 0//5ai
dd 1
dd 4
dd 7
dd 0Bh
dd 0
dd 0//
dd 1
dd 5
dd 6
dd 0Bh
dd 0

dd 0//6ai
dd 1
dd 3
dd 7
dd 9
dd 0Bh
dd 0//
dd 1
dd 4
dd 6
dd 0Ah
dd 0Bh

dd 0//7ai
dd 1
dd 2
dd 4
dd 6
dd 9
dd 0Bh
dd 0
dd 0//
dd 1
dd 2
dd 5
dd 7
dd 8
dd 0Ah
dd 0

dd 0//8ai
dd 1
dd 2
dd 3
dd 4
dd 6
dd 9
dd 0Bh
dd 0//
dd 1
dd 2
dd 4
dd 5
dd 7
dd 0Ah
dd 0Bh

dd 0//9ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 8
dd 0Ah
dd 0
dd 0//
dd 1
dd 2
dd 3
dd 4
dd 5
dd 7
dd 9
dd 0Bh
dd 0

dd 0//10ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 7
dd 0Ah
dd 0Bh
dd 0//
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 8
dd 9
dd 0Bh

dd 0//11ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 7
dd 8
dd 9
dd 0Ah
dd 0



//insane
dd 0//3 AI
dd 4
dd 9
dd 0
dd 1//
dd 3
dd 8
dd 0

dd 0//4ai
dd 1
dd 4
dd 9
dd 0//
dd 2
dd 5
dd 8

dd 0//5ai
dd 1
dd 4
dd 7
dd 9
dd 0
dd 0//
dd 1
dd 5
dd 6
dd 8
dd 0

dd 0//6ai
dd 1
dd 3
dd 5
dd 7
dd 8
dd 0//
dd 1
dd 4
dd 6
dd 7
dd 9

dd 0//7ai
dd 1
dd 2
dd 3
dd 6
dd 8
dd 9
dd 0
dd 0//
dd 1
dd 2
dd 4
dd 5
dd 7
dd 9
dd 0

dd 0//8ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 8
dd 9
dd 0//
dd 1
dd 2
dd 4
dd 5
dd 6
dd 7
dd 9

dd 0//9ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 7
dd 8
dd 0
dd 0//
dd 1
dd 2
dd 3
dd 4
dd 5
dd 7
dd 8
dd 9
dd 0

dd 0//10ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 7
dd 8
dd 9
dd 0//
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 7
dd 8
dd 0Ah

dd 0//11 ai
dd 1
dd 2
dd 3
dd 4
dd 5
dd 6
dd 7
dd 8
dd 9
dd 0Ah
dd 0

*/