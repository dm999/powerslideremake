#include "../pcheader.h"

#include "MultiplayerRoomInfo.h"

#include "multislider/MultiSlider.h"
#include "multislider/CommonIncludes.h"

bool MultiplayerRoomInfo::getRoomsList(const std::string& ip, size_t port, std::vector<std::string>& rooms, std::vector<std::vector<std::string> >& players)const
{

    bool res = true;

    rooms.clear();
    players.clear();

    try{

        multislider::shared_ptr<multislider::Lobby> lobby;
        lobby = multislider::shared_ptr<multislider::Lobby>(new multislider::Lobby(ip, port));

        typedef std::vector<multislider::RoomInfo> roomV;
        const roomV& roomsGet = lobby->getRooms();
        for(size_t q = 0; q < roomsGet.size(); ++q)
        {
            rooms.push_back(roomsGet[q].getName());
            players.push_back(std::vector<std::string>());

            for(size_t w = 0; w < roomsGet[q].getPlayers().size(); ++w)
            {
                players[q].push_back(roomsGet[q].getPlayers()[w]);
            }
        }

    }catch(const std::runtime_error& err)
    {
        (void)err;
        res = false;
    }

    return res;
}
