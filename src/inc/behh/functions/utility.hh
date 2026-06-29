#ifndef BEHH_UTILITY_HH
#define BEHH_UTILITY_HH
#include "../pch.hh"

#include <behh/constants/string_const.hh>

#if BEHH_USING_OPENSSL
#include <openssl/aes.h>
#include <openssl/buffer.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/types.h>
#endif // BEHH_USING_OPENSSL

#if BEHH_USING_JSONCPP
#include <json/json.h>
#endif // BEHH_USING_JSONCPP

#if BEHH_USING_COUCHBASE_CXX_CLIENT
#include <tao/json.hpp>
#include <tao/json/contrib/traits.hpp>
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

namespace behh {
namespace utility_functions {

/**
 * @brief check if file_path is exists
 *
 * @param file_path
 * @return int32_t 1 mean ok
 */
int32_t file_exists(const std::string& file_path);

/**
 * @brief check if input is numeric value
 *
 * @note next value of zero '0' will considered false, i.e. "0123"
 *
 * @param input
 * @return true
 * @return false
 */
bool is_numeric(const std::string& input);

/**
 * @brief check if input is floating valu (float/double)
 *
 * @note next value of zero '0' will considered false, i.e. "01.234"
 *
 * @param input
 * @return true
 * @return false
 */
bool is_numeric_decimal(const std::string& input);

/**
 * @brief check if input is unsigned numeric
 *
 * @note next value of zero '0' will considered false, i.e. "0123"
 *
 * @param input
 * @return true
 * @return false
 */
bool is_numeric_unsigned(const std::string& input);

/**
 * @brief encode base64
 *
 * @param input
 * @return std::string
 */
std::string base64encode(const std::string& input);

/**
 * @brief decode base64
 *
 * @param input
 * @return std::string
 */
std::string base64decode(const std::string& input);

#if BEHH_USING_OPENSSL
/**
 * @brief encode base64 using openssl
 *
 * @param input
 * @return std::string
 */
std::string base64encode_openssl(const std::string& input);

/**
 * @brief decode base64 using openssl
 *
 * @param input
 * @return std::string
 */
std::string base64decode_openssl(const std::string& input);
#endif // BEHH_USING_OPENSSL

namespace find {

/**
 * @brief find keyword/s from source where filter is in keywords
 *
 * @note return size is how many founded words in source
 *
 * @param source
 * @param keywords keyword/s to found
 * @return std::vector<std::string>
 */
std::vector<std::string> each_keywords(const std::string& source, const std::vector<std::string>& keywords);

/**
 * @brief format string source, find match query and replace it
 *
 * @param source
 * @param query
 * @param replacement
 * @return int32_t 1 mean ok
 */
int32_t and_replace_all(std::string& source, const std::string& query, const std::string& replacement);

/**
 * @brief find input end with `keyword` param
 *
 * @param input
 * @param keyword
 * @return int32_t 1 mean ok
 */
int32_t input_ends_with(const std::string& input, const std::string& keyword);

/**
 * @brief find keyword after `keyword` in source & extract it
 *
 * @param source
 * @param keyword
 * @param extraction_result
 * @return int32_t 1 mean ok
 */
int32_t keyword_after(const std::string& source, const std::string& keyword, std::string& extraction_result);

/**
 * @brief find keywird before `keyword` in source & extract it
 *
 * @param source
 * @param keyword
 * @param extraction_result
 * @return int32_t
 */
int32_t keyword_before(const std::string& source, const std::string& keyword, std::string& extraction_result);

} // namespace find

namespace generate {

/**
 * @brief generate random number from min to max param
 *
 * @param min
 * @param max
 * @return int32_t
 */
int32_t random_number(const int32_t& min, const int32_t& max);

/**
 * @brief generate random number from min to max param
 *
 * @param min
 * @param max
 * @return int64_t
 */
int64_t random_number(const int64_t& min, const int64_t& max);

/**
 * @brief generate random number from min to max param
 *
 * @param min
 * @param max
 * @return float32_t
 */
float32_t random_number(const float32_t& min, const float32_t& max);

/**
 * @brief generate random number from min to max param
 *
 * @param min
 * @param max
 * @return float64_t
 */
float64_t random_number(const float64_t& min, const float64_t& max);

/**
 * @brief generate random 16 bytes
 *
 * @return std::array<uint8_t, 16>
 */
std::array<uint8_t, 16> random_bytes();

/**
 * @brief generate random alphanumeric based on length param
 *
 * @note if size is less or equal to zero, it will set to 1 and return 1 letter
 *
 * @param length
 * @return std::string
 */
std::string random_alphanumeric(size_t length);

/**
 * @brief generate random alphanumeric with special chars based on length param
 *
 * @note if size is less or equal to zero, it will set to 1 and return 1 letter
 *
 * @param length
 * @return std::string
 */
std::string random_alphanumeric_with_special_character(size_t length);

namespace uuid {
/**
 * @brief generate uuid v1 ( MAC address & clock sequence ) for current system
 *
 * @note platform dependent may caught exception
 * @note but this didn't apply inside this body function
 *
 * @return std::string
 */
std::string v1();

/**
 * @brief generate uuid v4 for current system
 *
 * @note platform dependent may caught exception
 * @note but this didn't apply inside this body function
 *
 * @return std::string
 */
std::string v4();

/**
 * @brief format uuid to ref
 *
 * @note i.e. b135d6d2-5b93-4310-92d8-25f271535de7 to b135d6d25b93431092d825f271535de7
 *
 * @param uuid_string
 * @return std::string
 */
std::string format_uuid_to_ref(const std::string& uuid_string);

/**
 * @brief format ref to uuid
 *
 * @note i.e. b135d6d25b93431092d825f271535de7 to b135d6d2-5b93-4310-92d8-25f271535de7
 *
 * @param ref_string
 * @return std::string
 */
std::string format_ref_to_uuid(const std::string& ref_string);
} // namespace uuid

} // namespace generate

namespace string {

/**
 * @brief convert input as buffer with size requirement
 *
 * @param input
 * @param required_size
 * @return buffer_t
 */
buffer_t to_buffer(const std::string& input, size_t required_size);

/**
 * @brief convert input to another letter case
 *
 * @param input
 * @param mode 0:lowercase, 1:uppercase, 2:mixcase
 * @return std::string
 */
std::string to_another_letter_case(const std::string& input, const int32_t& mode);

} // namespace string

#if BEHH_USING_JSONCPP
namespace jsoncpp {

/**
 * @brief read json input and convert to string
 *
 * @param input
 * @param indent
 * @param precision
 * @return std::string
 */
std::string to_string(const Json::Value& input, const int32_t& indent = 4, const int32_t& precision = 16);

/**
 * @brief read csv from file path and convert it to json array
 *
 * @param csv_file_path
 * @return Json::Value
 */
Json::Value from_csv_file(const std::string& csv_file_path);

/**
 * @brief read json from file path and convert it to json object as is
 *
 * @param json_file_path
 * @return Json::Value
 */
Json::Value from_json_file(const std::string& json_file_path);

/**
 * @brief convert json from json string input
 *
 * @param input
 * @param indent
 * @param precision
 * @return Json::Value
 */
Json::Value from_string(const std::string& input, const int32_t& indent = 4, const int32_t& precision = 16);

/**
 * @brief save json input to output param as .csv
 *
 * @param input
 * @param output_file_path literal required correct extension file
 * @return int32_t 1 mean ok
 */
int32_t save_to_csv(const Json::Value& input, const std::string& output_file_path);

/**
 * @brief save json input to output param as .json
 *
 * @param input
 * @param output_file_path literal required correct extension file
 * @param indent
 * @param precision
 * @return int32_t 1 mean ok
 */
int32_t save_to_json(const Json::Value& input, const std::string& output_file_path, const int32_t& indent = 4, const int32_t& precision = 16);

} // namespace jsoncpp
#endif // BEHH_USING_JSONCPP

#if BEHH_USING_COUCHBASE_CXX_CLIENT
namespace jsontao {

/**
 * @brief read json input and convert to string
 *
 * @note jsontao doesn't provide precision
 *
 * @param input
 * @param indent
 * @param precision
 * @return std::string
 */
std::string to_string(const tao::json::value& input, const int32_t& indent = 4, const int32_t& precision = 16);

/**
 * @brief read csv from file path and convert it to json array
 *
 * @param csv_file_path
 * @return tao::json::value
 */
tao::json::value from_csv_file(const std::string& csv_file_path);

/**
 * @brief read json from file path and convert it to json object as is
 *
 * @param json_file_path
 * @return tao::json::value
 */
tao::json::value from_json_file(const std::string& json_file_path);

/**
 * @brief convert json from json string input
 *
 * @note jsontao doesn't provide precision
 *
 * @param input
 * @param indent
 * @param precision
 * @return tao::json::value
 */
tao::json::value from_string(const std::string& input, const int32_t& indent = 4, const int32_t& precision = 16);

/**
 * @brief save json input to output param as .csv
 *
 * @param input
 * @param output_file_path literal required correct extension file
 * @return int32_t 1 mean ok
 */
int32_t save_to_csv(const tao::json::value& input, const std::string& output_file_path);

/**
 * @brief save json input to output param as .
 *
 * @note jsontao doesn't provide precision
 *
 * @param input
 * @param output_file_path literal required correct extension file
 * @param indent
 * @param precision
 * @return int32_t 1 mean ok
 */
int32_t save_to_json(const tao::json::value& input, const std::string& output_file_path, const int32_t& indent = 4, const int32_t& precision = 16);

} // namespace jsontao
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

namespace read {

/**
 * @brief read file to buffer_data
 *
 * @param file_path
 * @param buffer_data
 * @param chunk_size default 1 Mb
 * @return int32_t 1 mean ok
 */
int32_t file_to_buffer(const std::string& file_path, buffer_t& buffer_data, const size_t& chunk_size = 1 * 1024 * 1024);

} // namespace read

namespace write {

/**
 * @brief write file from buffer
 *
 * @param file_path
 * @param buffer_data
 * @return int32_t 1 mean ok
 */
int32_t file_from_buffer(const std::string& file_path, const buffer_t& buffer_data);

} // namespace write

namespace file {

#if BEHH_USING_OPENSSL // could be using or later on
/**
 * @brief ecnrypt file with mode based on input file & save to output file
 *
 * @note careful with chunk_size
 *
 * @param mode 1:aes cbc, 2:aes gcm
 * @param file_input
 * @param file_output
 * @param iv
 * @param ik
 * @param chunk_size default 1 Mb
 * @return int32_t 1 mean ok
 */
int32_t encrypt(const int32_t& mode, const std::string& file_input, const std::string& file_output, const std::string& iv, const std::string& ik, const size_t& chunk_size = 1 * 1024 * 1024);

/**
 * @brief decrypt file with mode based on input file & save to output file
 *
 * @note careful with chunk_size
 *
 * @param mode 1:aes cbc, 2:aes gcm
 * @param file_input
 * @param file_output
 * @param iv
 * @param ik
 * @param chunk_size default 1 Mb
 * @return int32_t 1 mean ok
 */
int32_t decrypt(const int32_t& mode, const std::string& file_input, const std::string& file_output, const std::string& iv, const std::string& ik, const size_t& chunk_size = 1 * 1024 * 1024);
#endif // BEHH_USING_OPENSSL

} // namespace file

} // namespace utility_functions
} // namespace behh

#endif // BEHH_UTILITY_HH
