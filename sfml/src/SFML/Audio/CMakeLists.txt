
set(INCROOT ${PROJECT_SOURCE_DIR}/sfml/include/SFML/Audio)
set(SRCROOT ${PROJECT_SOURCE_DIR}/sfml/src/SFML/Audio)

include_directories (${PROJECT_SOURCE_DIR}/ogg/include)
include_directories (${PROJECT_SOURCE_DIR}/vorbis/include)

include_directories (${PROJECT_SOURCE_DIR}/sfml/include)
include_directories (${PROJECT_SOURCE_DIR}/sfml/src)

include_directories(${PowerslideRemake_OPENALDIR}/include)
link_directories (${PowerslideRemake_OPENALDIR}/lib)

# all source files
set(SRC
    ${SRCROOT}/ALCheck.cpp
    ${SRCROOT}/ALCheck.hpp
    ${SRCROOT}/AlResource.cpp
    ${INCROOT}/AlResource.hpp
    ${SRCROOT}/AudioDevice.cpp
    ${SRCROOT}/AudioDevice.hpp
    ${INCROOT}/Export.hpp
    ${SRCROOT}/Listener.cpp
    ${INCROOT}/Listener.hpp
    ${SRCROOT}/Music.cpp
    ${INCROOT}/Music.hpp
    ${SRCROOT}/Sound.cpp
    ${INCROOT}/Sound.hpp
    ${SRCROOT}/SoundBuffer.cpp
    ${INCROOT}/SoundBuffer.hpp
    ${SRCROOT}/SoundBufferRecorder.cpp
    ${INCROOT}/SoundBufferRecorder.hpp
    ${SRCROOT}/InputSoundFile.cpp
    ${INCROOT}/InputSoundFile.hpp
    ${SRCROOT}/OutputSoundFile.cpp
    ${INCROOT}/OutputSoundFile.hpp
    ${SRCROOT}/SoundRecorder.cpp
    ${INCROOT}/SoundRecorder.hpp
    ${SRCROOT}/SoundSource.cpp
    ${INCROOT}/SoundSource.hpp
    ${SRCROOT}/SoundStream.cpp
    ${INCROOT}/SoundStream.hpp
)
source_group("" FILES ${SRC})

set(CODECS_SRC
    ${SRCROOT}/SoundFileFactory.cpp
    ${INCROOT}/SoundFileFactory.hpp
    ${INCROOT}/SoundFileFactory.inl
    ${INCROOT}/SoundFileReader.hpp
    ${SRCROOT}/SoundFileReaderOgg.hpp
    ${SRCROOT}/SoundFileReaderOgg.cpp
)
source_group("codecs" FILES ${CODECS_SRC})

# avoids warnings in vorbisfile.h
#target_compile_definitions(Vorbis INTERFACE "OV_EXCLUDE_STATIC_CALLBACKS")

# define the sfml-audio target
add_library(sfml-audio STATIC ${SRC} ${CODECS_SRC})

# setup dependencies
if(ANDROID)
else()
    if(WIN32)
        target_link_libraries(sfml-audio PRIVATE OpenAL32)
    else()
        target_link_libraries(sfml-audio PRIVATE openal)
    endif()
endif()

if(SFML_OS_ANDROID)
    target_link_libraries(sfml-audio PRIVATE android OpenSLES)
endif()

target_link_libraries(sfml-audio PUBLIC sfml-system PRIVATE vorbis)
