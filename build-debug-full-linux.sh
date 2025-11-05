# sh linux
cmake -G "Ninja" -B build/debug -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=true -DBEHH_BUILD_TOOLS=true -DBEHH_BUILD_TESTS=true -DBEHH_BUILD_VENDOR_STB=true -DBEHH_BUILD_VENDOR_SDL3=true -DBEHH_BUILD_VENDOR_HARU=true -DBEHH_BUILD_VENDOR_NANOSVG=true -DBEHH_BUILD_VENDOR_ZXING_CPP=true -DBEHH_BUILD_VENDOR_COUCHBASE_CXX_CLIENT=true -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++;

ln -sf build/debug/compile_commands.json ./;

cmake --build build/debug --target all;

