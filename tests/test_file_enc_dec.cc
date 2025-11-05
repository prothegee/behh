#include <behh/functions/utility.hh>
#include <cassert>
#include <iostream>

// all output is inside "BEHH_TESTS" of build dir
int main() {
    std::string iv_16 = "abcdefghijklmnop";
    std::string iv_24 = "abcdefghijklmnopqrstuvwx";
    std::string ik_32 = "abcdefghijklmnopqrstuvwxyz012345";

    std::string example_in1 = "../../../tests/example_in1.json";
    std::string example_out1_enc = "../../../tests/example_out1_enc.json";
    std::string example_out1_dec = "../../../tests/example_out1_dec.json";

#if BEHH_USING_OPENSSL
    auto example_in1_status = behh::utility_functions::file::encrypt(
        1,
        example_in1, example_out1_enc,
        iv_16,
        ik_32);
    assert(example_in1_status == 1);
    std::cout << "passed: example_in1_status\n";

    auto example_out1_status = behh::utility_functions::file::decrypt(
        1,
        example_out1_enc, example_out1_dec,
        iv_16,
        ik_32);
    assert(example_out1_status == 1);
    std::cout << "passed: example_out1_status\n";
#endif // BEHH_USING_OPENSSL

    return 0;
}
