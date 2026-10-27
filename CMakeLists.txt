cmake_minimum_required(VERSION 3.19)

project(qodfisc LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
qt_standard_project_setup()
set(CMAKE_AUTORCC ON)

qt_add_executable(qodfisc WIN32 MACOSX_BUNDLE
	main.cpp
	qodfisc.cpp qodfisc.h
	comuni.h
	mainwindow.ui
	qodfisc.qrc
)

if(WIN32)
	target_sources(qodfisc PRIVATE qodfisc.rc)
endif()

target_link_libraries(qodfisc PRIVATE Qt6::Widgets)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
	target_compile_options(qodfisc PRIVATE -Wall -Wextra)
endif()

install(TARGETS qodfisc
	BUNDLE DESTINATION .
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
