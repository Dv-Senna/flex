include(FetchContent)

FetchContent_Declare(
	Catch2
	GIT_REPOSITORY https://github.com/catchorg/Catch2.git
	GIT_TAG        v3.8.1 # or a later release
	DOWNLOAD_DIR   ${PROJECT_BINARY_DIR}/download/vendors/Catch2
	SOURCE_DIR     ${PROJECT_SOURCE_DIR}/vendors/Catch2
	BINARY_DIR     ${PROJECT_BINARY_DIR}/vendors/Catch2
	FIND_PACKAGE_ARGS
)

FetchContent_MakeAvailable(Catch2)
if (DEFINED catch2_SOURCE_DIR)
	list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
endif()
