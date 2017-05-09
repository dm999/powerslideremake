/**
* MultiSlider library
*
* The MIT License (MIT)
* Copyright (c) 2016 Alexey Gruzdev
*/

#ifndef _MULTI_SLIDER_CONSTANTS_H_
#define _MULTI_SLIDER_CONSTANTS_H_

#include "CommonIncludes.h"

namespace multislider
{
    namespace constants
    {
        static const uint32_t VERSION_MAJOR    = 0;
        static const uint32_t VERSION_MINOR    = 8;
        static const uint32_t VERSION_REVISION = 3;

        static const char RESPONSE_GREET[] = "HALO";
        static const char RESPONSE_SUCC[]  = "SUCC";
        static const char RESPONSE_SUCK[]  = "SUCK";
        static const char RESPONSE_ROOM_IS_FULL[] = "FULL";
        static const char RESPONSE_ROOM_EXISTS[]  = "ROOM";
        static const char RESPONSE_NAME_EXISTS[]  = "NAME";

        static const char MESSAGE_KEY_CLASS[]            = "jsonClass";
        static const char MESSAGE_KEY_PLAYER_NAME[]      = "playerName";
        static const char MESSAGE_KEY_ROOM[]             = "room";
        static const char MESSAGE_KEY_ROOM_NAME[]        = "roomName";
        static const char MESSAGE_KEY_ROOM_DESC[]        = "description";
        static const char MESSAGE_KEY_DATA[]             = "data";
        static const char MESSAGE_KEY_SHARED_DATA[]      = "sharedData";
        static const char MESSAGE_KEY_TO_SELF[]          = "toSelf";
        static const char MESSAGE_KEY_ADDRESS[]          = "address";
        static const char MESSAGE_KEY_SENDER[]           = "sender";
        static const char MESSAGE_KEY_IP[]               = "ip";
        static const char MESSAGE_KEY_PORT[]             = "port";
        static const char MESSAGE_KEY_HOST[]             = "host";
        static const char MESSAGE_KEY_NAME[]             = "name";
        static const char MESSAGE_KEY_FLAGS[]            = "flags";
        static const char MESSAGE_KEY_ID[]               = "id";
        static const char MESSAGE_KEY_SESSION_ID[]       = "sessionId";
        static const char MESSAGE_KEY_TIMESTAMP[]        = "timestamp";
        static const char MESSAGE_KEY_SHARED_TIMESTAMP[] = "sharedTimestamp";
        static const char MESSAGE_KEY_DELAY[]            = "delay";
        static const char MESSAGE_KEY_SYNC_ID[]          = "syncId";
        static const char MESSAGE_KEY_FORCE_BROADCAST[]  = "forceBroadcast";
        static const char MESSAGE_KEY_PLAYERS[]          = "players";
        static const char MESSAGE_KEY_PLAYERS_LIMIT[]    = "playersLimit";
        static const char MESSAGE_KEY_PLAYERS_NUMBER[]   = "playersNumber";
        static const char MESSAGE_KEY_PLAYERS_RESERVED[] = "playersReserved";
        static const char MESSAGE_KEY_USER_PARAM[]       = "userParam";
        static const char MESSAGE_KEY_SEQ_INDEX[]        = "seqIdx";
        static const char MESSAGE_KEY_SERVER_NAME[]      = "serverName";
        static const char MESSAGE_KEY_VERSION_MAJOR[]    = "verMajor";
        static const char MESSAGE_KEY_VERSION_MINOR[]    = "verMinor";
        static const char MESSAGE_KEY_VERSION_REVISION[] = "verRevision";
        static const char MESSAGE_KEY_WEAK[]             = "weak";
        static const char MESSAGE_KEY_ALIVE[]            = "alive";
        static const char MESSAGE_KEY_REQUEST_TIME[]     = "requestTime";
        static const char MESSAGE_KEY_RESPONSE_TIME[]    = "responseTime";

        namespace frontend
        {
            static const char ENVELOP[] = "FrontendMessage$Envelop";

            // Outcoming
            static const char CREATE_ROOM[]   = "FrontendMessage$CreateRoom";
            static const char RECONFIGURE[]   = "FrontendMessage$Reconfigure";
            static const char GET_ROOMS[]     = "FrontendMessage$GetRooms";

            static const char CLOSE_ROOM[]    = "FrontendMessage$CloseRoom";
            static const char EJECT_PLAYER[]  = "FrontendMessage$EjectPlayer";

            static const char JOIN_ROOM[]     = "FrontendMessage$JoinRoom";
            static const char LEAVE_ROOM[]    = "FrontendMessage$LeaveRoom";

            // Incoming
            static const char EJECTED[]         = "FrontendMessage$Ejected";
            static const char START_SESSION[]   = "FrontendMessage$StartSession";
            static const char SESSION_STARTED[] = "FrontendMessage$SessionStarted";

            static const char GREETING[]      = "FrontendMessage$Greeting";

            static const char RECONFIGURE_SUCK[] = "FrontendMessage$ReconfigureSuck";
            static const char RECONFIGURE_SUCC[] = "FrontendMessage$ReconfigureSucc";

            // Both directions
            static const char BROADCAST[]     = "FrontendMessage$Update";
            static const char MESSAGE[]       = "FrontendMessage$Message";
        }

        namespace backend
        {
            static const char ENVELOP[] = "BackendMessage$Envelop";

            // Outcoming
            static const char READY[]  = "BackendMessage$Ready";
            static const char UPDATE[] = "BackendMessage$Update";
            static const char SYNC_REQUEST[] = "BackendMessage$RequestSync";
            static const char QUIT[]   = "BackendMessage$Quit";
            static const char KEEP_ALIVE[] = "BackendMessage$KeepAlive";

            // Incoming
            static const char START[] = "BackendMessage$Start";
            static const char STATE[] = "BackendMessage$SessionState";
            static const char SYNC[]  = "BackendMessage$Sync";
            static const char ACK[]   = "BackendMessage$Ack";

            // Both directions
            static const char CLOCK_SYNC[] = "BackendMessage$ClockSync";
            static const char MESSAGE[]    = "BackendMessage$Message";
        }

        static const uint64_t DEFAULT_TIMEOUT_MS = 10000; // 10 seconds
        static const uint64_t KEEP_ALIVE_LIMIT   = 10000; // 10 seconds
        static const uint64_t RESEND_INTERVAL    =  1000; // 1 second
    }
}


#endif
