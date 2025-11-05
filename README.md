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
- for gcc/clang, compile some library using -fPIC may require
- you may need to adjust CMAKE_C_COMPIPLER & CMAKE_CXX_COMPILER
- for scylladb & couchbase test, yout need to adjust as the config json file under tests dir

## highlight

[`encrypt/decrypt file`](./tests/test_file_enc_dec.cc)

[`encode/decode barcode & qrcode`](./tests/test_barcode_qrcode.cc)

[`encrypt/decrypt message aes openssl`](./tests/test_aes_openssl.cc)

<br>

---

###### end of readme

