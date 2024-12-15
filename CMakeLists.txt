cmake_minimum_required(VERSION 3.5)

project(Burning-Forest)

add_executable(${PROJECT_NAME} main.c)
set(ICON_PATH ${CMAKE_SOURCE_DIR}/assets/icon0.png)
target_link_libraries(${PROJECT_NAME} PRIVATE
    pspdebug
    pspdisplay
    pspge
    pspctrl
)

# Create an EBOOT.PBP file
create_pbp_file(
    TARGET ${PROJECT_NAME}
    ICON_PATH ${ICON_PATH}
    BACKGROUND_PATH NULL
    PREVIEW_PATH NULL
    TITLE ${PROJECT_NAME}
    VERSION 01.00
)
