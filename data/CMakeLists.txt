cmake_minimum_required(VERSION 3.10)

project(PulsarAssets)

file(GLOB IMAGES_FILES *.xpm *.gif)

install(FILES ${IMAGES_FILES}
        DESTINATION assets)

install(DIRECTORY map
        DESTINATION assets)