#ifndef COLORDEFS_HPP
#define COLORDEFS_HPP

#include <string>

// This file has the escape codes of colors
// of the terminal.
// RESET sets the colors back to default.

static const std::string RESET = "\033[0m";

static const std::string RED = "\033[0;31m";
static const std::string GREEN = "\033[0;32m";
static const std::string YELLOW = "\033[0;33m";
static const std::string BLUE = "\033[0;34m";
static const std::string MAGENTA = "\033[0;35m";
static const std::string CYAN = "\033[0;36m";
static const std::string WHITE = "\033[0;37m";

static const std::string BOLD_RED = "\033[1;31m";
static const std::string BOLD_GREEN = "\033[1;32m";
static const std::string BOLD_YELLOW = "\033[1;33m";
static const std::string BOLD_BLUE = "\033[1;34m";
static const std::string BOLD_MAGENTA = "\033[1;35m";
static const std::string BOLD_CYAN = "\033[1;36m";
static const std::string BOLD_WHITE = "\033[1;37m";

#endif // COLORDEFS_HPP
