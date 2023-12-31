
set(INCROOT ${PROJECT_SOURCE_DIR}/sfml/include/SFML/Network)
set(SRCROOT ${PROJECT_SOURCE_DIR}/sfml/src/SFML/Network)

include_directories (${PROJECT_SOURCE_DIR}/sfml/include)
include_directories (${PROJECT_SOURCE_DIR}/sfml/src)

# all source files
set(SRC
    ${INCROOT}/Export.hpp
    ${SRCROOT}/Ftp.cpp
    ${INCROOT}/Ftp.hpp
    ${SRCROOT}/Http.cpp
    ${INCROOT}/Http.hpp
    ${SRCROOT}/IpAddress.cpp
    ${INCROOT}/IpAddress.hpp
    ${SRCROOT}/Packet.cpp
    ${INCROOT}/Packet.hpp
    ${SRCROOT}/Socket.cpp
    ${INCROOT}/Socket.hpp
    ${SRCROOT}/SocketImpl.hpp
    ${INCROOT}/SocketHandle.hpp
    ${SRCROOT}/SocketSelector.cpp
    ${INCROOT}/SocketSelector.hpp
    ${SRCROOT}/TcpListener.cpp
    ${INCROOT}/TcpListener.hpp
    ${SRCROOT}/TcpSocket.cpp
    ${INCROOT}/TcpSocket.hpp
    ${SRCROOT}/UdpSocket.cpp
    ${INCROOT}/UdpSocket.hpp
)

# add platform specific sources
if(SFML_OS_WINDOWS)
    set(SRC
        ${SRC}
        ${SRCROOT}/Win32/SocketImpl.cpp
        ${SRCROOT}/Win32/SocketImpl.hpp
    )
else()
    set(SRC
        ${SRC}
        ${SRCROOT}/Unix/SocketImpl.cpp
        ${SRCROOT}/Unix/SocketImpl.hpp
    )
endif()

source_group("" FILES ${SRC})

# define the sfml-network target
add_library(sfml-network STATIC ${SRC})

# setup dependencies
target_link_libraries(sfml-network PUBLIC sfml-system)
if(SFML_OS_WINDOWS)
    target_link_libraries(sfml-network PRIVATE ws2_32)
endif()
