#include <behh/functions/cryptography.hh>
#include <behh/functions/utility.hh>

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <unordered_map>

namespace behh {
namespace utility_functions {

int32_t file_exists(const std::string& file_path) {
    std::ifstream file(file_path);
    return file.good() ? 1 : -1;
}

bool is_numeric(const std::string& input) {
    if (input.empty())
        return false;
    if (input == "0")
        return true;
    if (input[0] == '-') {
        if (input.size() == 1)
            return false;
        if (input.size() == 2 && input[1] == '0')
            return true; // handle "-0"
        if (input[1] == '0')
            return false; // leading zero after '-'
        if (!std::all_of(input.begin() + 1, input.end(), ::isdigit))
            return false;
    } else {
        if (input[0] == '0' && input.size() > 1)
            return false; // leading zero
        if (!std::all_of(input.begin(), input.end(), ::isdigit))
            return false;
    }

    // try conversion to int64_t is possible
    try {
        std::stoll(input);
    } catch (...) {
        return false;
    }
    return true;
}

bool is_numeric_decimal(const std::string& input) {
    if (input.empty())
        return false;

    size_t dot_pos = input.find('.');
    if (dot_pos == std::string::npos || dot_pos == 0 || dot_pos == input.size() - 1) {
        return false; // wrong decimal format: no dot, dot at start, or dot at end
    }

    std::string integer_part = input.substr(0, dot_pos);
    std::string fractional_part = input.substr(dot_pos + 1);

    // validate integer part
    if (integer_part == "0" || integer_part == "-0") {
        // ok for "0" or "-0"
    } else {
        if (integer_part[0] == '-') {
            if (integer_part.size() == 1 || integer_part[1] == '0')
                return false;
            // ensure no leading zeros after '-' and all characters are digits
            if (!std::all_of(integer_part.begin() + 1, integer_part.end(), ::isdigit))
                return false;
        } else {
            if (integer_part[0] == '0' && integer_part.size() > 1)
                return false; // leading zero
            if (!std::all_of(integer_part.begin(), integer_part.end(), ::isdigit))
                return false;
        }
    }

    // validate fractional part
    if (!std::all_of(fractional_part.begin(), fractional_part.end(), ::isdigit))
        return false;

    // try conversion to double is possible
    try {
        std::stod(input);
    } catch (...) {
        return false;
    }

    return true;
}

bool is_numeric_unsigned(const std::string& input) {
    if (input.empty() || input[0] == '-')
        return false;
    if (input == "0")
        return true;
    if (input[0] == '0' && input.size() > 1)
        return false; // leading zero
    if (!std::all_of(input.begin(), input.end(), ::isdigit))
        return false;

    // try conversion to uint64_t is possible
    try {
        std::stoull(input);
    } catch (...) {
        return false;
    }

    return true;
}

std::string base64encode(const std::string& input) {
    std::string result;
    size_t len = input.size();
    size_t i = 0;

    while (i < len) {
        uint8_t octet_a = i < len ? input[i++] : 0;
        uint8_t octet_b = i < len ? input[i++] : 0;
        uint8_t octet_c = i < len ? input[i++] : 0;

        uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

        result.push_back(BASE64_CHARS[(triple >> 18) & 0x3F]);
        result.push_back(BASE64_CHARS[(triple >> 12) & 0x3F]);

        if (i > len) {
            // only one byte processed (octet_a)
            result.push_back('=');
            result.push_back('=');
        } else if (i > len - 1) {
            // two bytes processed (octet_a and octet_b)
            result.push_back(BASE64_CHARS[(triple >> 6) & 0x3F]);
            result.push_back('=');
        } else {
            // three bytes processed
            result.push_back(BASE64_CHARS[(triple >> 6) & 0x3F]);
            result.push_back(BASE64_CHARS[triple & 0x3F]);
        }
    }

    return result;
}

std::string base64decode(const std::string& input) {
    std::unordered_map<char, uint8_t> char_to_value;
    for (size_t i = 0; i < BASE64_CHARS.size(); ++i) {
        char_to_value[BASE64_CHARS[i]] = static_cast<uint8_t>(i);
    }

    std::string decoded;
    size_t len = input.size();

    if (len % 4 != 0) {
        return decoded;
    }

    for (size_t i = 0; i < len; i += 4) {
        uint8_t v1 = char_to_value[input[i]];
        uint8_t v2 = char_to_value[input[i + 1]];
        uint8_t v3 = (input[i + 2] != '=') ? char_to_value[input[i + 2]] : 0;
        uint8_t v4 = (input[i + 3] != '=') ? char_to_value[input[i + 3]] : 0;

        uint32_t quad = (v1 << 18) | (v2 << 12) | (v3 << 6) | v4;

        decoded.push_back(static_cast<char>((quad >> 16) & 0xFF));
        if (input[i + 2] != '=') {
            decoded.push_back(static_cast<char>((quad >> 8) & 0xFF));
        }
        if (input[i + 3] != '=') {
            decoded.push_back(static_cast<char>(quad & 0xFF));
        }
    }

    return decoded;
}

#if BEHH_USING_OPENSSL
std::string base64encode_openssl(const std::string& input) {
    BIO *bio, *b64;
    BUF_MEM* bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, input.c_str(), (int)input.length());
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);

    std::string result(bufferPtr->data, bufferPtr->length);

    BIO_free_all(bio);
    // BIO_free_all(b64); // redundant section, if single openssl linked

    return result;
}

std::string base64decode_openssl(const std::string& input) {
    BIO *bio, *b64;

    bio = BIO_new_mem_buf(input.c_str(), -1);
    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_push(b64, bio);

    char* output = new char[input.size()];

    int decodedLength = BIO_read(bio, output, (int)input.size());

    BIO_free_all(bio);
    // BIO_free_all(b64); // redundant section, if single openssl linked

    std::string result(output, decodedLength);

    delete[] output;

    return result;
}
#endif // BEHH_USING_OPENSSL

namespace find {

std::vector<std::string> each_keywords(const std::string& source, const std::vector<std::string>& keywords) {
    std::vector<std::string> result;

    for (auto& keyword : keywords) {
        if (source.find(keyword) != std::string::npos) {
            result.emplace_back(keyword);
        }
    }

    return result;
}

int32_t and_replace_all(std::string& source, const std::string& query, const std::string& replacement) {
    try {
        size_t position = 0;

        while ((position = source.find(query, position)) != std::string::npos) {
            source.replace(position, query.size(), replacement);
            position += replacement.size();
        }

        return 1;
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -1;
    }
}

int32_t input_ends_with(const std::string& input, const std::string& keyword) {
    if (input.length() < keyword.length()) {
        return -1;
    }

    return input.compare(input.length() - keyword.length(), keyword.length(), keyword) == 0 ? 1 : -2;
}

int32_t keyword_after(const std::string& source, const std::string& keyword, std::string& extraction_result) {
    size_t pos = source.find(keyword);

    if (pos == std::string::npos) {
        return -1;
    }

    extraction_result = source.substr(pos + keyword.length());
    return 1;
}

int32_t keyword_before(const std::string& source, const std::string& keyword, std::string& extraction_result) {
    size_t pos = source.find(keyword);

    if (pos == std::string::npos) {
        return -1;
    }

    extraction_result = source.substr(0, pos);
    return 1;
}

} // namespace find

namespace generate {

int32_t random_number(const int32_t& min, const int32_t& max) {
    std::random_device rand;
    std::default_random_engine rande(rand());

    std::uniform_int_distribution<int32_t> evaluation(min, max);

    return evaluation(rande);
}

int64_t random_number(const int64_t& min, const int64_t& max) {
    std::random_device rand;
    std::mt19937_64 rande(rand());

    std::uniform_int_distribution<int64_t> evaluation(min, max);

    return evaluation(rande);
}

float32_t random_number(const float32_t& min, const float32_t& max) {
    std::random_device rand;
    std::default_random_engine rande(rand());

    std::uniform_real_distribution<float32_t> evaluation(min, max);

    return evaluation(rande);
}

float64_t random_number(const float64_t& min, const float64_t& max) {
    std::random_device rand;
    std::mt19937_64 rande(rand());

    std::uniform_real_distribution<float64_t> evaluation(min, max);

    return evaluation(rande);
}

std::array<uint8_t, 16> random_bytes() {
    std::array<uint8_t, 16> bytes;

    std::random_device rand;
    std::mt19937 rande(rand());

    std::uniform_int_distribution<int32_t> evaluation(0, 255);

    for (auto& byte : bytes) {
        byte = static_cast<uint8_t>(evaluation(rande));
    }

    return bytes;
}

std::string random_alphanumeric(size_t length) {
    std::string result;

    if (length <= 0) {
        length = 1;
    }

    result.resize(length);

    for (size_t i = 0; i < length; i++) {
        result += ALPHANUMERIC[random_number(0, (int)ALPHANUMERIC.length() - 1)];
    }

    return result;
}

std::string random_alphanumeric_with_special_character(size_t length) {
    std::string result;

    if (length <= 0) {
        length = 1;
    }

    result.resize(length);

    for (size_t i = 0; i < length; i++) {
        result += ALPHANUMERIC_WITH_SPECIAL_CHARACTER[random_number(0, (int)ALPHANUMERIC_WITH_SPECIAL_CHARACTER.length() - 1)];
    }

    return result;
}

namespace uuid {
std::string v1() {
    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    auto timestamp = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();

    // byte sequence
    std::array<uint8_t, 16> uuid_bytes = random_bytes();

    // timestamp
    uuid_bytes[0] = (timestamp >> 40) & 0xFF;
    uuid_bytes[1] = (timestamp >> 32) & 0xFF;
    uuid_bytes[2] = (timestamp >> 24) & 0xFF;
    uuid_bytes[3] = (timestamp >> 16) & 0xFF;
    uuid_bytes[4] = (timestamp >> 8) & 0xFF;
    uuid_bytes[5] = timestamp & 0xFF;

    // version 1
    uuid_bytes[6] = (uuid_bytes[6] & 0x0F) | 0x10;

    // variant to 10xx
    uuid_bytes[8] = (uuid_bytes[8] & 0x3F) | 0x80;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (size_t i = 0; i < uuid_bytes.size(); ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            oss << '-';
        }
        oss << std::setw(2) << static_cast<int32_t>(uuid_bytes[i]);
    }

    return oss.str();
}

std::string v4() {
    auto bytes = random_bytes();

    // version 4
    bytes[6] = (bytes[6] & 0x0F) | 0x40;

    // variant 10xx (RFC4122)
    bytes[8] = (bytes[8] & 0x3F) | 0x80;

    std::ostringstream oss;
    oss << std::hex << std::setfill('0');

    for (size_t i = 0; i < bytes.size(); ++i) {
        if (i == 4 || i == 6 || i == 8 || i == 10) {
            oss << '-';
        }
        oss << std::setw(2) << static_cast<int>(bytes[i]);
    }

    return oss.str();
}

std::string format_uuid_to_ref(const std::string& uuid_string) {
    std::string result = uuid_string;
    result.erase(std::remove(result.begin(), result.end(), '-'), result.end());
    return result;
}

std::string format_ref_to_uuid(const std::string& uuid_string) {
    return uuid_string.substr(0, 8) + "-" + uuid_string.substr(8, 4) + "-" + uuid_string.substr(12, 4) + "-" + uuid_string.substr(16, 4) + "-" + uuid_string.substr(20, 12);
}
} // namespace uuid

} // namespace generate

namespace string {

buffer_t to_buffer(const std::string& input, size_t required_size) {
    buffer_t result(input.begin(), input.end());

    if (result.size() > required_size) {
        result.reserve(required_size);
    } else if (result.size() < required_size) {
        result.resize(required_size, 0x00);
    }

    return result;
}

std::string to_another_letter_case(const std::string& input, const int32_t& mode) {
    std::string result;

    result.reserve(input.length());

    switch (mode) {
    case 0: {
        for (size_t i = 0; i < input.length(); i++) {
            result += tolower(input[i]);
        }
    } break;

    case 1: {
        for (size_t i = 0; i < input.length(); i++) {
            result += toupper(input[i]);
        }
    } break;

    case 2: {
        for (size_t i = 0; i < input.length(); i++) {
            //  0 is lower, 1 is upper
            int mixedcase = generate::random_number(0, 1);

            if (mixedcase == 0 && isalpha(input[i])) {
                result += tolower(input[i]);
            } else if (mixedcase == 1 && isalpha(input[i])) {
                result += toupper(input[i]);
            } else {
                result += input[i];
            }
        }
    } break;

    default: {
        result = input;
    } break;
    }

    return result;
}

} // namespace string

#if BEHH_USING_JSONCPP
namespace jsoncpp {

std::string to_string(const Json::Value& input, const int32_t& indent, const int32_t& precision) {
    int32_t _indent = indent, _precision = precision;

    // force some rule
    if (_indent <= 0) {
        _indent = 0;
    }
    if (_precision <= 2) {
        _precision = 2;
    }

    Json::StreamWriterBuilder writer;

    std::string _indent_string = "";
    for (auto i = 0; i < _indent; i++) {
        _indent_string += " ";
    }

    writer.settings_["indentation"] = _indent_string;
    writer.settings_["precision"] = _precision;

    return std::string(Json::writeString(writer, input));
}

Json::Value from_csv_file(const std::string& csv_file_path) {
    Json::Value result;

    if (find::input_ends_with(csv_file_path, ".csv") != 0) {
        std::cerr << "csv_file_path is not end with .csv\n";
        return result;
    }

    try {
        std::ifstream file(csv_file_path);
        std::string line;
        std::vector<std::string> headers;

        if (file.is_open()) {
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string header;
                while (std::getline(ss, header, ',')) {
                    header.erase(header.find_last_not_of(" \t\r\n") + 1);
                    header.erase(0, header.find_first_not_of(" \t\r\n"));
                    headers.push_back(header);
                }
            }

            Json::Value json(Json::arrayValue);

            while (std::getline(file, line)) {
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                std::stringstream ss(line);
                std::string cell;
                Json::Value json_obj(Json::objectValue);
                size_t header_index = 0;

                while (std::getline(ss, cell, ',')) {
                    cell.erase(cell.find_last_not_of(" \t\n") + 1);
                    cell.erase(0, cell.find_first_not_of(" \t\n"));

                    if (header_index < headers.size()) {
                        std::string upper_cell = cell;
                        std::transform(upper_cell.begin(), upper_cell.end(), upper_cell.begin(), ::toupper);

                        if (upper_cell == "TRUE") {
                            json_obj[headers[header_index]] = true;
                        } else if (upper_cell == "FALSE") {
                            json_obj[headers[header_index]] = false;
                        } else if (is_numeric(cell)) {
                            try {
                                size_t pos;
                                int intValue = std::stoi(cell, &pos);
                                if (pos == cell.size()) {
                                    json_obj[headers[header_index]] = intValue;
                                } else {
                                    json_obj[headers[header_index]] = std::stod(cell);
                                }
                            } catch (...) {
                                json_obj[headers[header_index]] = cell;
                            }
                        } else {
                            json_obj[headers[header_index]] = cell;
                        }
                    }
                    header_index++;
                }
                json.append(json_obj);
            }

            result = json;

            file.close();
        } else {
            std::cerr << "json::from_csv: can't find csv file from \"" << csv_file_path << "\"\n";
            result = Json::Value();
        }
    } catch (const std::exception& e) {
        std::cerr << "json::from_csv: " << e.what() << '\n';
        result = Json::Value();
    }

    return result;
}

Json::Value from_json_file(const std::string& json_file_path) {
    Json::Value result;

    if (find::input_ends_with(json_file_path, ".json") != 1) {
        std::cerr << "json_file_path is not end with .json\n";
        return result;
    }

    std::ifstream file(json_file_path);

    if (file.is_open()) {
        file >> result;
        file.close();
    } else {
        std::cerr << "json::from_json_file: can't find json file from \"" << json_file_path << "\"\n";
    }

    return result;
}

Json::Value from_string(const std::string& input, const int32_t& indent, const int32_t& precision) {
    Json::Value result;

    try {
        int32_t _indent = indent, _precision = precision;

        // force some rule
        if (_indent <= 0) {
            _indent = 0;
        }
        if (_precision <= 2) {
            _precision = 2;
        }

        std::string _indent_string = "";
        for (auto i = 0; i < _indent; i++) {
            _indent_string += " ";
        }

        JSONCPP_STRING err;
        Json::CharReaderBuilder builder;

        builder.settings_["indentation"] = _indent_string;
        builder.settings_["precision"] = _precision;

        const int inputLength = static_cast<int>(input.length());

        const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
        reader->parse(input.c_str(), input.c_str() + inputLength, &result, &err);
    } catch (const std::exception& e) {
        std::cerr << "exception json::from_string: " << e.what() << '\n';
    }

    return result;
}

int32_t save_to_csv(const Json::Value& input, const std::string& output_file_path) {
    if (find::input_ends_with(output_file_path, ".csv") != 0) {
        std::cerr << "json::save_to_csv: output_file_path is not end with .csv\n";
        return -1;
    }

    std::string error_message;
    Json::CharReaderBuilder reader_builder;
    std::ofstream file(output_file_path);

    if (!file.is_open()) {
        std::cerr << "json::save_to_csv: can't open file " << output_file_path << "\n";
        return -2;
    }

    if (!input.isArray() || input.empty()) {
        std::cerr << "json::save_to_csv: input should be an array\n";
        return -3;
    }
    if (input.empty()) {
        std::cerr << "json::save_to_csv: input can't be empty\n";
        return -4;
    }

    // create headers
    const Json::Value& header = input[0];
    for (auto it = header.begin(); it != header.end(); it++) {
        if (it != header.begin()) {
            file << ",";
        }
        file << it.key().asString();
    }
    file << "\n";

    // insert rows
    for (auto& row : input) {
        for (auto it = header.begin(); it != header.end(); it++) {
            if (it != header.begin()) {
                file << ",";
            }
            file << row[it.key().asString()].asString();
        }
        file << "\n";
    }
    file.close();

    return 1;
}

int32_t save_to_json(const Json::Value& input, const std::string& output_file_path, const int32_t& indent, const int32_t& precision) {
    if (find::input_ends_with(output_file_path, ".json") != 0) {
        std::cerr << "jsoncpp::save_to_json: output_file_path is not end with .json\n";
        return -1;
    }

    int32_t _indent = indent, _precision = precision;

    // force some rule
    if (_indent <= 0) {
        _indent = 0;
    }
    if (_precision <= 2) {
        _precision = 2;
    }

    std::string _indent_string = "";
    for (auto i = 0; i < _indent; i++) {
        _indent_string += " ";
    }

    Json::StreamWriterBuilder writer;

    writer.settings_["indentation"] = _indent_string;
    writer.settings_["precision"] = _precision;

    std::ofstream file(output_file_path);

    if (!file.is_open()) {
        std::cerr << "jsoncpp::save_to_json: can't write to file " << output_file_path << "\n";
        return -2;
    }

    file << Json::writeString(writer, input);
    file.close();

    return 1;
}

} // namespace jsoncpp
#endif // BEHH_USING_JSONCPP

#if BEHH_USING_COUCHBASE_CXX_CLIENT
PRAGMA_MESSAGE("maybe there will be if using BEHH_USING_JSONTAO")
namespace jsontao {

std::string to_string(const tao::json::value& input, const int32_t& indent, const int32_t& precision) {
    int32_t _indent = indent, _precision = precision;

    if (_indent <= 0) {
        _indent = 0;
    }
    if (_precision <= 2) {
        _precision = 2;
    }

    return tao::json::to_string(input, _indent);
}

tao::json::value from_csv_file(const std::string& csv_file_path) {
    tao::json::value result = tao::json::empty_array;

    if (!find::input_ends_with(csv_file_path, ".csv")) {
        std::cerr << "csv_file_path is not end with .csv\n";
        return result;
    }

    try {
        std::ifstream file(csv_file_path);
        std::string line;
        std::vector<std::string> headers;

        if (file.is_open()) {
            if (std::getline(file, line)) {
                std::stringstream ss(line);
                std::string header;
                while (std::getline(ss, header, ',')) {
                    header.erase(header.find_last_not_of(" \t\r\n") + 1);
                    header.erase(0, header.find_first_not_of(" \t\r\n"));
                    headers.push_back(header);
                }
            }

            tao::json::value json = tao::json::empty_array;

            while (std::getline(file, line)) {
                line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

                std::stringstream ss(line);
                std::string cell;
                tao::json::value json_obj = tao::json::empty_object;
                size_t header_index = 0;

                while (std::getline(ss, cell, ',')) {
                    cell.erase(cell.find_last_not_of(" \t\n") + 1);
                    cell.erase(0, cell.find_first_not_of(" \t\n"));

                    if (header_index < headers.size()) {
                        std::string upper_cell = cell;
                        std::transform(upper_cell.begin(), upper_cell.end(), upper_cell.begin(), ::toupper);

                        if (upper_cell == "TRUE") {
                            json_obj[headers[header_index]] = true;
                        } else if (upper_cell == "FALSE") {
                            json_obj[headers[header_index]] = false;
                        } else {
                            try {
                                // Try to parse as number
                                if (cell.find('.') != std::string::npos ||
                                    cell.find('e') != std::string::npos ||
                                    cell.find('E') != std::string::npos) {
                                    json_obj[headers[header_index]] = std::stod(cell);
                                } else {
                                    json_obj[headers[header_index]] = std::stoll(cell);
                                }
                            } catch (...) {
                                // If not a number, store as string
                                json_obj[headers[header_index]] = cell;
                            }
                        }
                    }
                    header_index++;
                }
                json.emplace_back(json_obj);
            }

            result = json;
            file.close();
        } else {
            std::cerr << "jsontao::from_csv: can't find csv file from \"" << csv_file_path << "\"\n";
            result = tao::json::null;
        }
    } catch (const std::exception& e) {
        std::cerr << "jsontao::from_csv: " << e.what() << '\n';
        result = tao::json::null;
    }

    return result;
}

tao::json::value from_json_file(const std::string& json_file_path) {
    tao::json::value result = tao::json::null;

    if (!find::input_ends_with(json_file_path, ".json")) {
        std::cerr << "json_file_path is not end with .json\n";
        return result;
    }

    try {
        std::ifstream file(json_file_path);
        if (file.is_open()) {
            result = tao::json::from_stream(file, json_file_path);
            file.close();
        } else {
            std::cerr << "jsontao::from_json_file: can't find json file from \"" << json_file_path << "\"\n";
        }
    } catch (const std::exception& e) {
        std::cerr << "jsontao::from_json_file: " << e.what() << '\n';
    }

    return result;
}

tao::json::value from_string(const std::string& input) {
    tao::json::value result = tao::json::null;

    try {
        result = tao::json::from_string(input);
    } catch (const std::exception& e) {
        std::cerr << "exception jsontao::from_string: " << e.what() << '\n';
    }

    return result;
}

int32_t save_to_csv(const tao::json::value& input, const std::string& output_file_path) {
    if (!find::input_ends_with(output_file_path, ".csv")) {
        std::cerr << "jsontao::save_to_csv: output_file_path is not end with .csv\n";
        return -1;
    }

    std::ofstream file(output_file_path);

    if (!file.is_open()) {
        std::cerr << "jsontao::save_to_csv: can't open file " << output_file_path << "\n";
        return -2;
    }

    if (!input.is_array() || input.get_array().empty()) {
        std::cerr << "jsontao::save_to_csv: input should be a non-empty array\n";
        return -3;
    }

    // create headers from first object
    const auto& first_obj = input[0];
    if (!first_obj.is_object()) {
        std::cerr << "jsontao::save_to_csv: array elements should be objects\n";
        return -4;
    }

    bool first = true;
    for (const auto& [key, _] : first_obj.get_object()) {
        if (!first) {
            file << ",";
        }
        file << key;
        first = false;
    }
    file << "\n";

    // insert rows
    for (const auto& row : input.get_array()) {
        if (!row.is_object()) {
            continue; // skip non-object rows
        }

        first = true;
        for (const auto& [key, _] : first_obj.get_object()) {
            if (!first) {
                file << ",";
            }
            const auto* val = row.find(key);
            if (val != nullptr) {
                if (val->is_string()) {
                    file << val->get_string();
                } else {
                    file << tao::json::to_string(*val);
                }
            }
            first = false;
        }
        file << "\n";
    }
    file.close();

    return 1;
}

int32_t save_to_json(const tao::json::value& input, const std::string& output_file_path, const int32_t& indent, const int32_t& precision) {
    if (!find::input_ends_with(output_file_path, ".json")) {
        std::cerr << "jsontao::save_to_json: output_file_path is not end with .json\n";
        return -1;
    }

    int32_t _indent = indent;

    // force some rules
    if (_indent <= 0) {
        _indent = 0;
    }

    std::ofstream file(output_file_path);

    if (!file.is_open()) {
        std::cerr << "jsontao::save_to_json: can't write to file " << output_file_path << "\n";
        return -2;
    }

    tao::json::to_stream(file, input, _indent);
    file.close();

    return 1;
}

} // namespace jsontao
#endif // BEHH_USING_COUCHBASE_CXX_CLIENT

namespace read {

int32_t file_to_buffer(const std::string& file_path, buffer_t& buffer_data, const size_t& chunk_size) {
    std::ifstream file(file_path, std::ios::binary);

    if (!file) {
        std::cerr << "file_to_buffer: can't open file " << file_path << "\n";
        buffer_data = {};
        return -1;
    }

    buffer_t buffer(chunk_size);

    while (file) {
        file.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
        size_t bytes = file.gcount();

        if (bytes > 0) {
            buffer_data.insert(
                buffer_data.end(),
                buffer.begin(), buffer.begin() + bytes);
        }
    }

    return 1;
}

} // namespace read

namespace write {

int32_t file_from_buffer(const std::string& file_path, const buffer_t& buffer_data) {
    std::ofstream file(file_path, std::ios::binary);

    if (!file) {
        std::cerr << "file_from_buffer: can't open file " << file_path << "\n";
        return -1;
    }

    if (!file.write(reinterpret_cast<const char*>(buffer_data.data()), buffer_data.size())) {
        std::cerr << "file_from_buffer: can't write file to " << file_path << "\n";
        return -2;
    }

    return 1;
}

} // namespace write

namespace file {

#if BEHH_USING_OPENSSL
int32_t encrypt(const int32_t& mode, const std::string& file_input, const std::string& file_output, const std::string& iv, const std::string& ik, const size_t& chunk_size) {
    bool isOk = true;

    buffer_t buffer(chunk_size);
    std::ifstream file_in(file_input, std::ios::binary);

    if (!file_in) {
        std::cerr << "file::ecnrypt: can't open file_input";
        return -1;
    }

    switch (mode) {
    case 1: {
        std::ofstream file_out(file_output, std::ios::binary);

        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        buffer_t iv_buffer = string::to_buffer(iv, 16);
        buffer_t ik_buffer = string::to_buffer(ik, 16);

        while (file_in) {
            file_in.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
            size_t bytes_read = file_in.gcount();

            if (bytes_read > 0) {
                buffer_t chunk = cryptography_functions::stream_cipher::aes_cbc_encrypt_to_buffer_openssl(buffer_t(buffer.begin(), buffer.begin() + bytes_read), iv_buffer.data(), ik_buffer.data());

                if (!file_out.write(reinterpret_cast<const char*>(chunk.data()), chunk.size())) {
                    std::cerr << "file::encrypt: error encrypt at " << bytes_read << " of bytes_read\n";
                    isOk = false;
                }
            }
        }

        EVP_cleanup();
        ERR_free_strings();
    } break;

    case 2: {
        std::ofstream file_out(file_output, std::ios::binary);

        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        buffer_t iv_buffer = string::to_buffer(iv, 16);
        buffer_t ik_buffer = string::to_buffer(ik, 16);

        while (file_in) {
            file_in.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
            size_t bytes_read = file_in.gcount();

            if (bytes_read > 0) {
                buffer_t chunk = cryptography_functions::stream_cipher::aes_gcm_encrypt_to_buffer_openssl(buffer_t(buffer.begin(), buffer.begin() + bytes_read), iv_buffer.data(), ik_buffer.data());

                if (!file_out.write(reinterpret_cast<const char*>(chunk.data()), chunk.size())) {
                    std::cerr << "file::encrypt: error encrypt at " << bytes_read << " of bytes_read\n";
                    isOk = false;
                }
            }
        }

        EVP_cleanup();
        ERR_free_strings();
    } break;

    default: {
        std::cerr << "file::encrypt: mode is not support";
        isOk = false;
    } break;
    }

    if (!isOk) {
        return -69;
    } // mode error

    return 1;
}

int32_t decrypt(const int32_t& mode, const std::string& file_input, const std::string& file_output, const std::string& iv, const std::string& ik, const size_t& chunk_size) {
    bool isOk = true;

    buffer_t buffer(chunk_size);
    std::ifstream file_in(file_input, std::ios::binary);

    if (!file_in) {
        std::cerr << "file::decrypt: can't open file_input";
        return -1;
    }

    switch (mode) {
    case 1: {
        std::ofstream file_out(file_output, std::ios::binary);

        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        buffer_t iv_buffer = string::to_buffer(iv, 16);
        buffer_t ik_buffer = string::to_buffer(ik, 16);

        while (file_in) {
            file_in.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
            size_t bytes_read = file_in.gcount();

            if (bytes_read > 0) {
                buffer_t chunk = cryptography_functions::stream_cipher::aes_cbc_decrypt_to_buffer_openssl(buffer_t(buffer.begin(), buffer.begin() + bytes_read), iv_buffer.data(), ik_buffer.data());

                if (!file_out.write(reinterpret_cast<const char*>(chunk.data()), chunk.size())) {
                    std::cerr << "file::decrypt: error decrypt at " << bytes_read << " of bytes_read\n";
                    isOk = false;
                }
            }
        }

        EVP_cleanup();
        ERR_free_strings();
    } break;

    case 2: {
        std::ofstream file_out(file_output, std::ios::binary);

        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        buffer_t iv_buffer = string::to_buffer(iv, 16);
        buffer_t ik_buffer = string::to_buffer(ik, 16);

        while (file_in) {
            file_in.read(reinterpret_cast<char*>(buffer.data()), chunk_size);
            size_t bytes_read = file_in.gcount();

            if (bytes_read > 0) {
                buffer_t chunk = cryptography_functions::stream_cipher::aes_gcm_decrypt_to_buffer_openssl(buffer_t(buffer.begin(), buffer.begin() + bytes_read), iv_buffer.data(), ik_buffer.data());

                if (!file_out.write(reinterpret_cast<const char*>(chunk.data()), chunk.size())) {
                    std::cerr << "file::decrypt: error decrypt at " << bytes_read << " of bytes_read\n";
                    isOk = false;
                }
            }
        }

        EVP_cleanup();
        ERR_free_strings();
    } break;

    default: {
        std::cerr << "file::decrypt: mode is not support";
        isOk = false;
    } break;
    }

    if (!isOk) {
        return -69;
    } // mode error

    return 1;
}
#endif // BEHH_USING_OPENSSL

} // namespace file

} // namespace utility_functions
} // namespace behh
