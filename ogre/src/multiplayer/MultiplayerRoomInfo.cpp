#include "../pcheader.h"

#include "MultiplayerRoomInfo.h"

#include "multislider/MultiSlider.h"
#include "multislider/CommonIncludes.h"

bool MultiplayerRoomInfo::getRoomsList(const std::string& ip, size_t port, std::vector<std::string>& rooms, std::vector<std::string>& roomsDesc, std::vector<std::pair<size_t, size_t> >& players)const
{

    bool res = true;

    rooms.clear();
    roomsDesc.clear();
    players.clear();

    try{

        multislider::shared_ptr<multislider::Lobby> lobby;
        lobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        typedef std::vector<multislider::RoomInfo> roomV;
        const roomV& roomsGet = lobby->getRooms();
        for(size_t q = 0; q < roomsGet.size(); ++q)
        {
            rooms.push_back(roomsGet[q].getName());
            roomsDesc.push_back(roomsGet[q].getDescription());
            players.push_back(std::make_pair(roomsGet[q].getPlayersNumber(), roomsGet[q].getReservedPlayersNumber()));
        }

    }catch(const std::runtime_error& err)
    {
        (void)err;
        res = false;
    }

    return res;
}
