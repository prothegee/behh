#include <behh/functions/generate.hh>
#include <cassert>
#include <iostream>

int main() {
#if BEHH_USING_STB && BEHH_USING_NANOSVG && BEHH_USING_ZXING_CPP
    // barcode: encode & decode to both svg and png
    {
        std::string in_1 = "my barcode in 1";
        BarcodeFormat format = BarcodeFormat::Code128;

        std::string out_1_svg_file = BEHH_TESTS_DIR "/my-barcode-out-1.svg";
        std::string out_1_png_file = BEHH_TESTS_DIR "/my-barcode-out-1.png";

        auto encode_svg = behh::generate_functions::barqr::encode(in_1, out_1_svg_file, 256, 256, 0, format);
        assert(encode_svg == 1);
        std::cout << "passed: encode barcode svg\n";

        auto encode_png = behh::generate_functions::barqr::encode(in_1, out_1_png_file, 256, 256, 0, format);
        assert(encode_png == 1);
        std::cout << "passed: encode barcode png\n";

        std::string out_1_svg = "n/a";
        auto decode_svg = behh::generate_functions::barqr::decode(out_1_svg_file, out_1_svg, format);
        assert(decode_svg == 1);
        std::cout << "passed: decode barcode svg\n";

        std::string out_1_png = "n/a";
        auto decode_png = behh::generate_functions::barqr::decode(out_1_png_file, out_1_png, format);
        assert(decode_png == 1);
        std::cout << "passed: decode barcode png\n";
    }

    // qrcode: encode & decode to both svg and png
    {
        std::string in_1 = "my qrcode in 1";
        BarcodeFormat format = BarcodeFormat::QRCode;

        std::string out_1_svg_file = BEHH_TESTS_DIR "/my-qrcode-out-1.svg";
        std::string out_1_png_file = BEHH_TESTS_DIR "/my-qrcode-out-1.png";

        auto encode_svg = behh::generate_functions::barqr::encode(in_1, out_1_svg_file, 256, 256, 0, format);
        assert(encode_svg == 1);
        std::cout << "passed: encode qrcode svg\n";

        auto encode_png = behh::generate_functions::barqr::encode(in_1, out_1_png_file, 256, 256, 0, format);
        assert(encode_png == 1);
        std::cout << "passed: encode qrcode png\n";

        std::string out_1_svg = "n/a";
        auto decode_svg = behh::generate_functions::barqr::decode(out_1_svg_file, out_1_svg, format);
        assert(decode_svg == 1);
        std::cout << "passed: decode qrcode svg\n";

        std::string out_1_png = "n/a";
        auto decode_png = behh::generate_functions::barqr::decode(out_1_png_file, out_1_png, format);
        assert(decode_png == 1);
        std::cout << "passed: decode qrcode png\n";
    }

// encode decode batch?
#endif // BEHH_USING_STB && BEHH_USING_NANOSVG && BEHH_USING_ZXING_CPP

    return 0;
}
