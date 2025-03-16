#ifndef COMMON_HPP_
#define COMMON_HPP_

#include <iostream>
#include <string_view>
#include <source_location>

namespace Wombat {
    inline void wassert(
        bool cond,
        std::string_view msg, 
        std::source_location loc = std::source_location::current()
    ) {
        if(cond) return;
        std::cerr << "at " 
                  << loc.file_name() << ":"  
                  << loc.line()      << ":"
                  << loc.column()    << ": "
                  << msg << "\n";
        exit(1);
    }
}

#define WOMBAT_ASSERT(cond, msg) Wombat::wassert(cond, msg)

#endif // COMMON_HPP_