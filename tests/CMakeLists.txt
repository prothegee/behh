cmake_minimum_required(VERSION 3.15...3.30)

project(test_behh)

# absolute path to this tests source dir, so test data files resolve from any
# working directory instead of the cwd-relative "../../../tests/" form.
add_compile_definitions(BEHH_TESTS_DIR="${CMAKE_CURRENT_SOURCE_DIR}")

# test_aes_openssl
if(BEHH_USING_OPENSSL)
    add_executable(test_aes_openssl test_aes_openssl.cc)
    target_link_libraries(test_aes_openssl PRIVATE behh behh_static)
    message(NOTICE "-- behh: adding test_aes_openssl")
endif()

# test_argon2
if(BEHH_USING_ARGON2)
    add_executable(test_argon2 test_argon2.cc)
    target_link_libraries(test_argon2 PRIVATE behh behh_static)
    message(NOTICE "-- behh: adding test_argon2")
endif()

# test_barcode_qrcode
if(BEHH_USING_STB AND BEHH_USING_NANOSVG AND BEHH_USING_ZXING_CPP)
    add_executable(test_barcode_qrcode test_barcode_qrcode.cc)
    target_link_libraries(test_barcode_qrcode PRIVATE behh behh_static)
    message(NOTICE "-- behh: adding test_barcode_qrcode")
endif()

# test_base64
add_executable(test_base64 test_base64.cc)
target_link_libraries(test_base64 PRIVATE behh behh_static)
message(NOTICE "-- behh: adding test_base64")

# test_communication_smtps
if(BEHH_USING_CURL_EXECUTEABLE AND BEHH_USING_DROGON)
    add_executable(test_communication_smtps test_communication_smtps.cc)
    target_link_libraries(test_communication_smtps PRIVATE behh behh_static Drogon::Drogon)
    message(NOTICE "-- behh: adding test_communication_smtps")
endif()

# test_date_and_time
add_executable(test_date_and_time test_date_and_time.cc)
target_link_libraries(test_date_and_time PRIVATE behh behh_static)
message(NOTICE "-- behh: adding test_date_and_time")

# test_database_couchbase_rw
if(BEHH_USING_COUCHBASE_CXX_CLIENT)
    add_executable(test_database_couchbase_rw test_database_couchbase_rw.cc)
    target_link_libraries(test_database_couchbase_rw behh behh_static)
    message(NOTICE "-- behh: adding test_database_couchbase_rw")
endif()

# test_database_scylla_rw
if(BEHH_USING_SCYLLADB)
    add_executable(test_database_scylla_rw test_database_scylla_rw.cc)
    target_link_libraries(test_database_scylla_rw behh behh_static)
    message(NOTICE "-- behh: adding test_database_scylla_rw")
endif()

# test_file_enc_dec
if(BEHH_USING_OPENSSL)
    add_executable(test_file_enc_dec test_file_enc_dec.cc)
    target_link_libraries(test_file_enc_dec PRIVATE behh behh_static)

    message(NOTICE "-- behh: adding test_file_enc_dec")
endif()

# test_sdl3_check_screen_resolutions
if(BEHH_USING_SDL3)
    add_executable(test_sdl3_check_screen_resolutions test_sdl3_check_screen_resolutions.cc)
    target_link_libraries(test_sdl3_check_screen_resolutions PRIVATE behh behh_static)
    message(NOTICE "-- behh: adding test_sdl3_check_screen_resolutions")
endif()