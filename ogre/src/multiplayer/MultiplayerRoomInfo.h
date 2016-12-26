#ifndef MULTIPLAYERROOMINFO_H
#define MULTIPLAYERROOMINFO_H

#include <string>
#include <vector>

class MultiplayerRoomInfo
{
public:

    MultiplayerRoomInfo(){}
    ~MultiplayerRoomInfo(){}

    bool getRoomsList(const std::string& ip, size_t port, std::vector<std::string>& rooms, std::vector<std::string>& roomsDesc, std::vector<std::pair<size_t, size_t> >& playersReserved)const;
};

#endif