#include <behh/functions/generate.hh>
#include <cassert>
#include <iostream>

int main() {
#if BEHH_USING_STB && BEHH_USING_NANOSVG && BEHH_USING_ZXING_CPP
    // barcode
    {
        std::string in_1 = "my barcode in 1";
        std::string out_1 = "n/a";
        std::string out_1_file = "../../../tests/my barcode out 1.svg";
        BarcodeFormat format = BarcodeFormat::Code128;

        auto encode = behh::generate_functions::barqr::encode(
            in_1,
            out_1_file,
            256,
            256,
            0,
            format);
        assert(encode == 1);
        std::cout << "passed: encode barcode\n";

        auto decode = behh::generate_functions::barqr::decode(
            out_1_file,
            out_1,
            format);
        assert(decode == 1);
        std::cout << "passed: decode barcode\n";
    }

    // qrcode
    {
        std::string in_1 = "my qrcode in 1";
        std::string out_1 = "n/a";
        std::string out_1_file = "../../../tests/my qrcode out 1.png";
        BarcodeFormat format = BarcodeFormat::QRCode;

        auto encode = behh::generate_functions::barqr::encode(
            in_1,
            out_1_file,
            256,
            256,
            0,
            format);
        assert(encode == 1);
        std::cout << "passed: encode qrcode\n";

        auto decode = behh::generate_functions::barqr::decode(
            out_1_file,
            out_1,
            format);
        assert(decode == 1);
        std::cout << "passed: decode qrcode\n";
    }

// encode decode batch?
#endif // BEHH_USING_STB && BEHH_USING_NANOSVG && BEHH_USING_ZXING_CPP

    return 0;
}
