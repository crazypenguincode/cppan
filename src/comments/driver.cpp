/*
 * Copyright (c) 2016, Egor Pugin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     1. Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *     2. Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *     3. Neither the name of the copyright holder nor the names of
 *        its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "driver.h"

#include <algorithm>
#include <fstream>
#include <sstream>

// Prevent using <unistd.h> because of bug in flex.
#define YY_NO_UNISTD_H 1
#define YY_DECL 1
#include <lexer.h>
extern yy_comments::parser::symbol_type ll_commentslex(yyscan_t yyscanner, yy_comments::location &loc);

std::vector<std::string> extract_comments(const std::string &s)
{
    CommentsParserDriver driver;
    driver.parse(s);
    return driver.comments;
}

CommentsParserDriver::CommentsParserDriver()
{
}

yy_comments::parser::symbol_type CommentsParserDriver::lex()
{
	auto ret = ll_commentslex(scanner, location);
	return ret;
}

int CommentsParserDriver::parse(const std::string &s)
{
    ll_commentslex_init(&scanner);
    ll_comments_scan_string(s.c_str(), scanner);
    auto res = parse();
    ll_commentslex_destroy(scanner);

    return res;
}

int CommentsParserDriver::parse()
{
    yy_comments::parser parser(*this);
    parser.set_debug_level(debug);
    int res = parser.parse();
    return res;
}

void CommentsParserDriver::error(const yy_comments::location &l, const std::string &m)
{
    if (silent)
        return;
    std::ostringstream ss;
    ss << l << " " << m << "\n";
    if (!can_throw)
        std::cerr << ss.str();
    else
        throw std::runtime_error("Error during parse: " + ss.str());
}

void CommentsParserDriver::error(const std::string& m)
{
    if (silent)
        return;
    std::ostringstream ss;
    ss << m << "\n";
    if (!can_throw)
        std::cerr << ss.str();
    else
        throw std::runtime_error("Error during parse: " + ss.str());
}