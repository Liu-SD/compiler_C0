#pragma once

#include <regex>

#include "symbolTable.h"
#include "mcode.h"

static std::regex rfunc("(void|int|char) (\\S+)\\(\\)");
static std::regex rreturn("RETURN");
static std::regex rassign2("(\\S+) = (\\S+)");
static std::regex rassign2_with_left_arr("(\\S+?)\\[(\\S+?)\\] = (\\S+)");
static std::regex rassign2_without_left_arr("([^\\s\\[\\]]+) = (\\S+)");
static std::regex rassign3("(\\S+) = (\\S+) (\\+|-|\\*|/) (\\S+)");
static std::regex rbranch("(GEZ|GZ|NEZ|EZ|LZ|LEZ) (\\S+) #(Label_\\d+_)");
static std::regex rretval("RETV (\\S+)");
static std::regex rjump("JMP #(Label_\\d+_)");
static std::regex rpush("PUSH (\\S+) (\\d+)");
static std::regex rcal("CAL (\\S+)");
static std::regex rprintv("PRINTF(N|C) (\\S+)");
static std::regex rprints("PRINTFS <(.*)>");
static std::regex rscan("SCANF(N|C) (\\S+)");

static std::regex rarr("(\\S+)\\[(\\S+)\\]");

extern void translate(EMBEDTAB embedcode);
extern void printTcode(std::string path);
