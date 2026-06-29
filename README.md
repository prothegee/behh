# behh

module library treatment for c++ backend project
<!--
note:
on my old legacy code on libprcpp, some cryptography use RC6 & XChaCha20
that implementation is not available here, and gcc could create error on libprcpp

use this & that as future reference, I'm might create another module lib again, just because
-->

__*NOTE:*__
- mostly for personal usage
- for gcc/clang, compiling some library may require -fPIC
- you may need to adjust CMAKE_C_COMPIPLER & CMAKE_CXX_COMPILER
- when some functionality using filepath, it always relative from the executeable program
- for scylladb & couchbase test, yout need to adjust as the config json file under tests dir

<br>

## what this library can do

grouped by module under the `behh` namespace. some capabilities only build when their dependency is found at configure time (the optional column).

### cryptography (`behh::cryptography_functions`)

| capability | detail | optional |
| :- | :- | :- |
| hash | sha1, sha224, sha256, sha384, sha512 | openssl |
| password hash | argon2id hash and verifier | argon2 |
| symmetric cipher | aes-256-cbc, string in/out and buffer in/out | openssl |
| encoding | bytes to hex, string to hex, custom base36 (encode and decode) | openssl |

> note: only aes-256-cbc is provided today (no gcm, no 128/192 bit).

### utility (`behh::utility_functions`)

| capability | detail | optional |
| :- | :- | :- |
| base64 | encode and decode, stl based and openssl based | openssl (for the openssl path) |
| checks | file exists, numeric, floating, unsigned numeric | - |
| string find | keyword search, find and replace, ends-with, extract before/after a keyword | - |
| random | number ranges, random 16 bytes, alphanumeric, alphanumeric with special chars | - |
| uuid | v1 (mac and clock based) and v4, plus uuid to ref and ref to uuid | uuid |
| string | letter case convert, convert to sized buffer | - |
| json | jsoncpp and taocpp-json: read, convert, save as .json and .csv, csv to json array | jsoncpp, taocpp-json |
| file io | read file to buffer, write buffer to file | - |
| file crypto | encrypt and decrypt a file by mode (uses aes-256-cbc buffer path) | openssl |

### generate (`behh::generate_functions`)

| capability | detail | optional |
| :- | :- | :- |
| barcode and qrcode | encode content to .svg or .png, decode from .svg or .png | zxing-cpp, stb, nanosvg |
| image | svg from a bit matrix, png and jpg/jpeg from a pixel buffer | stb, nanosvg |
| rasterize | render an .svg file to a raw pixel output | nanosvg |

### date and time (`behh::date_and_time_functions`)

| capability | detail | optional |
| :- | :- | :- |
| timezone | local machine timezone, iso8601 string with offset | - |
| parts | year, month, day, hour, minute, second as int or string (month also as short name) | - |
| combined | yyyymmdd, hhmmss, and yyyymmddhhmmss in int, raw, and formatted forms | - |
| precision | seconds, milliseconds, microseconds, nanoseconds variants with millis conversion | - |

### communication (`behh::communication_functions`)

| capability | detail | optional |
| :- | :- | :- |
| email | secure smtp send by template using the curl executable | curl |
| email | template mail to sparkpost using drogon (future based) | drogon |

### database interfaces (`behh::interfaces`)

| capability | detail | optional |
| :- | :- | :- |
| postgresql | core interface for read and write | postgresql |
| scylladb | core interface for read and write | scylladb |
| couchbase | read and write (see test under tests dir) | couchbase-cxx-client |

### bundled vendors (available to link, no dedicated behh api yet)

these are vendored and built with the project so you can use them directly. behh does not wrap them in its own functions yet.

| vendor | note | optional |
| :- | :- | :- |
| sdl3 | usable from your code, a screen resolution example lives under tests | sdl3 |
| haru (hpdf) | pdf library is vendored and built, no behh function uses it yet | haru |
| zxing-cpp, stb, nanosvg | also power the generate module above | zxing-cpp, stb, nanosvg |

---

###### end of readme

