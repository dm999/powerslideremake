/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_LOBBY_H_
#define _MULTI_SLIDER_LOBBY_H_

#include <vector>

#include "LobbyCallback.h"
#include "RoomInfo.h"

namespace multislider
{
    class UdpInterface;
    class TcpInterface;

    class Lobby
    {
    public:
        typedef LobbyCallback Callback;

        enum Status
        {
            SUCCESS,        ///< Returned on success operation
            FAIL,           ///< Returned on failed operation; no specific fail reason
            ROOM_IS_FULL,   ///< Trying to join a full room
            NAME_EXISTS,    ///< Player with such name is already in the room
            ROOM_EXISTS     ///< Trying to create a room with existing name
        };

        //-------------------------------------------------------

    private:
        shared_ptr<TcpInterface> mTcpInterface;

        std::string mServerIp;
        uint16_t mServerPort;

        RoomInfo mMyRoom;
        std::string mPlayerName;
        LobbyCallback* mCallback;
        bool mIsJoined;
        bool mIsHost;

        //-------------------------------------------------------

        jsonxx::Object makeEnvelop(const jsonxx::Object & obj) const;

        //void sendTcpMessage(const std::string & message) const;

        Lobby(const Lobby &);
        Lobby & operator=(const Lobby &);
        //-------------------------------------------------------

    public:
        
        Lobby(const std::string & serverIp, uint16_t serverPort);

        
        ~Lobby();

        /**
         *  Create new room on the server
         *  @param playerName name of the current player. It will be assigned as host of the created room
         *  @param roomName name of the room
         *  @param description description of the room
         *  @param playersLimit maximum number of players in the room (should be >= 1)
         *  @param playersReserved number of reserved players slots in the room (0 <= playersReserved < playersLimit)
         *  @param callback callback for host events, can't be null
         */
        
        Status createRoom(const std::string & playerName, const std::string & roomName, const std::string & description, uint32_t playersLimit, uint32_t playersReserved, LobbyCallback* callback);

        /**
         *  Create new room on the server
         *  @param playerName name of the current player. It will be assigned as host of the created room
         *  @param roomName name of the room
         *  @param description description of the room
         *  @param playersLimit maximum number of players in the room (should be >= 1)
         *  @param playersReserved number of reserved players slots in the room (0 <= playersReserved < playersLimit)
         *  @param callback callback for host events, can't be null
         *  @param userParameter arbitrary user data
         */
        
        Status createRoom(const std::string & playerName, const std::string & roomName, const std::string & description, uint32_t playersLimit, uint32_t playersReserved, const std::string & userParameter, LobbyCallback* callback);
         
        /**
         * Change room parameters
         * In the case of success all players get onRoomUpdate callback with the new parameters
         * In the case of fail only host gets onRoomUpdate callback with the flag FLAG_RECONFIGURE_FAIL
         * @param playersLimit new players limit (should be >= 1)
         * @param new number of reserved players (0 <= playersReserved < playersLimit)
         */
        
        void reconfigure(uint32_t playersLimit, uint32_t playersReserved);

        /**
         *  Get a list of all opened rooms on the server
         */
        
        static std::vector<RoomInfo> getRooms(const std::string & serverIp, uint16_t serverPort);

        /**
         *  Join a room
         *  @param playerName name of the current player
         *  @param room reference to the room info to join
         *  @param callback callback for client events, can't be null
         */
        
        Status joinRoom(const std::string & playerName, const RoomInfo & room, LobbyCallback* callback);

        //-------------------------------------------------------
        // Host specific functions

        /**
         *  Eject player from the room
         */
        
        void eject(const std::string & playerName);

        /**
         *  Close the current room
         */
        
        void closeRoom();

        /**
         *  Start game session for all joined players
         */
        
        void startSession();

        /**
         *  Start game session for all joined players
         *  @param sessionData user data passed to all players on session start
         */
        
        void startSession(const std::string & sessionData);

        //-------------------------------------------------------
        // Common functions for both host and client

        /**
         *  Leave the current room
         *  After leaving Client instance can't be reused for another room
         */
        
        void leaveRoom();

        /**
         *  Send message to all players
         *  @param message User's message
         *  @param toSelf Send message to self as well
         */
        
        void say(const std::string & message, bool toSelf);

        /**
         *  Check incoming broadcast messages and call callback for the each message
         *  @return number of processed messages
         */
        
        uint32_t receive();

        
        size_t getLastPing()const;

        
        void pollPing();

        //-------------------------------------------------------

        /**
         *  Return true if player is joined a room
         */
        bool isJoined() const
        {
            return mIsJoined;
        }

        /**
         *  Return true if player is host in the current room
         *  Is valid only if isJoined() returned true
         */
        bool isHost() const
        {
            return mIsHost;
        }

        /**
         * Get name of the current player
         */
        const std::string & getPlayerName() const 
        {
            return mPlayerName;
        }

        /**
         *  Get current room state
         */
        const RoomInfo & getRoomInfo() const
        {
            return mMyRoom;
        }
    };

}

#endif
