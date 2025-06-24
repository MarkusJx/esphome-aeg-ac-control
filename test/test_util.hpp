#ifndef TEST_UTIL_HPP
#define TEST_UTIL_HPP

#include <bitset>
#include <string>
#include <climits>

#define IS_INTEGRAL(T) typename std::enable_if<std::is_integral<T>::value>::type * = 0
#define EXPECT_BIN_EQ(a, b) EXPECT_EQ(a, b) << "Expected " << std::uppercase                 \
                                            << std::hex << b << " but got " << std::hex << a \
                                            << std::endl                                     \
                                            << "Expected: " << to_bin(b) << std::endl        \
                                            << "But got:  " << to_bin(a)

template<class T>
inline std::string to_bin(T byte, IS_INTEGRAL(T)) {
    std::bitset<sizeof(T) * CHAR_BIT> bs(byte);
    std::string str = bs.to_string();

    std::string res;
    for (size_t i = 0; i < str.size(); i += 4) {
        if (!res.empty()) {
            res += " ";
        }

        res += str.substr(i, 4);
    }

    return res;
}

#endif// TEST_UTIL_HPP