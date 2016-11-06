/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_LOBBY_CALLBACK_H_
#define _MULTI_SLIDER_LOBBY_CALLBACK_H_

#include "CommonIncludes.h"

namespace multislider
{

    class LobbyCallback
    {
    public:
        MULTISLIDER_EXPORT static const uint8_t FLAG_IS_EJECTED;               ///< The player was ejected from the room
        MULTISLIDER_EXPORT static const uint8_t FLAG_JOINED;                   ///< Somebody joined the room
        MULTISLIDER_EXPORT static const uint8_t FLAG_LEFT;                     ///< Somebody left the room
        MULTISLIDER_EXPORT static const uint8_t FLAG_NEW_HOST;                 ///< Room has a new host
        MULTISLIDER_EXPORT static const uint8_t FLAG_RECONFIGURED_BY_HOST;     ///< Room has been reconfigured by host
        MULTISLIDER_EXPORT static const uint8_t FLAG_RECONFIGURE_FAIL;         ///< Couldn't change room parameters
        MULTISLIDER_EXPORT static const uint8_t FLAG_ROOM_CLOSED_BY_HOST;      ///< The player was ejected because the room was closed by host

        virtual ~LobbyCallback() 
        { }

        /**
         *  Player has joined the room
         *  Is called immediately for host after creating room and for client after join request
         *  @param lobby This lobby pointer
         *  @param room This room info
         */
        virtual void onJoined(Lobby* /*lobby*/, const RoomInfo & /*room*/)
        { }

        /**
         *  Player left the room
         *  @param lobby This lobby pointer
         *  @param room This room info
         *  @param flags May be FLAG_ROOM_CLOSED_BY_HOST or FLAG_IS_EJECTED
         */
        virtual void onLeft(Lobby* /*lobby*/, const RoomInfo & /*room*/, uint8_t /*flags*/)
        { }

        /**
         *  Is called for each update of the room state
         *  @param lobby This lobby pointer
         *  @param room This room info
         *  @param sender Name of the player who sent this broadcast. Or name of joined/left player if flags variable contains FLAG_JOINED/FLAG_LEFT respectively
         *  @param flags May be FLAG_JOINED or FLAG_LEFT, if somebody has joined or left the room. FLAG_RECONFIGURED_BY_HOST is host has changed room parameters
         *               FLAG_RECONFIGURE_FAIL is set for the host if reconfigure operation failed
         */
        virtual void onRoomUpdate(Lobby* /*lobby*/, const RoomInfo & /*room*/, const std::string & /*sender*/, uint8_t /*flags*/)
        { }

        /**
         *  Is called for each message from say()
         *  @param lobby This lobby pointer
         *  @param room This room info
         *  @param sender Name of the player who sent this broadcast. Or name of joined/left player if flags variable contains FLAG_JOINED/FLAG_LEFT respectively
         *  @param message User message
         */
        virtual void onMessage(Lobby* /*lobby*/, const RoomInfo & /*room*/, const std::string & /*sender*/, const std::string & /*message*/)
        { }

        /**
         *  Is called as soon as a host has started a session
         *  @param lobby This lobby pointer
         *  @param room This room info
         *  @param session Pointer to the newly created session
         *  @param sessionData User data passed by host
         */
        virtual void onSessionStart(Lobby* /*lobby*/, const RoomInfo & /*room*/, SessionPtr /*session*/, const std::string & /*sessionData*/)
        { }
    };

}

#endif
