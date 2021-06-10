cmake_minimum_required(VERSION 3.5)

set(CLIENT_NAME appchat-client)

set(QT_DEBUG_PLUGINS 1)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CLIENT_BIN  client)
set(Control_dir control)
set(Client_dir client)
set(GUI_dir gui)
set(Cache_dir cache)

set(CLIENT_SOURCES
    startup_client.cpp
    client/msgtype.cpp
    client/msgtype.h
    ${Client_dir}/client.h
    ${Client_dir}/client.cpp

    ${GUI_dir}/LoginWidget/loginwidget.h
    ${GUI_dir}/LoginWidget/loginwidget.cpp
    ${GUI_dir}/LoginWidget/loginwidget.ui

	  ${GUI_dir}/ChannelWidget/channelwidget.h
	  ${GUI_dir}/ChannelWidget/channelwidget.cpp
	  ${GUI_dir}/ChannelWidget/channelwidget.ui

	  ${GUI_dir}/MessageWidget/messagewidget.h
	  ${GUI_dir}/MessageWidget/messagewidget.cpp
	  ${GUI_dir}/MessageWidget/messagewidget.ui

	  ${GUI_dir}/delegate/chatdelegate.h
	  ${GUI_dir}/delegate/chatdelegate.cpp

    ${GUI_dir}/CustomTextEdit/customtextedit.h
    ${GUI_dir}/CustomTextEdit/customtextedit.cpp

	  ${GUI_dir}/mainwindow.h
	  ${GUI_dir}/mainwindow.cpp
	  ${GUI_dir}/mainwindow.ui

    ${Control_dir}/control.h
    ${Control_dir}/control.cpp

    ${Cache_dir}/icache.h
    ${Cache_dir}/sqlitecache.h
    ${Cache_dir}/history_cacher.h
    ${Cache_dir}/history_cacher.cpp
)

find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)
find_package (Threads)
find_library(PROTOCOL NAMES protocol_lib HINTS "/usr/local/lib/goodok")

add_executable(${CLIENT_NAME}  ${CLIENT_SOURCES})

find_library(SQLITE3_LIBRARY NAMES sqlite3)
find_package(Boost COMPONENTS system log REQUIRED)

set_target_properties(${CLIENT_NAME} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
        LINK_LIBRARIES pthread
#        COMPILE_OPTIONS "-std=c++17;-O2;-Wall;-Wextra;-Wpedantic"
		COMPILE_OPTIONS "-fPIC"
)
#   set(CMAKE_CXX_FLAGS "-fPIC")

target_include_directories(${CLIENT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
		"/usr/local/include/goodok"
        ${GUI_dir}
)
target_link_libraries (${CLIENT_NAME}
		Qt5::Widgets
	${CONAN_LIBS}
	${PROTOCOL}
)

install(TARGETS ${CLIENT_NAME}
        DESTINATION
            "/usr/local/bin/goodok")


