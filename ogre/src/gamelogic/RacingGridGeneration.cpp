
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
        aiIndexes = getMediumIndexes();
        break;
    case Hard :
        aiLinearIndexes = getLinearIndexes(gameState.getAICountInRace());
        aiIndexes = getHardIndexes();
        break;
    case Insane :
        aiLinearIndexes = getLinearInsaneIndexes(gameState.getAICountInRace());
        aiIndexes = getHardIndexes();
        break;
    default:
        aiLinearIndexes = getLinearIndexes(gameState.getAICountInRace());
        aiIndexes = getEasyIndexes();
    }
    std::vector<std::string> availableCharacters = gameState.getSTRPowerslide().getArrayValue("", "available characters");

    for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
    {
        res.push_back(availableCharacters[aiIndexes[aiLinearIndexes[gameState.getAICountInRace() - q - 1]] % 32]);
    }

    for(size_t q = 0; q < gameState.getAICountInRace(); ++q)
    {
        if(gameState.getAIStrength() == Insane || GameState::mAIMax >= GameState::mRaceGridCarsMax)
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
                std::string newCharName = availableCharacters[aiIndexes[newCharIndex] % availableCharacters.size()];
                if(newCharName != playerCharacter)
                {
                    std::vector<std::string>::const_iterator foundMore = std::find(res.begin(), res.end(), newCharName);
                    if(foundMore == res.end())
                    {
                        (*found) = newCharName;

                        size_t foundIndex = found - res.begin();
                        if(gameState.getAIStrength() == Insane || GameState::mAIMax >= GameState::mRaceGridCarsMax)
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

std::vector<size_t> RacingGridGeneration::getEasyIndexes() const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C4A0
    if (GameState::mAIMax < GameState::mRaceGridCarsMax)
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
        for (size_t q = 0; q < GameState::mAIMax; ++q)
        {
            aiIndexes.push_back(q);
        }
    }

    return aiIndexes;
}

std::vector<size_t> RacingGridGeneration::getMediumIndexes() const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C530
    if (GameState::mAIMax < GameState::mRaceGridCarsMax)
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
        for (size_t q = 0; q < GameState::mAIMax; ++q)
        {
            aiIndexes.push_back(q);
        }
    }

    return aiIndexes;
}

std::vector<size_t> RacingGridGeneration::getHardIndexes() const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C5C0                - hard
    //sub_48C650 -> sub_48C5C0  - insane
    if (GameState::mAIMax < GameState::mRaceGridCarsMax)
    {
        for (size_t q = 0; q < 10; ++q)
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
    }
    else
    {
        for (size_t q = 0; q < GameState::mAIMax; ++q)
        {
            aiIndexes.push_back(q);
        }
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
    default : ret = 0;
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
        for (size_t q = 0; q < aiAmount; ++q) aiLinearIndexes.push_back(q);
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