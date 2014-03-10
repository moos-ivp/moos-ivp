#ifndef __UTILS_H
#define __UTILS_H

#include <vector>
#include <string>

namespace MOOS {
namespace AlogTools {

void Tokenize(const std::string& str,
        std::vector<std::string>& tokens,
        const std::string& delimiters = " ");

}  // namespace AlogTools
}  // namespace MOOS

#endif // __UTILS_H
