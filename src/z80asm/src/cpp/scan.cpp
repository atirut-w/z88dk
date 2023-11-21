/* Generated by re2c 3.1 */
//-----------------------------------------------------------------------------
// z80asm
// scanner
// Copyright (C) Paulo Custodio, 2011-2023
// License: The Artistic License 2.0, http://www.perlfoundation.org/artistic_license_2_0
//-----------------------------------------------------------------------------

#include "args.h"
#include "if.h"
#include "scan.h"
#include "utils.h"
#include <unordered_map>
#include <cassert>
#include <cmath>
using namespace std;

//-----------------------------------------------------------------------------



		#define YYMAXFILL 4

#define FILL_SIZE (8*1024)

//-----------------------------------------------------------------------------

static int a2i(const char* start, const char* end, int base) {
    return static_cast<int>(strtol(string(start, end).c_str(), NULL, base));
}

static double a2f(const char* start, const char* end) {
    return atof(string(start, end).c_str());
}

static string str_swap_x_y(string str) {
    for (auto& c : str) {
        switch (c) {
        case 'x': c = 'y'; break;
        case 'X': c = 'Y'; break;
        case 'y': c = 'x'; break;
        case 'Y': c = 'X'; break;
        default:;
        }
    }
    return str;
}

//-----------------------------------------------------------------------------

static int keyword_flags[] = {
#define X(id, text, flags)		flags,
#include "keyword.def"
};

Keyword keyword_lookup(const string& text) {
    static unordered_map<string, Keyword> keywords = {
#define X(id, text, flags)      { text, Keyword::id },
#include "keyword.def"
    };

    auto it = keywords.find(str_tolower(text));
    if (it == keywords.end())
        return Keyword::None;
    else
        return it->second;
}

bool keyword_is_reg_8(Keyword keyword) {
    return keyword_flags[static_cast<int>(keyword)] & KW_REG_8;
}

bool keyword_is_reg_ix_iy(Keyword keyword) {
    return keyword_flags[static_cast<int>(keyword)] & KW_REG_IX_IY;
}

bool keyword_is_z80_ld_bit(Keyword keyword) {
    return keyword_flags[static_cast<int>(keyword)] & KW_Z80_LD_BIT;
}

//-----------------------------------------------------------------------------

Token::Token(TType type, bool blank_before, int ivalue)
    : m_type(type), m_blank_before(blank_before), m_ivalue(ivalue), m_keyword(Keyword::None) {
}

Token::Token(TType type, bool blank_before, double fvalue)
    : m_type(type), m_blank_before(blank_before), m_fvalue(fvalue), m_keyword(Keyword::None) {
}

Token::Token(TType type, bool blank_before, const string& svalue)
    : m_type(type), m_blank_before(blank_before), m_svalue(svalue) {
    m_keyword = keyword_lookup(svalue);
}

string Token::to_string() const {
    static string tokens[] = {
#define X(id, text)     text,
#include "scan.def"
    };

    switch (m_type) {
    case TType::Ident:
        return m_svalue;
    case TType::Integer:
        return std::to_string(m_ivalue);
    case TType::Floating:
        return std::to_string(m_fvalue);
    case TType::String:
        return string_bytes(m_svalue);
    default:
        return tokens[static_cast<int>(m_type)];
    }
}

string Token::to_string(const vector<Token>& tokens) {
    string out = "";
    for (auto& token : tokens) {
        out = concat(out, token.to_string());
    }
    return out;
}

string Token::string_bytes(const string& text) {
    string out = "\"";
    for (auto c : text) {
        switch (c) {
        case '\a': out += "\\a"; break;
        case '\b': out += "\\b"; break;
        case '\f': out += "\\f"; break;
        case '\n': out += "\\n"; break;
        case '\r': out += "\\r"; break;
        case '\t': out += "\\t"; break;
        case '\v': out += "\\v"; break;
        case '\\': out += "\\\\"; break;
        case '"': out += "\\\""; break;
        default:
            if (c >= 0x20 && c < 0x7f)
                out.push_back(c);
            else if ((c & 0xff) < 8)
                out += "\\" + std::to_string(c);			// \o
            else {
                std::ostringstream ss;
                ss << "\\x"
                    << std::setfill('0') << std::setw(2)
                    << std::hex << static_cast<unsigned int>(c & 0xff) << std::dec;
                out += ss.str();
            }
        }
    }
    out += "\"";
    return out;
}

string Token::concat(const string& s1, const string& s2) {
    if (s1.empty() || s2.empty())
        return s1 + s2;
    else if (str_ends_with(s1, "##"))   // cpp-style concatenation
        return s1.substr(0, s1.length() - 2) + s2;
    else if (is_space(s1.back()) || is_space(s2.front()))
        return s1 + s2;
    else if (is_ident(s1.back()) && is_ident(s2.front()))
        return s1 + " " + s2;
    else if (s1.back() == '$' && is_xdigit(s2.front()))
        return s1 + " " + s2;
    else if ((s1.back() == '%' || s1.back() == '@') &&
        (is_digit(s2.front()) || s2.front() == '"'))
        return s1 + " " + s2;
    else if ((s1.back() == '&' && s2.front() == '&') ||
        (s1.back() == '|' && s2.front() == '|') ||
        (s1.back() == '^' && s2.front() == '^') ||
        (s1.back() == '*' && s2.front() == '*') ||
        (s1.back() == '<' && (s2.front() == '=' || s2.front() == '<' || s2.front() == '>')) ||
        (s1.back() == '>' && (s2.front() == '=' || s2.front() == '>')) ||
        (s1.back() == '=' && s2.front() == '=') ||
        (s1.back() == '!' && s2.front() == '=') ||
        (s1.back() == '#' && s2.front() == '#'))
        return s1 + " " + s2;
    else
        return s1 + s2;
}

//-----------------------------------------------------------------------------

ScannedLine::ScannedLine(const string& text, const vector<Token>& tokens)
    : m_text(text), m_pos(0) {
    std::copy(tokens.begin(), tokens.end(), std::back_inserter(m_tokens));
}

void ScannedLine::append(const ScannedLine& other) {
    m_text = Token::concat(m_text, other.text());
    std::copy(other.tokens().begin(), other.tokens().end(), std::back_inserter(m_tokens));
}

void ScannedLine::append(const vector<Token>& tokens) {
    ScannedLine other{ Token::to_string(tokens), tokens };
    append(other);
}

void ScannedLine::clear() {
    m_text.clear();
    m_tokens.clear();
    m_pos = 0;
}

Token& ScannedLine::peek(int offset) {
    static Token end{ TType::End, false };
    unsigned index = m_pos + offset;
    if (index >= m_tokens.size())
        return end;
    else
        return m_tokens[index];
}

void ScannedLine::next(int n) {
    m_pos += n;
    if (m_pos > m_tokens.size())
        m_pos = static_cast<unsigned>(m_tokens.size());
}

vector<Token> ScannedLine::peek_tokens(int offset) {
    vector<Token> out;
    unsigned index = m_pos + offset;
    for (unsigned i = index; i < m_tokens.size(); i++)
        out.push_back(m_tokens[i]);
    return out;
}

string ScannedLine::peek_text(int offset) {
    vector<Token> out = peek_tokens(offset);
    return Token::to_string(out);
}

//-----------------------------------------------------------------------------

FileScanner::FileScanner() {
    line_start = line_end = p = p0 = marker = limit = m_buffer.c_str();
}

bool FileScanner::open(const string& filename) {
    m_got_eof = true;
    m_buffer.clear();
    line_start = line_end = p = p0 = marker = limit = m_buffer.c_str();

    if (!fs::is_regular_file(fs::path(filename))) {
        g_errors.error(ErrCode::FileNotFound, filename);
        return false;
    }
    else {
        m_ifs.open(filename, ios::binary);
        if (!m_ifs.is_open()) {
            g_errors.error(ErrCode::FileOpen, filename);
            perror(filename.c_str());
            return false;
        }
        else {
            m_filename = filename;
            m_location = Location(filename);
            m_got_eof = false;
            fill();
            return true;
        }
    }
}

void FileScanner::scan_text(Location location, const string& text) {
    if (m_ifs.is_open())
        m_ifs.close();
    m_filename = location.filename();
    m_location = location;
    m_buffer = text;
    line_start = line_end = p = p0 = marker = m_buffer.c_str();
    limit = m_buffer.c_str() + m_buffer.size();
    m_got_error = false;
    m_got_eof = true;
}

bool FileScanner::get_text_line(ScannedLine& line) {
    line.clear();
    if (peek_text_line(line)) {
        line_start = p0 = marker = p = line_end;
        return true;
    }
    else
        return false;
}

bool FileScanner::peek_text_line(ScannedLine& line) {
    p = p0 = marker = line_end = line_start;

    while (true) {
        
		{
			char yych;
yyFillLabel0:
			yych = *p;
			switch (yych) {
				case 0x00:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel0;
						goto yy6;
					}
					goto yy1;
				case '\n': goto yy3;
				case '\r': goto yy5;
				default: goto yy2;
			}
yy1:
			++p;
			{ p--; goto end; }
yy2:
			++p;
			{ continue; }
yy3:
			++p;
yy4:
			{ m_location.inc_line(); goto end; }
yy5:
			++p;
yyFillLabel1:
			yych = *p;
			switch (yych) {
				case '\n': goto yy3;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel1;
					}
					goto yy4;
			}
yy6:
			{ goto end; }
		}

    }

end:
    if (p > line_start) {
        line_end = p;
        p = p0 = marker = line_start;
        string text = string(line_start, line_end);
        line.set_text(text);
        notify_new_line(text);
        return true;
    }
    else {
        return false;
    }
}

bool FileScanner::get_token_line(ScannedLine& line) {
    line.clear();
    string str, error;
    int quote = 0;
    bool raw_strings = g_args.raw_strings();
    m_got_error = false;
    m_blank_before = false;

    peek_text_line(line);
    p = p0 = marker = line_end = line_start;

main_loop:
    while (true) {
        p0 = p;

#define PUSH_TOKEN1(type)       do { line.tokens().emplace_back(type, m_blank_before); \
                                     m_blank_before = false; } while (0)
#define PUSH_TOKEN2(type, arg)  do { line.tokens().emplace_back(type, m_blank_before, arg); \
                                     m_blank_before = false; } while (0)
        
		{
			char yych;
			unsigned int yyaccept = 0;
yyFillLabel2:
			yych = *p;
			switch (yych) {
				case 0x00:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel2;
						goto yy106;
					}
					goto yy8;
				case '\t':
				case '\v':
				case '\f':
				case ' ': goto yy11;
				case '\n': goto yy13;
				case '\r': goto yy15;
				case '!': goto yy16;
				case '"': goto yy18;
				case '#': goto yy19;
				case '$': goto yy21;
				case '%': goto yy23;
				case '&': goto yy25;
				case '\'': goto yy27;
				case '(': goto yy28;
				case ')': goto yy29;
				case '*': goto yy30;
				case '+': goto yy32;
				case ',': goto yy33;
				case '-': goto yy34;
				case '.': goto yy35;
				case '/': goto yy37;
				case '0': goto yy38;
				case '1': goto yy40;
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy42;
				case ':': goto yy43;
				case ';': goto yy44;
				case '<': goto yy46;
				case '=': goto yy48;
				case '>': goto yy50;
				case '?': goto yy52;
				case '@': goto yy53;
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
				case 'H':
				case 'I':
				case 'J':
				case 'K':
				case 'L':
				case 'M':
				case 'N':
				case 'O':
				case 'P':
				case 'Q':
				case 'R':
				case 'S':
				case 'T':
				case 'U':
				case 'V':
				case 'W':
				case 'X':
				case 'Y':
				case 'Z':
				case '_':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
				case 'j':
				case 'k':
				case 'l':
				case 'm':
				case 'n':
				case 'o':
				case 'p':
				case 'q':
				case 'r':
				case 's':
				case 't':
				case 'u':
				case 'v':
				case 'w':
				case 'x':
				case 'y':
				case 'z': goto yy54;
				case '[': goto yy56;
				case '\\': goto yy57;
				case ']': goto yy59;
				case '^': goto yy60;
				case '{': goto yy62;
				case '|': goto yy63;
				case '}': goto yy65;
				case '~': goto yy66;
				default: goto yy9;
			}
yy8:
			++p;
			{ p--; goto end; }
yy9:
			++p;
yy10:
			{ scan_error(ErrCode::InvalidChar); continue; }
yy11:
			++p;
yyFillLabel3:
			yych = *p;
			switch (yych) {
				case '\t':
				case '\v':
				case '\f':
				case ' ': goto yy11;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel3;
					}
					goto yy12;
			}
yy12:
			{ m_blank_before = true; continue; }
yy13:
			++p;
yy14:
			{ goto end; }
yy15:
			++p;
yyFillLabel4:
			yych = *p;
			switch (yych) {
				case '\n': goto yy13;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel4;
					}
					goto yy14;
			}
yy16:
			++p;
yyFillLabel5:
			yych = *p;
			switch (yych) {
				case '=': goto yy67;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel5;
					}
					goto yy17;
			}
yy17:
			{ PUSH_TOKEN1(TType::LogNot); continue; }
yy18:
			++p;
			{ quote = 2; goto string_loop; }
yy19:
			++p;
yyFillLabel6:
			yych = *p;
			switch (yych) {
				case '#': goto yy68;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel6;
					}
					goto yy20;
			}
yy20:
			{ PUSH_TOKEN1(TType::Hash); continue; }
yy21:
			++p;
yyFillLabel7:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy69;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel7;
					}
					goto yy22;
			}
yy22:
			{ PUSH_TOKEN1(TType::ASMPC); continue; }
yy23:
			yyaccept = 0;
			marker = ++p;
yyFillLabel8:
			yych = *p;
			switch (yych) {
				case '"': goto yy71;
				case '0':
				case '1': goto yy73;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel8;
					}
					goto yy24;
			}
yy24:
			{ PUSH_TOKEN1(TType::Mod); continue; }
yy25:
			++p;
yyFillLabel9:
			yych = *p;
			switch (yych) {
				case '&': goto yy75;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel9;
					}
					goto yy26;
			}
yy26:
			{ PUSH_TOKEN1(TType::BinAnd); continue; }
yy27:
			++p;
			{ quote = 1; goto string_loop; }
yy28:
			++p;
			{ PUSH_TOKEN1(TType::LParen); continue; }
yy29:
			++p;
			{ PUSH_TOKEN1(TType::RParen); continue; }
yy30:
			++p;
yyFillLabel10:
			yych = *p;
			switch (yych) {
				case '*': goto yy76;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel10;
					}
					goto yy31;
			}
yy31:
			{ PUSH_TOKEN1(TType::Mult); continue; }
yy32:
			++p;
			{ PUSH_TOKEN1(TType::Plus); continue; }
yy33:
			++p;
			{ PUSH_TOKEN1(TType::Comma); continue; }
yy34:
			++p;
			{ PUSH_TOKEN1(TType::Minus); continue; }
yy35:
			++p;
yyFillLabel11:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy77;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel11;
					}
					goto yy36;
			}
yy36:
			{ PUSH_TOKEN1(TType::Dot); continue; }
yy37:
			++p;
			{ PUSH_TOKEN1(TType::Div); continue; }
yy38:
			yyaccept = 1;
			marker = ++p;
yyFillLabel12:
			yych = *p;
			switch (yych) {
				case 0x00:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel12;
					}
					goto yy39;
				case 'B':
				case 'b': goto yy80;
				case 'X':
				case 'x': goto yy84;
				default: goto yy41;
			}
yy39:
			{ PUSH_TOKEN2(TType::Integer, a2i(p0, p, 10)); continue; }
yy40:
			yyaccept = 1;
			marker = ++p;
yyFillLabel13:
			yych = *p;
yy41:
			switch (yych) {
				case '.': goto yy77;
				case '0':
				case '1': goto yy40;
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy42;
				case 'A':
				case 'C':
				case 'E':
				case 'F':
				case 'a':
				case 'c':
				case 'e':
				case 'f': goto yy79;
				case 'B':
				case 'b': goto yy85;
				case 'D':
				case 'd': goto yy82;
				case 'H':
				case 'h': goto yy83;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel13;
					}
					goto yy39;
			}
yy42:
			yyaccept = 1;
			marker = ++p;
yyFillLabel14:
			yych = *p;
			switch (yych) {
				case '.': goto yy77;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy42;
				case 'A':
				case 'B':
				case 'C':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'e':
				case 'f': goto yy79;
				case 'D':
				case 'd': goto yy82;
				case 'H':
				case 'h': goto yy83;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel14;
					}
					goto yy39;
			}
yy43:
			++p;
			{ PUSH_TOKEN1(TType::Colon); continue; }
yy44:
			++p;
yyFillLabel15:
			yych = *p;
			switch (yych) {
				case 0x00:
				case '\n':
				case '\r':
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel15;
					}
					goto yy45;
				default: goto yy44;
			}
yy45:
			{ continue; }
yy46:
			++p;
yyFillLabel16:
			yych = *p;
			switch (yych) {
				case '<': goto yy86;
				case '=': goto yy87;
				case '>': goto yy67;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel16;
					}
					goto yy47;
			}
yy47:
			{ PUSH_TOKEN1(TType::Lt); continue; }
yy48:
			++p;
yyFillLabel17:
			yych = *p;
			switch (yych) {
				case '=': goto yy88;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel17;
					}
					goto yy49;
			}
yy49:
			{ PUSH_TOKEN1(TType::Eq); continue; }
yy50:
			++p;
yyFillLabel18:
			yych = *p;
			switch (yych) {
				case '=': goto yy89;
				case '>': goto yy90;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel18;
					}
					goto yy51;
			}
yy51:
			{ PUSH_TOKEN1(TType::Gt); continue; }
yy52:
			++p;
			{ PUSH_TOKEN1(TType::Quest); continue; }
yy53:
			yyaccept = 2;
			marker = ++p;
yyFillLabel19:
			yych = *p;
			switch (yych) {
				case '"': goto yy71;
				case '0':
				case '1': goto yy73;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel19;
					}
					goto yy10;
			}
yy54:
			++p;
yyFillLabel20:
			yych = *p;
			switch (yych) {
				case '\'': goto yy91;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'G':
				case 'H':
				case 'I':
				case 'J':
				case 'K':
				case 'L':
				case 'M':
				case 'N':
				case 'O':
				case 'P':
				case 'Q':
				case 'R':
				case 'S':
				case 'T':
				case 'U':
				case 'V':
				case 'W':
				case 'X':
				case 'Y':
				case 'Z':
				case '_':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'g':
				case 'h':
				case 'i':
				case 'j':
				case 'k':
				case 'l':
				case 'm':
				case 'n':
				case 'o':
				case 'p':
				case 'q':
				case 'r':
				case 's':
				case 't':
				case 'u':
				case 'v':
				case 'w':
				case 'x':
				case 'y':
				case 'z': goto yy54;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel20;
					}
					goto yy55;
			}
yy55:
			{ str = string(p0, p);

                              // to upper
                              if (g_args.ucase()) str = str_toupper(str);

                              // handle af' et all
                              Keyword keyword = keyword_lookup(str);
                              if (str.back() == '\'' && keyword == Keyword::None) { // drop quote
                                str.pop_back();
                                p--;
                                keyword = keyword_lookup(str);
                              }

                              // check for -IXIY
                              if (g_args.swap_ixiy() != IXIY_NO_SWAP) {
                                switch (keyword) {
                                case Keyword::IX: case Keyword::IXH: case Keyword::IXL:
                                case Keyword::IY: case Keyword::IYH: case Keyword::IYL:
                                  str = str_swap_x_y(str);
                                  keyword = keyword_lookup(str);
                                  break;
                                default:;
                                }
                              }

                              // check for .ASSUME
                              if (keyword == Keyword::ASSUME && !line.tokens().empty() &&
                                  line.tokens().back().is(TType::Dot))
                                line.tokens().pop_back();       // remove '.'

                              // need raw strings after INCLUDE, BINARY, INCBIN, LINE, C_LINE
                              switch (keyword) {
                              case Keyword::INCLUDE: case Keyword::BINARY: case Keyword::INCBIN:
                              case Keyword::LINE:    case Keyword::C_LINE:
                                raw_strings = true;
                                break;
                              default:;
                              }

                              // check for ASMPC
                              if (keyword == Keyword::ASMPC)
                                PUSH_TOKEN1(TType::ASMPC);
                              else
                                PUSH_TOKEN2(TType::Ident, str);
                              continue;
                            }
yy56:
			++p;
			{ PUSH_TOKEN1(TType::LSquare); continue; }
yy57:
			++p;
yyFillLabel21:
			yych = *p;
			switch (yych) {
				case '\n': goto yy92;
				case '\r': goto yy94;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel21;
					}
					goto yy58;
			}
yy58:
			{ PUSH_TOKEN1(TType::Backslash); continue; }
yy59:
			++p;
			{ PUSH_TOKEN1(TType::RSquare); continue; }
yy60:
			++p;
yyFillLabel22:
			yych = *p;
			switch (yych) {
				case '^': goto yy95;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel22;
					}
					goto yy61;
			}
yy61:
			{ PUSH_TOKEN1(TType::BinXor); continue; }
yy62:
			++p;
			{ PUSH_TOKEN1(TType::LBrace); continue; }
yy63:
			++p;
yyFillLabel23:
			yych = *p;
			switch (yych) {
				case '|': goto yy96;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel23;
					}
					goto yy64;
			}
yy64:
			{ PUSH_TOKEN1(TType::BinOr); continue; }
yy65:
			++p;
			{ PUSH_TOKEN1(TType::RBrace); continue; }
yy66:
			++p;
			{ PUSH_TOKEN1(TType::BinNot); continue; }
yy67:
			++p;
			{ PUSH_TOKEN1(TType::Ne); continue; }
yy68:
			++p;
			{ PUSH_TOKEN1(TType::DblHash); continue; }
yy69:
			++p;
yyFillLabel24:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy69;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel24;
					}
					goto yy70;
			}
yy70:
			{ PUSH_TOKEN2(TType::Integer, a2i(p0+1, p, 16)); continue; }
yy71:
			++p;
yyFillLabel25:
			yych = *p;
			switch (yych) {
				case '"': goto yy97;
				case '#':
				case '-': goto yy71;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel25;
					}
					goto yy72;
			}
yy72:
			p = marker;
			switch (yyaccept) {
				case 0: goto yy24;
				case 1: goto yy39;
				case 2: goto yy10;
				case 3: goto yy78;
				case 4: goto yy81;
				default: goto yy101;
			}
yy73:
			++p;
yyFillLabel26:
			yych = *p;
			switch (yych) {
				case '0':
				case '1': goto yy73;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel26;
					}
					goto yy74;
			}
yy74:
			{ PUSH_TOKEN2(TType::Integer, a2i(p0+1, p, 2)); continue; }
yy75:
			++p;
			{ PUSH_TOKEN1(TType::LogAnd); continue; }
yy76:
			++p;
			{ PUSH_TOKEN1(TType::Power); continue; }
yy77:
			yyaccept = 3;
			marker = ++p;
yyFillLabel27:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy77;
				case 'E':
				case 'e': goto yy98;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel27;
					}
					goto yy78;
			}
yy78:
			{ PUSH_TOKEN2(TType::Floating, a2f(p0, p)); continue; }
yy79:
			++p;
yyFillLabel28:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy79;
				case 'H':
				case 'h': goto yy83;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel28;
					}
					goto yy72;
			}
yy80:
			yyaccept = 4;
			marker = ++p;
yyFillLabel29:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'H':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f':
				case 'h': goto yy100;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel29;
					}
					goto yy81;
			}
yy81:
			{ PUSH_TOKEN2(TType::Integer, a2i(p0, p, 2)); continue; }
yy82:
			yyaccept = 1;
			marker = ++p;
yyFillLabel30:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy79;
				case 'H':
				case 'h': goto yy83;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel30;
					}
					goto yy39;
			}
yy83:
			++p;
			{ PUSH_TOKEN2(TType::Integer, a2i(p0, p, 16)); continue; }
yy84:
			++p;
yyFillLabel31:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy102;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel31;
					}
					goto yy72;
			}
yy85:
			yyaccept = 4;
			marker = ++p;
yyFillLabel32:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy79;
				case 'H':
				case 'h': goto yy83;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel32;
					}
					goto yy81;
			}
yy86:
			++p;
			{ PUSH_TOKEN1(TType::LShift); continue; }
yy87:
			++p;
			{ PUSH_TOKEN1(TType::Le); continue; }
yy88:
			++p;
			goto yy49;
yy89:
			++p;
			{ PUSH_TOKEN1(TType::Ge); continue; }
yy90:
			++p;
			{ PUSH_TOKEN1(TType::RShift); continue; }
yy91:
			++p;
			goto yy55;
yy92:
			++p;
yy93:
			{ line_start = p; peek_text_line(line); continue; }
yy94:
			++p;
yyFillLabel33:
			yych = *p;
			switch (yych) {
				case '\n': goto yy92;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel33;
					}
					goto yy93;
			}
yy95:
			++p;
			{ PUSH_TOKEN1(TType::LogXor); continue; }
yy96:
			++p;
			{ PUSH_TOKEN1(TType::LogOr); continue; }
yy97:
			++p;
			{
                              int n = 0;
                              for (const char* i = p0+2; i < p-1; i++) {
                                  n *= 2;
                                  if (*i == '#') n++;
                              }
                              PUSH_TOKEN2(TType::Integer, n);
                              continue;
                            }
yy98:
			++p;
yyFillLabel34:
			yych = *p;
			switch (yych) {
				case '+':
				case '-': goto yy104;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy105;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel34;
					}
					goto yy72;
			}
yy99:
			yyaccept = 5;
			marker = ++p;
yyFillLabel35:
			yych = *p;
yy100:
			switch (yych) {
				case '0':
				case '1': goto yy99;
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy79;
				case 'H':
				case 'h': goto yy83;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel35;
					}
					goto yy101;
			}
yy101:
			{ PUSH_TOKEN2(TType::Integer, a2i(p0+2, p, 2)); continue; }
yy102:
			++p;
yyFillLabel36:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy102;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel36;
					}
					goto yy103;
			}
yy103:
			{ PUSH_TOKEN2(TType::Integer, a2i(p0+2, p, 16)); continue; }
yy104:
			++p;
yyFillLabel37:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy105;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel37;
					}
					goto yy72;
			}
yy105:
			++p;
yyFillLabel38:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9': goto yy105;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel38;
					}
					goto yy78;
			}
yy106:
			{ goto end; }
		}

    }

string_loop:
    str.clear();
    error.clear();
    while (true) {
        p0 = p;
        
		{
			char yych;
yyFillLabel39:
			yych = *p;
			switch (yych) {
				case 0x00:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel39;
						goto yy135;
					}
					goto yy108;
				case '\n': goto yy111;
				case '\r': goto yy113;
				case '"': goto yy114;
				case '\'': goto yy115;
				case '\\': goto yy116;
				default: goto yy109;
			}
yy108:
			++p;
			{ p--; scan_error(ErrCode::MissingQuote, error); goto end; }
yy109:
			++p;
yy110:
			{ str.push_back(*p0); continue; }
yy111:
			++p;
yy112:
			{ if (raw_strings) {
                                str.append(string(p0, p));
                                error = "started at " + m_location.filename() +
                                        ":" + std::to_string(m_location.line_num());
                                line_start = p; peek_text_line(line);
                                continue;
                              }
                              else {
                                scan_error(ErrCode::MissingQuote, error); goto end;
                              }
                            }
yy113:
			++p;
yyFillLabel40:
			yych = *p;
			switch (yych) {
				case '\n': goto yy111;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel40;
					}
					goto yy112;
			}
yy114:
			++p;
			{ if (quote == 2) {
                                PUSH_TOKEN2(TType::String, str);
                                goto main_loop;
                              }
                              else {
                                str.push_back(*p0);
                                continue;
                              }
                            }
yy115:
			++p;
			{ if (quote == 1) {
                                if (str.length() != 1) {
                                  scan_error(ErrCode::InvalidCharConst);
                                  goto main_loop;
                                }
                                else {
                                  PUSH_TOKEN2(TType::Integer, str[0]);
                                  goto main_loop;
                                }
                              }
                              else {
                                str.push_back(*p0);
                                continue;
                              }
                            }
yy116:
			++p;
yyFillLabel41:
			yych = *p;
			switch (yych) {
				case '\n': goto yy110;
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7': goto yy119;
				case 'a': goto yy121;
				case 'b': goto yy122;
				case 'e': goto yy123;
				case 'f': goto yy124;
				case 'n': goto yy125;
				case 'r': goto yy126;
				case 't': goto yy127;
				case 'v': goto yy128;
				case 'x': goto yy129;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel41;
						goto yy110;
					}
					goto yy117;
			}
yy117:
			++p;
yy118:
			{ if (raw_strings) {
                                str.push_back(*p0); p--;
                              }
                              else {
                                str.push_back(p0[1]);
                              }
                              continue;
                            }
yy119:
			++p;
yyFillLabel42:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7': goto yy130;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel42;
					}
					goto yy120;
			}
yy120:
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back(a2i(p0+1, p, 8)); } continue; }
yy121:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\a'); } continue; }
yy122:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\b'); } continue; }
yy123:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\x1b'); } continue; }
yy124:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\f'); } continue; }
yy125:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\n'); } continue; }
yy126:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\r'); } continue; }
yy127:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\t'); } continue; }
yy128:
			++p;
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back('\v'); } continue; }
yy129:
			++p;
yyFillLabel43:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy131;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel43;
					}
					goto yy118;
			}
yy130:
			++p;
yyFillLabel44:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7': goto yy133;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel44;
					}
					goto yy120;
			}
yy131:
			++p;
yyFillLabel45:
			yych = *p;
			switch (yych) {
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
				case 'A':
				case 'B':
				case 'C':
				case 'D':
				case 'E':
				case 'F':
				case 'a':
				case 'b':
				case 'c':
				case 'd':
				case 'e':
				case 'f': goto yy134;
				default:
					if (limit <= p) {
						if (yyfill() == 0) goto yyFillLabel45;
					}
					goto yy132;
			}
yy132:
			{ if (raw_strings) { str.push_back(*p0); p = p0+1; }
                              else { str.push_back(a2i(p0+2, p, 16)); } continue; }
yy133:
			++p;
			goto yy120;
yy134:
			++p;
			goto yy132;
yy135:
			{ scan_error(ErrCode::MissingQuote, error); goto end; }
		}

    }

end:
    if (p > line_start) {
        if (m_got_error)
            line.tokens().clear();

        PUSH_TOKEN1(TType::Newline);
        line_start = line_end = p0 = marker = p;
        return true;
    }
    else
        return false;
}

bool FileScanner::fill() {
    if (m_got_eof)
        return false;
    else {
        // save indexes
        unsigned line_start_index = line_start - m_buffer.c_str();
        unsigned line_end_index = line_end - m_buffer.c_str();
        unsigned p_index = p - m_buffer.c_str();
        unsigned p0_index = p0 - m_buffer.c_str();
        unsigned marker_index = marker - m_buffer.c_str();

        // remove all before line_start
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + line_start_index);

        // adjust indices
        line_end_index -= line_start_index;
        p_index -= line_start_index;
        p0_index -= line_start_index;
        line_start_index = 0;

        // read from file
        unsigned cur_size = m_buffer.size();
        m_buffer.resize(cur_size + FILL_SIZE + YYMAXFILL);      // reserve extra YYMAXFILL for re2c
        m_ifs.read(&m_buffer[cur_size], FILL_SIZE);
        m_buffer.resize(cur_size + m_ifs.gcount());

        // check for end of file
        if (m_ifs.gcount() < FILL_SIZE) {     // got to the end of file
            m_got_eof = true;
            if (!m_buffer.empty() && !is_eol(m_buffer.back()))
                m_buffer.push_back('\n');       // add missing newline
            m_buffer.push_back('\n');           // read a blank line at the end
        }

        // adjust pointers
        line_start = m_buffer.c_str() + line_start_index;
        line_end = m_buffer.c_str() + line_end_index;
        p = m_buffer.c_str() + p_index;
        p0 = m_buffer.c_str() + p0_index;
        marker = m_buffer.c_str() + marker_index;
        limit = m_buffer.c_str() + m_buffer.size();

        return true;
    }
}

void FileScanner::notify_new_line(const string& text_) {
    string text = str_chomp(text_) + "\n";
    m_location.set_source_line(text);
    g_errors.set_location(m_location);
    list_got_source_line(m_location.filename().c_str(), m_location.line_num(),
        m_location.source_line().c_str());
}

void FileScanner::scan_error(ErrCode code, const string& arg) {
    if (!m_got_error)
        g_errors.error(code, arg);
    m_got_error = true;
}
