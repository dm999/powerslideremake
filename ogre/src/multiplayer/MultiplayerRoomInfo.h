#ifndef MULTIPLAYERROOMINFO_H
#define MULTIPLAYERROOMINFO_H

#include <string>
#include <vector>

class MultiplayerRoomInfo
{
public:

    MultiplayerRoomInfo(){}
    ~MultiplayerRoomInfo(){}

    bool getRoomsList(const std::string& ip, size_t port, std::vector<std::string>& rooms, std::vector<std::vector<std::string> >& players)const;
};

#endif