enable_testing()
find_library(M_LIB m)
find_package(Check 0.9.10 REQUIRED)
find_package(Threads)
include_directories(. ../src ../json ../http ../crypto ${CHECK_INCLUDE_DIRS})

IF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-function")
ENDIF(CMAKE_COMPILER_IS_GNUCC OR CMAKE_C_COMPILER_ID MATCHES "Clang")

IF(CMAKE_COMPILER_IS_GNUCC)
	SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sign-compare")
	IF(GCC_WARN_SIGN_CONVERSION)
		SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-sign-conversion")
	ENDIF(GCC_WARN_SIGN_CONVERSION)
ENDIF(CMAKE_COMPILER_IS_GNUCC)

IF(CMAKE_C_COMPILER_ID MATCHES "Clang")
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-invalid-source-encoding -Wno-shorten-64-to-32")
ENDIF(CMAKE_C_COMPILER_ID MATCHES "Clang")

# On Windows .exe and .dll files must be placed at the same directory
if(WIN32 AND BUILD_SHARED_LIBS)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/src)
  set(TEST_PATH ${CMAKE_BINARY_DIR}/src)
else()
  set(TEST_PATH ${CMAKE_CURRENT_BINARY_DIR})
endif()

if(WIN32 AND NOT MSVC)
set(CHECK_LDFLAGS ${CHECK_LIBRARIES})
set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -lcompat")
endif()

set(LIBS ${LIBS}
	${M_LIB}
	${CHECK_LDFLAGS}
	${CMAKE_THREAD_LIBS_INIT}
	${CMAKE_DL_LIBS}
	signal-protocol-c
)

if(WIN32)
set(LIBS ${LIBS}
	ws2_32
)
endif()

set(common_SRCS
	test_common.c
	test_common.h
)

set(common_SRCS ${common_SRCS}
	test_common_mbedtls.c
)

if(NOT MSVC)
add_executable(test_curve25519 test_curve25519.c ${common_SRCS})
target_link_libraries(test_curve25519 ${LIBS})
add_test(test_curve25519 ${TEST_PATH}/test_curve25519)

add_executable(test_hkdf test_hkdf.c ${common_SRCS})
target_link_libraries(test_hkdf ${LIBS})
add_test(test_hkdf ${TEST_PATH}/test_hkdf)

add_executable(test_ratchet test_ratchet.c ${common_SRCS})
target_link_libraries(test_ratchet ${LIBS})
add_test(test_ratchet ${TEST_PATH}/test_ratchet)

add_executable(test_protocol test_protocol.c ${common_SRCS})
target_link_libraries(test_protocol ${LIBS})
add_test(test_protocol ${TEST_PATH}/test_protocol)

add_executable(test_session_record test_session_record.c ${common_SRCS})
target_link_libraries(test_session_record ${LIBS})
add_test(test_session_record ${TEST_PATH}/test_session_record)

add_executable(test_session_cipher test_session_cipher.c ${common_SRCS})
target_link_libraries(test_session_cipher ${LIBS})
add_test(test_session_cipher ${TEST_PATH}/test_session_cipher)

add_executable(test_session_builder test_session_builder.c ${common_SRCS})
target_link_libraries(test_session_builder ${LIBS})
add_test(test_session_builder ${TEST_PATH}/test_session_builder)

add_executable(test_key_helper test_key_helper.c ${common_SRCS})
target_link_libraries(test_key_helper ${LIBS})
add_test(test_key_helper ${TEST_PATH}/test_key_helper)

add_executable(test_simultaneous_initiate test_simultaneous_initiate.c ${common_SRCS})
target_link_libraries(test_simultaneous_initiate ${LIBS})
add_test(test_simultaneous_initiate ${TEST_PATH}/test_simultaneous_initiate)

add_executable(test_sender_key_record test_sender_key_record.c ${common_SRCS})
target_link_libraries(test_sender_key_record ${LIBS})
add_test(test_sender_key_record ${TEST_PATH}/test_sender_key_record)

add_executable(test_group_cipher test_group_cipher.c ${common_SRCS})
target_link_libraries(test_group_cipher ${LIBS})
add_test(test_group_cipher ${TEST_PATH}/test_group_cipher)

add_executable(test_fingerprint test_fingerprint.c ${common_SRCS})
target_link_libraries(test_fingerprint ${LIBS})
add_test(test_fingerprint ${TEST_PATH}/test_fingerprint)

add_executable(test_device_consistency test_device_consistency.c ${common_SRCS})
target_link_libraries(test_device_consistency ${LIBS})
add_test(test_device_consistency ${TEST_PATH}/test_device_consistency)
endif()

add_executable(crypto_selftest crypto_selftest.c)
target_link_libraries(crypto_selftest ${LIBS})
add_test(test_crypto_svc_provider ${TEST_PATH}/crypto_selftest)

add_executable(test_json_parser test_json_parse.c)
target_link_libraries(test_json_parser ${LIBS})
add_test(test_json_parser ${TEST_PATH}/test_json_parser)

add_executable(test_web_client test_http_client.c)
target_link_libraries(test_web_client ${LIBS})
add_test(test_web_client ${TEST_PATH}/test_web_client)

add_custom_command(TARGET test_web_client POST_BUILD
  COMMAND curl https://curl.haxx.se/ca/cacert.pem > rootcerts.pem
  COMMENT "Downloading Netscape Navigator root certificate bundle for unit testing"
)

add_executable(crypto_benchmark crypto_benchmark.c)
target_link_libraries(crypto_benchmark ${LIBS})
