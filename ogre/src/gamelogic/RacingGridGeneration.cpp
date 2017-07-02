
#include "RacingGridGeneration.h"

std::vector<std::string> RacingGridGeneration::generate(GameState& gameState, const std::vector<std::string>& playersCharacters) const
{
    std::vector<std::string> res;

    std::vector<size_t> aiIndexes;

    switch(gameState.getAIStrength())
    {
    case Medium :
        //aiIndexes = gameState.getSTRPowerslide().getArrayValue("", "medium racin characters ids");
        aiIndexes = getMediumIndexes();
        break;
    case Hard :
        //aiIndexes = gameState.getSTRPowerslide().getArrayValue("", "hard racin characters ids");
        aiIndexes = getHardIndexes();
        break;
    case Insane :
        //aiIndexes = gameState.getSTRPowerslide().getArrayValue("", "evolve racin characters ids");
        aiIndexes = getHardIndexes();
        break;
    default:
        //aiIndexes = gameState.getSTRPowerslide().getArrayValue("", "easy racin characters ids");
        aiIndexes = getEasyIndexes();
    }
    std::vector<std::string> availableCharacters = gameState.getSTRPowerslide().getArrayValue("", "available characters");

    for(size_t q = 0; q < gameState.getAICount(); ++q)
    {
        res.push_back(availableCharacters[aiIndexes[gameState.getAICount() - q - 1]]);
    }

    //solve collision
    for(size_t q = 0; q < playersCharacters.size(); ++q)
    {
        std::string playerCharacter = playersCharacters[q];

        std::vector<std::string>::iterator found = std::find(res.begin(), res.end(), playerCharacter);
        if(found != res.end())
        {
            size_t newCharIndex = 0;
            while(newCharIndex < 12)
            {
                std::string newCharName = availableCharacters[aiIndexes[newCharIndex]];
                if(newCharName != playerCharacter)
                {
                    std::vector<std::string>::const_iterator foundMore = std::find(res.begin(), res.end(), newCharName);
                    if(foundMore == res.end())
                    {
                        (*found) = newCharName;
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

    return aiIndexes;
}

std::vector<size_t> RacingGridGeneration::getMediumIndexes() const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C530
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

    return aiIndexes;
}

std::vector<size_t> RacingGridGeneration::getHardIndexes() const
{
    std::vector<size_t> aiIndexes;

    //from original 'random' generator
    //sub_48C5C0                - hard
    //sub_48C650 -> sub_48C5C0  - insane
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

    return aiIndexes;
}