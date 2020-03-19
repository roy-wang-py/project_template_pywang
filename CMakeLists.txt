# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.11)

find_package(Git)
# 生成版本描述字符串类似 TAG-X-gHASH
execute_process(COMMAND ${GIT_EXECUTABLE} describe --abbrev=6 --always --tags
    OUTPUT_VARIABLE  GIT_REPO_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# 获取最新 commit 日期，YYYY-MM-DD_HH:MM:SS
execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%cd --date=format:%F_%T
    OUTPUT_VARIABLE  GIT_REPO_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
# 获取最新 commit Hash
execute_process(
        COMMAND ${GIT_EXECUTABLE} log -1 --pretty=format:%h
        OUTPUT_VARIABLE GIT_REPO_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
)
execute_process(
    COMMAND ${GIT_EXECUTABLE} symbolic-ref --short -q HEAD
    OUTPUT_VARIABLE _git_branch
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
execute_process(
    COMMAND ${GIT_EXECUTABLE} config user.name
    OUTPUT_VARIABLE _GIT_USER_NAME
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
)
execute_process(
    COMMAND ${GIT_EXECUTABLE} log --no-merges 
    COMMAND grep commit
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_FILE __temp_git_commit
    ERROR_QUIET
)
execute_process(
    COMMAND cat 
    COMMAND wc -l
    OUTPUT_STRIP_TRAILING_WHITESPACE
    INPUT_FILE __temp_git_commit
    OUTPUT_VARIABLE _GIT_COMMIT_COUNT
    ERROR_QUIET
)
execute_process(
    COMMAND date +%Y-%m-%d_%H:%M:%S
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE _BUILD_TIME
    ERROR_QUIET
)
message("============================================")
message("-- git user:${_GIT_USER_NAME}")
message("-- git version:${GIT_REPO_VERSION}")
message("-- git commit date:${GIT_REPO_DATE}")
message("-- git commit hash:${GIT_REPO_HASH}")
message("-- git commit count:${_GIT_COMMIT_COUNT}")
message("-- build date_time:${_BUILD_TIME}")
message("============================================")
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/source/git_version.h.in
    ${CMAKE_BINARY_DIR}/Version.h
    @ONLY
    )

execute_process(
    COMMAND sed -n "/define/p" 
    COMMAND sed -n "2,2 p" 
    COMMAND awk "{print $3}"
    COMMAND sed  "s/\"//g" 
    OUTPUT_STRIP_TRAILING_WHITESPACE
    INPUT_FILE ${CMAKE_BINARY_DIR}/Version.h
    OUTPUT_FILE ${CMAKE_BINARY_DIR}/version
)
execute_process(
    COMMAND rm   __temp_git_commit
)

add_subdirectory ("source")

