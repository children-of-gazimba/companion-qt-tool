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
            networkauth \
            concurrent
            #webenginewidgets

include(../sockets/src/companion-qt-sockets.pri)
include(../qtuio/src/companion-qtuio.pri)
include(../api/src/companion-qt-api-client.pri)

SOURCES += main.cpp \
    api/api_config_model.cpp \
    api/cloud_control_panel.cpp \
    api/extended_data_types.cpp \
    main_window.cpp \
    _TEST/audio_widget.cpp \
    _TEST/content_browser.cpp \
    _TEST/multi_track_media_player.cpp \
    _TEST/player_controls.cpp \
    db/core/sqlite_wrapper.cpp \
    db/model/category_tree_model.cpp \
    db/model/sound_file_table_model.cpp \
    db/table_records.cpp \
    misc/drop_group_box.cpp \
    misc/standard_item_model.cpp \
    misc/char_input_dialog.cpp \
    db/model/resource_dir_table_model.cpp \
    playlist/playlist.cpp \
    playlist/playlist_deprecated.cpp \
    resources/importer.cpp \
    resources/lib.cpp \
    resources/path_fixer.cpp \
    resources/image_file.cpp \
    resources/resource_file.cpp \
    resources/sound_file.cpp \
    log/file_logger.cpp \
    db/model/image_dir_table_model.cpp \
    sound/sound_list_playback_view.cpp \
    sound/sound_list_playback_view_deprecated.cpp \
    sound/sound_stream_player.cpp \
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
    spotify/spotify_tile_configure_dialog.cpp \
    resources/web_pixmap.cpp \
    image/image_item.cpp \
    image/interactive/interactive_image.cpp \
    image/interactive/interactive_image_token.cpp \
    image/image_display_widget.cpp \
    tuio/tuio_control_panel.cpp \
    tuio/tuio_graphics_view.cpp \
    tuio/models/tuio_cursor_table_model.cpp \
    tuio/models/tuio_token_table_model.cpp \
    tuio/tuio_model_handler.cpp \
    tuio/tuio_token_item.cpp \
    tuio/tuio_cursor_item.cpp \
    tracking/tracker.cpp \
    tracking/trackable.cpp \
    tuio/register_token_dialog.cpp \
    tracking/tuio_token_tracker.cpp \
    tuio/models/tuio_blob_table_model.cpp \
    tuio/tuio_blob_item.cpp \
    tracking/models/tracker_table_model.cpp \
    misc/widget_list_view.cpp \
    misc/container.cpp \
    image/interactive/interactive_image_token_widget.cpp \
    tracking/tracker_picker.cpp \
    image/interactive/interactive_image_shape.cpp \
    image/interactive/interactive_image_shape_widget.cpp \
    tracking/activation_tracker.cpp \
    tracking/tracker_picker_dialog.cpp \
    tile/map_tile.cpp \
    misc/volume_mapper.cpp \
    image/models/image_directory_model.cpp \
    image/thumbnail_list.cpp \
    tile/nested_path_widget.cpp \
    misc/progress_button.cpp \
    web/companion_server.cpp \
    web/companion_udp_discovery.cpp \
    web/socket_host_widget.cpp \
    category/category_tree_view.cpp \
    db/database_handler.cpp \
    db/core/database_api.cpp \
    image/image_browser.cpp \
    image/image_canvas.cpp \
    sound/sound_file_player.cpp \
    sound/sound_list_view.cpp \
    sound/sound_list_view_dialog.cpp \
    tile/canvas.cpp \
    playlist/playlist_player.cpp \
    playlist/playlist_settings_widget.cpp \
    json/json_mime_data_parser.cpp

HEADERS  += main_window.h \
    _TEST/audio_widget.h \
    _TEST/content_browser.h \
    _TEST/multi_track_media_player.h \
    _TEST/player_controls.h \
    api/api_config.h \
    api/api_config_model.h \
    api/cloud_control_panel.h \
    api/extended_data_types.h \
    db/core/sqlite_wrapper.h \
    db/model/category_tree_model.h \
    db/model/sound_file_table_model.h \
    db/table_records.h \
    misc/drop_group_box.h \
    misc/char_input_dialog.h \
    misc/standard_item_model.h \
    db/model/resource_dir_table_model.h \
    playlist/playlist.h \
    playlist/playlist_deprecated.h \
    resources/importer.h \
    resources/lib.h \
    resources/path_fixer.h \
    resources/image_file.h \
    resources/resource_file.h \
    resources/sound_file.h \
    log/file_logger.h \
    db/model/image_dir_table_model.h \
    sound/sound_list_playback_view.h \
    sound/sound_list_playback_view_deprecated.h \
    sound/sound_stream_player.h \
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
    spotify/spotify_tile_configure_dialog.h \
    resources/web_pixmap.h \
    image/image_item.h \
    image/interactive/interactive_image.h \
    image/interactive/interactive_image_token.h \
    image/image_display_widget.h \
    tuio/tuio_control_panel.h \
    tuio/tuio_graphics_view.h \
    tuio/models/tuio_cursor_table_model.h \
    tuio/models/tuio_token_table_model.h \
    tuio/tuio_model_handler.h \
    tuio/tuio_token_item.h \
    tuio/tuio_cursor_item.h \
    tracking/tracker.h \
    tracking/trackable.h \
    tuio/register_token_dialog.h \
    tracking/tuio_token_tracker.h \
    tuio/models/tuio_blob_table_model.h \
    tuio/tuio_blob_item.h \
    tracking/models/tracker_table_model.h \
    misc/widget_list_view.h \
    misc/container.h \
    image/interactive/interactive_image_token_widget.h \
    tracking/tracker_picker.h \
    image/interactive/interactive_image_shape.h \
    image/interactive/interactive_image_shape_widget.h \
    tracking/activation_tracker.h \
    tracking/tracker_picker_dialog.h \
    tile/map_tile.h \
    misc/volume_mapper.h \
    image/models/image_directory_model.h \
    image/thumbnail_list.h \
    tile/nested_path_widget.h \
    misc/progress_button.h \
    web/companion_server.h \
    web/companion_udp_discovery.h \
    web/socket_host_widget.h \
    category/category_tree_view.h \
    db/database_handler.h \
    db/core/database_api.h \
    image/image_browser.h \
    image/image_canvas.h \
    sound/sound_file_player.h \
    sound/sound_list_view.h \
    sound/sound_list_view_dialog.h \
    tile/canvas.h \
    playlist/playlist_player.h \
    playlist/playlist_settings.h \
    playlist/playlist_settings_widget.h \
    json/json_mime_data_parser.h

RESOURCES += \
    _RES/resources.qrc

RC_FILE = companion.rc

OTHER_FILES += _RES/webapp1.ini

