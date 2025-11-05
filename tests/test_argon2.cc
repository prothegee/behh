#include <behh/functions/cryptography.hh>
#include <cassert>
#include <iostream>

int main() {
#if BEHH_USING_ARGON2
    std::string password = "strong123!", salt = "abcdefghijklmnop", result;

    auto status = behh::cryptography_functions::hash::argon2id(password, salt, result);

    assert(status == (int32_t)ARGON2_OK);
    std::cout << "passed: argon2id as " << result << "\n";

    auto verify = behh::cryptography_functions::hash::argon2id_verifier(password, result);
    assert(verify == (int32_t)ARGON2_OK);
    std::cout << "passed: argon2id verified\n";
#endif // BEHH_USING_ARGON2

    return 0;
}
