
#include "MultiplayerRoomInfo.h"

#include "MultiSlider.h"
#include "CommonIncludes.h"

#if defined(__ANDROID__)
    #include <android/log.h>

    #define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "OGRE", __VA_ARGS__))
    #define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "OGRE", __VA_ARGS__)) 
#endif

bool MultiplayerRoomInfo::getRoomsList(const std::string& ip, size_t port, std::vector<std::string>& rooms, std::vector<std::string>& roomsDesc, std::vector<std::string>& roomHostName, std::vector<std::pair<size_t, size_t> >& players, std::vector<std::vector<std::string>>& playersNamesInRooms)const
{

    bool res = true;

    rooms.clear();
    roomsDesc.clear();
    players.clear();
    roomHostName.clear();
    playersNamesInRooms.clear();

    try{

        multislider::shared_ptr<multislider::Lobby> lobby;
        lobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        typedef std::vector<multislider::RoomInfo> roomV;
        const roomV& roomsGet = lobby->getRooms();
        for(size_t q = 0; q < roomsGet.size(); ++q)
        {
            rooms.push_back(roomsGet[q].getName());
            roomsDesc.push_back(roomsGet[q].getDescription());
            roomHostName.push_back(roomsGet[q].getHostName());
            players.push_back(std::make_pair(roomsGet[q].getPlayersNumber(), roomsGet[q].getReservedPlayersNumber()));
            playersNamesInRooms.push_back(roomsGet[q].getPlayers());//not filled
        }

    }catch(const std::runtime_error& err)
    {
        (void)err;
        res = false;

#if defined(__ANDROID__)
        LOGE("MultiplayerRoomInfo[getRoomsList]: error - %s", err.what()); 
#endif
    }

    return res;
}
