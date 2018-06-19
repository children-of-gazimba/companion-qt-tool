#-------------------------------------------------
#
# Project created by QtCreator 2016-04-20T18:37:45
#
#-------------------------------------------------
TARGET = companion-desktop
TEMPLATE = app

#QMAKE_CXXFLAGS += -std=c++0x

QT       += core \
            gui \
            multimedia \
            multimediawidgets \
            widgets \
            sql \
            network \
            networkauth  #\
            #webenginewidgets


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
    category/tree_view.cpp \
    misc/drop_group_box.cpp \
    misc/json_mime_data_parser.cpp \
    misc/standard_item_model.cpp \
    misc/char_input_dialog.cpp \
    sound_file/list_view.cpp \
    sound_file/master_view.cpp \
    sound_file/list_view_dialog.cpp \
    tile/graphics_view.cpp \
    playlist/settings_widget.cpp \
    custom_media_player.cpp \
    db/model/resource_dir_table_model.cpp \
    resources/importer.cpp \
    resources/lib.cpp \
    resources/path_fixer.cpp \
    resources/image_file.cpp \
    resources/resource_file.cpp \
    resources/sound_file.cpp \
    playlist/media_playlist.cpp \
    log/file_logger.cpp \
    image/view.cpp \
    image/browser.cpp \
    image/list.cpp \
    db/model/image_dir_table_model.cpp \
    tile/nested_tile.cpp \
    preset/preset_view.cpp \
    db/model/preset_table_model.cpp \
    tile/base_tile.cpp \
    tile/playlist_tile.cpp \
    companion_widget.cpp \
    spotify/oauth2_request_handler.cpp \
    spotify/spotify_authenticator.cpp \
    spotify/spotify_handler.cpp \
    spotify/spotify_remote_controller.cpp \
    tile/spotify_tile.cpp \
    spotify/spotify_control_panel.cpp \
    spotify/spotify_tile_configure_dialog.cpp

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
    category/tree_view.h \
    misc/drop_group_box.h \
    misc/char_input_dialog.h \
    misc/json_mime_data_parser.h \
    misc/standard_item_model.h \
    sound_file/list_view.h \
    sound_file/master_view.h \
    sound_file/list_view_dialog.h \
    tile/graphics_view.h \
    playlist/settings.h \
    playlist/settings_widget.h \
    custom_media_player.h \
    db/model/resource_dir_table_model.h \
    resources/importer.h \
    resources/lib.h \
    resources/path_fixer.h \
    resources/image_file.h \
    resources/resource_file.h \
    resources/sound_file.h \
    playlist/media_playlist.h \
    log/file_logger.h \
    image/view.h \
    image/browser.h \
    image/list.h \
    db/model/image_dir_table_model.h \
    tile/nested_tile.h \
    preset/preset_view.h \
    db/model/preset_table_model.h \
    tile/base_tile.h \
    tile/playlist_tile.h \
    companion_widget.h \
    spotify/oauth2_request_handler.h \
    spotify/spotify_authenticator.h \
    spotify/spotify_handler.h \
    spotify/spotify_remote_controller.h \
    tile/spotify_tile.h \
    spotify/spotify_control_panel.h \
    spotify/spotify_tile_configure_dialog.h

RESOURCES += \
    _RES/resources.qrc

RC_FILE = companion.rc

OTHER_FILES += _RES/webapp1.ini

