#include <behh/functions/generate.hh>
#include <behh/functions/utility.hh>

#if BEHH_USING_STB
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#endif // STB_IMAGE_IMPLEMENTATION
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>
#endif // STB_IMAGE_WRITE_IMPLEMENTATION
#endif // BEHH_USING_STB

#include <fstream>
#include <future>
#include <iostream>
#include <thread>

namespace behh {
namespace generate_functions {

namespace image {
#if BEHH_USING_ZXING_CPP && BEHH_USING_NANOSVG
int32_t matrix_to_svg(const std::string& file_output, const BitMatrix& matrix, const int32_t& width, const int32_t& height, const int32_t& margin) {
    int32_t svg_width = width + 2 * margin;
    int32_t svg_height = height + 2 * margin;

    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"" << svg_width << "\" height=\"" << svg_height << "\">\n";

    ss << "<rect id=\"background\" width=\"" << svg_width << "\" height=\"" << svg_height << "\" fill=\"white\" />\n";

    for (int32_t y = 0; y < height; ++y) {
        for (int32_t x = 0; x < width; ++x) {
            if (matrix.get(x, y)) {
                ss << "<rect x=\"" << (x + margin) << "\" y=\"" << (y + margin) << "\" width=\"1\" height=\"1\" fill=\"black\" />\n";
            }
        }
    }
    ss << "</svg>\n";

    std::ofstream file(file_output);

    try {
        if (!file.is_open()) {
            std::cerr << "image::matrix_to_svg: fail to open " << file_output << "\n";
            return -2;
        }

        file << ss.str();
        file.close();
    } catch (const std::exception& e) {
        std::cerr << e.what() << '\n';
        return -69;
    }

    return 1;
}
#endif // BEHH_USING_ZXING_CPP && BEHH_USING_NANOSVG

#if BEHH_USING_STB

int32_t buffer_to_png(const std::string& file_output, const buffer_t& buffer, const int32_t& width, const int32_t& height, const int32_t& stb_comp) {
    if (utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_PNG_HINT) != 1) {
        return -1;
    }

    return stbi_write_png(file_output.c_str(), width, height, stb_comp, buffer.data(), (width * 4));
}

int32_t buffer_to_jpg(const std::string& file_output, const buffer_t& buffer, const int32_t& width, const int32_t& height, const int32_t& stb_comp) {
    if (utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_JPG_HINT) != 1 || utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_JPEG_HINT) != 1) {
        return -1;
    }

    return stbi_write_jpg(file_output.c_str(), width, height, stb_comp, buffer.data(), (width * 4));
}
#endif // BEHH_USING_STB
} // namespace image

namespace rasterize {
#if BEHH_USING_NANOSVG
int32_t from_svg(const std::string& file_path, std::vector<uint8_t>& output, int32_t& width, int32_t& height) {
    if (utility_functions::find::input_ends_with(file_path, FILE_EXTENSION_SVG_HINT) != 1) {
        std::cerr << "rasterize::from_svg: file is not ended with .svg\n";
        return -1;
    }

    auto fimage = std::async(std::launch::async, nsvgParseFromFile, file_path.c_str(), "px", 96.0f);

    fimage.wait(); // well...

    NSVGimage* pImage = fimage.get();

    if (!pImage) {
        std::cerr << "rasterize::from_svg: failed to load .svg file \"" << file_path << "\"\n";
        return -2;
    }

    // default output dimensions
    if (width < 256 || height < 256) {
        width = 256;
        height = static_cast<int32_t>(256 * pImage->height / pImage->width);
    }

    // rasterizer context
    NSVGrasterizer* pRast = nsvgCreateRasterizer();
    if (!pRast) {
        std::cerr << "rasterize::from_svg: failed to create rasterizer\n";
        nsvgDelete(pImage);
        return -3;
    }

    // allocate memory for the output image (RGBA format)
    std::vector<uint8_t> rgbaImage(width * height * 4);
    nsvgRasterize(pRast, pImage, 0, 0, width / pImage->width, rgbaImage.data(), width, height, width * 4);

    // convert the RGBA pixels to grayscale
    output.resize(width * height);
    for (int32_t y = 0; y < height; y++) {
        for (int32_t x = 0; x < width; x++) {
            int index = (y * width + x) * 4;  // 4 bytes per pixel (RGBA)
            uint8_t r = rgbaImage[index];     // R
            uint8_t g = rgbaImage[index + 1]; // G
            uint8_t b = rgbaImage[index + 2]; // B

            // convert RGB to grayscale using the luminance formula
            output[y * width + x] = static_cast<uint8_t>(
                0.2126 * r + 0.7152 * g + 0.0722 * b);
        }
    }

    // clean up
    nsvgDeleteRasterizer(pRast);
    nsvgDelete(pImage);

    return 1;
}
#endif // BEHH_USING_NANOSVG
} // namespace rasterize

namespace barqr {
#if BEHH_USING_ZXING_CPP && BEHH_USING_STB && BEHH_USING_NANOSVG
int32_t encode(const std::string& content, const std::string& file_output, const int32_t& width, const int32_t& height, const int32_t& margin, const BarcodeFormat& format, const int32_t& channels) {
    try {
        int32_t _width = width, _height = height, _margin = margin, _channels = channels;

        if (_width <= 256) {
            _width = 256;
        }
        if (_height <= 256) {
            _height = 256;
        }
        if (_margin <= 0) {
            _margin = 0;
        }

        BitMatrix matrix;
        MultiFormatWriter writer(format);

        writer.setMargin(_margin);

        matrix = writer.encode(content, _width, _height);

        // vector to hold pixel data for the image channels
        // add channels modifier?
        buffer_t image_data(_width * _height * _channels);

        // fill image data based on the BitMatrix (1 is black, 0 is white)
        for (int y = 0; y < _height; ++y) {
            for (int x = 0; x < _width; ++x) {
                int32_t pixel_index = (y * _width + x) * 4; // RGBA

                if (matrix.get(x, y)) {
                    // black pixel (RGBA = 0, 0, 0, 255)
                    image_data[pixel_index] = 0;       // Red
                    image_data[pixel_index + 1] = 0;   // Green
                    image_data[pixel_index + 2] = 0;   // Blue
                    image_data[pixel_index + 3] = 255; // Alpha (fully opaque)
                } else {
                    // white pixel (RGBA = 255, 255, 255, 255)
                    image_data[pixel_index] = 255;     // Red
                    image_data[pixel_index + 1] = 255; // Green
                    image_data[pixel_index + 2] = 255; // Blue
                    image_data[pixel_index + 3] = 255; // Alpha (fully opaque)
                }
            }
        }

        if (utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_PNG_HINT) == 1) {
            return image::buffer_to_png(file_output, image_data, _width, _height);
        } else if (utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_JPG_HINT) == 1) {
            return image::buffer_to_jpg(file_output, image_data, _width, _height);
        } else if (utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_JPEG_HINT) == 1) {
            return image::buffer_to_jpg(file_output, image_data, _width, _height);
        } else if (utility_functions::find::input_ends_with(file_output, FILE_EXTENSION_SVG_HINT) == 1) {
            return image::matrix_to_svg(file_output.c_str(), matrix, _width, _height, _margin);
        } else {
            return image::matrix_to_svg(file_output.c_str(), matrix, _width, _height, _margin);
        }
    } catch (const std::exception& e) {
        std::cerr << "barqr::encode: exception: " << e.what() << '\n';
        return -69;
    }
}

int32_t decode(const std::string& content, std::string& file_output, const BarcodeFormat& format, const bool& tryhard) {
    try {
        bool isOk = false;
        int32_t width, height, channels;

        std::vector<uint8_t> luminance_data;

        if (content.size() >= 4 && content.substr(content.size() - 4) == ".svg") {
            if (rasterize::from_svg(content, luminance_data, width, height) != 1) {
                std::cerr << "barqr::decode: failed to render svg: \"" << content << "\"\n";
                return -1;
            }
            isOk = true;
        } else {
            auto image_data = stbi_load(content.c_str(), &width, &height, &channels, 0);

            if (!image_data) {
                std::cerr << "barqr::decode: image data can't be loaded from \"" << content << "\"\n";
                return -2;
            }

            if (channels == 3 || channels == 4) // RGB or TGBA
            {
                luminance_data.resize(width * height);

                for (int32_t y = 0; y < height; y++) {
                    for (int32_t x = 0; x < width; x++) {
                        int32_t source = (y * width + x) * channels;
                        int32_t destination = y * width + x;

                        luminance_data[destination] = static_cast<uint8_t>(
                            0.2126 * image_data[source] +     // R
                            0.7152 * image_data[source + 1] + // G
                            0.0722 * image_data[source + 2]   // B
                        );
                    }
                }
            } else if (channels == 1) // grayscale
            {
                luminance_data.assign(image_data, image_data + width * height);
            } else {
                std::cerr << "barcode::decode: error image channel count (" << channels << ") is not supported\n";
                stbi_image_free(image_data);
                return -3;
            }

            stbi_image_free(image_data);

            isOk = true;
        }

        if (!isOk) {
            return -4;
        }

        ZXing::ImageView imageView(luminance_data.data(), width, height, ImageFormat::Lum);

        ReaderOptions option;

        option.setTryHarder(tryhard);
        option.setFormats(format); // why?
        // option.setFormats(ZXing::BarcodeFormat::Any);

        auto result = ZXing::ReadBarcode(imageView, option);

        if (!result.isValid()) {
            std::cerr << "barqr::decode result invalid, format is \"" << ZXing::ToString(result.format()) << "\"\n";
            return -5;
        }

        file_output = result.text();
    } catch (const std::exception& e) {
        std::cerr << "barqr::decode: exception: " << e.what() << '\n';
        return -69;
    }

    return 1;
}
#endif // BEHH_USING_ZXING_CPP && BEHH_USING_STB && BEHH_USING_NANOSVG
} // namespace barqr

} // namespace generate_functions
} // namespace behh
