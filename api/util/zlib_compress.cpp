#include <vector>
#include <zlib.h>
#include <cstddef> 
#include <iostream>
#include <fstream>
#include <string>

namespace util {
    //Ripped from llm
    
    std::vector<std::byte> compressGzip(const std::vector<std::byte>& data) {
        z_stream zs = {};
        deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);

        zs.next_in = reinterpret_cast<Bytef*>(const_cast<std::byte*>(data.data()));
        zs.avail_in = data.size();

        std::vector<std::byte> compressed;
        char outbuffer[32768];

        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);

            deflate(&zs, Z_FINISH);
            // Append the compressed chunk as std::byte
            compressed.insert(compressed.end(), 
                    reinterpret_cast<std::byte*>(outbuffer), 
                    reinterpret_cast<std::byte*>(outbuffer + (sizeof(outbuffer) - zs.avail_out)));
        } while (zs.avail_out == 0);

        deflateEnd(&zs);
        return compressed;
    }

    std::string compressGzip(const std::string& data) {
        z_stream zs = {};
        deflateInit2(&zs, Z_BEST_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY);
        zs.next_in = (Bytef*)data.data();
        zs.avail_in = data.size();
        std::string compressed;
        char outbuffer[32768];
        do {
            zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
            zs.avail_out = sizeof(outbuffer);
            deflate(&zs, Z_FINISH);
            compressed.append(outbuffer, sizeof(outbuffer) - zs.avail_out);
        } while (zs.avail_out == 0);
        deflateEnd(&zs);
        return compressed;
    }

}



