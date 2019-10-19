// Copyright (C) 2019 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

#include <storm/log/LogColorizer.hpp>

using namespace storm::log;

LogColorizer::LogColorizer() {
}

#if defined(STORM_OS_WINDOWS)
#include <Windows.h>

static constexpr const std::uint8_t KBLA = 0;
static constexpr const std::uint8_t KRED = 12;
static constexpr const std::uint8_t KGRN = 2;
static constexpr const std::uint8_t KYEL = 14;
static constexpr const std::uint8_t KBLU = 9;
static constexpr const std::uint8_t KMAG = 13;
static constexpr const std::uint8_t KCYN = 11;
static constexpr const std::uint8_t KWHT = 15;
static constexpr const std::uint8_t KGRS = 8;

/////////////////////////////////////
/////////////////////////////////////
void colorify(std::string severity) {
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

	std::uint8_t background = KBLA;
	std::uint8_t text		= KWHT;
	if (severity.at(1) == 'I') {
		background = KGRN;
		text	   = KGRS;
	} else if (severity.at(1) == 'W') {
		background = KMAG;
		text	   = KGRS;
	}
	if (severity.at(1) == 'E') {
		background = KYEL;
		text	   = KGRS;
	}
	if (severity.at(1) == 'F') {
		background = KRED;
		text	   = KGRS;
	}
	if (severity.at(1) == 'D') {
		background = KCYN;
		text	   = KGRS;
	}

	SetConsoleTextAttribute(handle, (background << 4) + text);
	std::cout << severity;
	SetConsoleTextAttribute(handle, (KBLA << 4) + KWHT);
}
#elif defined(STORM_OS_IOS)
/////////////////////////////////////
/////////////////////////////////////
void colorify(std::string severity) {
	/*if(severity == "[Information] ")

	 else if(severity == "[Warning] ")

	 if(severity == "[Error] ")

	 if(severity == "[FatalError] ")

	 if(severity == "[Debug] ")*/
	std::cout << severity;
}
#elif defined(STORM_POSIX)

static constexpr const char *const KNRM = "\x1B[0m";
static constexpr const char *const KRED = "\x1B[31m";
static constexpr const char *const KGRN = "\x1B[32m";
static constexpr const char *const KYEL = "\x1B[33m";
static constexpr const char *const KBLU = "\x1B[34m";
static constexpr const char *const KMAG = "\x1B[35m";
static constexpr const char *const KCYN = "\x1B[36m";
static constexpr const char *const KWHT = "\x1B[37m";
static constexpr const char *const KGRS = "\033[1m";
static constexpr const char *const KINV = "\e[7m";

/////////////////////////////////////
/////////////////////////////////////
void colorify(std::string severity) {
	if (severity.at(1) == 'I')
		std::cout << KGRS << KINV << KGRN << severity << KNRM << " ";
	else if (severity.at(1) == 'W')
		std::cout << KGRS << KINV << KMAG << severity << KNRM << " ";
	else if (severity.at(1) == 'E')
		std::cout << KGRS << KINV << KYEL << severity << KNRM << " ";
	else if (severity.at(1) == 'F')
		std::cout << KGRS << KINV << KRED << severity << KNRM << " ";
	else if (severity.at(1) == 'D')
		std::cout << KGRS << KINV << KCYN << severity << KNRM << " ";
}
#endif
