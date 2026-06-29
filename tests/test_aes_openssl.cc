#include <behh/functions/cryptography.hh>
#include <cassert>
#include <iostream>

int main() {
#if BEHH_USING_OPENSSL
    std::string message = "this is our message", encrypted, decrypted;
    std::string iv = "2TpQjkQLRsj5Zxgc";
    std::string ik = "zu7yvTKAH8C5c2V6Ja7VBG4U7fNjjkGL";

    assert(behh::cryptography_functions::stream_cipher::aes_cbc_encrypt_openssl(message, encrypted, iv, ik) == 1);
    std::cout << "passed: encrypt aes cbc openssl\n";

    assert(behh::cryptography_functions::stream_cipher::aes_cbc_decrypt_openssl(encrypted, decrypted, iv, ik) == 1);
    std::cout << "passed: decrypt aes cbc openssl\n";

    assert(message == decrypted);
    std::cout << "passed: cbc message is original\n";

    std::string wrong_ik = "00000000000000000000000000000000";

    std::string cbc_wrong_decrypted;

    std::cout << "note: the next line is the expected rejection error\n";
    assert(behh::cryptography_functions::stream_cipher::aes_cbc_decrypt_openssl(encrypted, cbc_wrong_decrypted, iv, wrong_ik) != 1);
    std::cout << "passed: cbc wrong key rejected\n";

    std::string gcm_encrypted, gcm_decrypted;

    assert(behh::cryptography_functions::stream_cipher::aes_gcm_encrypt_openssl(message, gcm_encrypted, iv, ik) == 1);
    std::cout << "passed: encrypt aes gcm openssl\n";

    assert(behh::cryptography_functions::stream_cipher::aes_gcm_decrypt_openssl(gcm_encrypted, gcm_decrypted, iv, ik) == 1);
    std::cout << "passed: decrypt aes gcm openssl\n";

    assert(message == gcm_decrypted);
    std::cout << "passed: gcm message is original\n";

    std::string gcm_wrong_decrypted;

    std::cout << "note: the next line is the expected rejection error\n";
    assert(behh::cryptography_functions::stream_cipher::aes_gcm_decrypt_openssl(gcm_encrypted, gcm_wrong_decrypted, iv, wrong_ik) != 1);
    std::cout << "passed: gcm wrong key rejected\n";
#endif // BEHH_USING_OPENSSL

    return 0;
}
