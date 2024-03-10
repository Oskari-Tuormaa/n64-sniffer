add_library(usb_library src/usb.cpp src/usb_descriptors.cpp)
target_include_directories(usb_library PUBLIC include)
target_link_libraries(usb_library PUBLIC pico_stdlib pico_unique_id
                                         tinyusb_device tinyusb_board)
