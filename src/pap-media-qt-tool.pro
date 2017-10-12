#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T18:37:45
#
#-------------------------------------------------
TARGET = pap-media-qt-tool
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

QT       += core \
            gui \
            multimedia \
            multimediawidgets \
            widgets \
            sql \
            network

include(../QtWebApp/QtWebApp/httpserver/httpserver.pri)

SOURCES += main.cpp \
    main_window.cpp \
    _TEST/audio_widget.cpp \
    _TEST/content_browser.cpp \
    _TEST/multi_track_media_player.cpp \
    _TEST/player_controls.cpp \
    db/core/api.cpp \
    db/core/sqlite_wrapper.cpp \
    db/model/category_tree_model.cpp \
    db/model/sound_file_table_model.cpp \
    db/handler.cpp \
    db/table_records.cpp \
    dsa_media_control_kit.cpp \
    category/tree_view.cpp \
    misc/drop_group_box.cpp \
    misc/json_mime_data_parser.cpp \
    misc/standard_item_model.cpp \
    misc/char_input_dialog.cpp \
    sound_file/list_view.cpp \
    sound_file/master_view.cpp \
    sound_file/list_view_dialog.cpp \
    2D/graphics_view.cpp \
    2D/tile.cpp \
    2D/player_tile.cpp \
    2D/playlist_player_tile.cpp \
    playlist/settings_widget.cpp \
    custom_media_player.cpp \
    db/model/resource_dir_table_model.cpp \
    web/host.cpp \
    resources/importer.cpp \
    resources/lib.cpp \
    resources/path_fixer.cpp \
    resources/image_file.cpp \
    resources/resource_file.cpp \
    resources/sound_file.cpp \
    playlist/media_playlist.cpp \
    web/request_handler.cpp \
    web/controllers/preset_controller.cpp \
    log/file_logger.cpp \
    web/app/chat.cpp \
    web/controllers/chat_controller.cpp \
    web/app/chat_message_model.cpp \
    web/app/image_loader.cpp \
    web/controllers/image_controller.cpp

HEADERS  += main_window.h \
    _TEST/audio_widget.h \
    _TEST/content_browser.h \
    _TEST/multi_track_media_player.h \
    _TEST/player_controls.h \
    db/core/api.h \
    db/core/sqlite_wrapper.h \
    db/model/category_tree_model.h \
    db/model/sound_file_table_model.h \
    db/handler.h \
    db/table_records.h \
    dsa_media_control_kit.h \
    category/tree_view.h \
    misc/drop_group_box.h \
    misc/char_input_dialog.h \
    misc/json_mime_data_parser.h \
    misc/standard_item_model.h \
    sound_file/list_view.h \
    sound_file/master_view.h \
    sound_file/list_view_dialog.h \
    2D/graphics_view.h \
    2D/tile.h \
    2D/player_tile.h \
    2D/playlist_player_tile.h \
    playlist/settings.h \
    playlist/settings_widget.h \
    custom_media_player.h \
    db/model/resource_dir_table_model.h \
    web/host.h \
    resources/importer.h \
    resources/lib.h \
    resources/path_fixer.h \
    resources/image_file.h \
    resources/resource_file.h \
    resources/sound_file.h \
    playlist/media_playlist.h \
    web/request_handler.h \
    web/controllers/preset_controller.h \
    web/response.h \
    log/file_logger.h \
    web/app/chat.h \
    web/controllers/chat_controller.h \
    web/app/chat_message_model.h \
    web/app/chat_message.h \
    web/app/image_loader.h \
    web/controllers/image_controller.h

RESOURCES += \
    _RES/resources.qrc

OTHER_FILES += _RES/webapp1.ini

