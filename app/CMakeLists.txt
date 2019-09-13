IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-variable -Wno-unused-function -Wno-shadow")
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")

IF(CMAKE_COMPILER_IS_GNUCC)
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sign-compare")
    IF(GCC_WARN_SIGN_CONVERSION)
        SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sign-conversion")
    ENDIF(GCC_WARN_SIGN_CONVERSION)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_C_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-shorten-64-to-32")
ENDIF(CMAKE_C_COMPILER_ID MATCHES "Clang")

set(LIBS ${LIBS}
	${M_LIB}
	${CMAKE_THREAD_LIBS_INIT}
	${CMAKE_DL_LIBS}
	signal-protocol-c
)

include_directories(. ../src ../crypto)

set(loki_pager_app_SRCS
main.c
)

add_executable(pager ${loki_pager_app_SRCS})
target_link_libraries(pager ${LIBS})

# Add -fPIC flag
if(BUILD_SHARED_LIBS)
    set_property(TARGET pager PROPERTY POSITION_INDEPENDENT_CODE ON)
endif()