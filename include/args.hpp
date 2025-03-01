#ifndef SEWS_ARGS_HPP
#define SEWS_ARGS_HPP

#include <tuple>

namespace sews {
    std::tuple<int, int, int> handleArgs(int argumentCount, char* argumentVector[]);
} // namespace sews

#endif // !SEWS_ARGS_HPP
