#include "http_parser.h"

namespace lite {

HttpParser::HttpParser()
    : state_(method_start)
{
}

void HttpParser::Reset()
{
    state_ = method_start;
}

HttpParser::result_type HttpParser::Consume(Request &req, char input)
{
    switch (state_)
    {
    case method_start:
        if (!is_CHAR(input) || is_CTL(input) || is_separators(input))
        {
            return bad;
        }
        else
        {
            state_ = method;
            req.method.push_back(input);
            return indeterminate;
        }
    case method:
        if (input == ' ')
        {
            state_ = uri;
            return indeterminate;
        }
        else if (!is_CHAR(input) || is_CTL(input) || is_separators(input))
        {
            return bad;
        }
        else
        {
            req.method.push_back(input);
            return indeterminate;
        }
    case uri:
        if (input == ' ')
        {
            state_ = http_version_h;
            return indeterminate;
        }
        else if (is_CTL(input))
        {
            return bad;
        }
        else
        {
            req.uri.push_back(input);
            return indeterminate;
        }
    case http_version_h:
        if (input == 'H')
        {
            state_ = http_version_t_1;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_t_1:
        if (input == 'T')
        {
            state_ = http_version_t_2;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_t_2:
        if (input == 'T')
        {
            state_ = http_version_p;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_p:
        if (input == 'P')
        {
            state_ = http_version_slash;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_slash:
        if (input == '/')
        {
            req.http_version_major = 0;
            req.http_version_minor = 0;
            state_ = http_version_major_start;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_major_start:
        if (is_DIGIT(input))
        {
            req.http_version_major = req.http_version_major * 10 + input - '0';
            state_ = http_version_major;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_major:
        if (input == '.')
        {
            state_ = http_version_minor_start;
            return indeterminate;
        }
        else if (is_DIGIT(input))
        {
            req.http_version_major = req.http_version_major * 10 + input - '0';
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_minor_start:
        if (is_DIGIT(input))
        {
            req.http_version_minor = req.http_version_minor * 10 + input - '0';
            state_ = http_version_minor;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case http_version_minor:
        if (input == '\r')
        {
            state_ = expecting_newline_1;
            return indeterminate;
        }
        else if (is_DIGIT(input))
        {
            req.http_version_minor = req.http_version_minor * 10 + input - '0';
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case expecting_newline_1:
        if (input == '\n')
        {
            state_ = header_line_start;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case header_line_start:
        if (input == '\r')
        {
            state_ = expecting_newline_3;
            return indeterminate;
        }
        else if (!req.headers.empty() && (input == ' ' || input == '\t'))
        {
            state_ = header_lws;
            return indeterminate;
        }
        else if (!is_CHAR(input) || is_CTL(input) || is_separators(input))
        {
            return bad;
        }
        else
        {
            req.headers.push_back(Header());
            req.headers.back().name.push_back(input);
            state_ = header_name;
            return indeterminate;
        }
    case header_lws:
        if (input == '\r')
        {
            state_ = expecting_newline_2;
            return indeterminate;
        }
        else if (input == ' ' || input == '\t')
        {
            return indeterminate;
        }
        else if (is_CTL(input))
        {
            return bad;
        }
        else
        {
            state_ = header_value;
            req.headers.back().value.push_back(input);
            return indeterminate;
        }
    case header_name:
        if (input == ':')
        {
            state_ = space_before_header_value;
            return indeterminate;
        }
        else if (!is_CHAR(input) || is_CTL(input) || is_separators(input))
        {
            return bad;
        }
        else
        {
            req.headers.back().name.push_back(input);
            return indeterminate;
        }
    case space_before_header_value:
        if (input == ' ')
        {
            state_ = header_value;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case header_value:
        if (input == '\r')
        {
            state_ = expecting_newline_2;
            return indeterminate;
        }
        else if (is_CTL(input))
        {
            return bad;
        }
        else
        {
            req.headers.back().value.push_back(input);
            return indeterminate;
        }
    case expecting_newline_2:
        if (input == '\n')
        {
            state_ = header_line_start;
            return indeterminate;
        }
        else
        {
            return bad;
        }
    case expecting_newline_3:
        return (input == '\n') ? good : bad;
    default:
        return bad;
    }
}

bool HttpParser::is_CHAR(int c)
{
    return c >= 0 && c <= 127;
}
bool HttpParser::is_CTL(int c)
{
    return (c >= 0 && c <= 31) || (c == 127);
}

bool HttpParser::is_separators(int c)
{
    switch (c)
    {
    case '(':
    case ')':
    case '<':
    case '>':
    case '@':
    case ',':
    case ';':
    case ':':
    case '\\':
    case '"':
    case '/':
    case '[':
    case ']':
    case '?':
    case '=':
    case '{':
    case '}':
    case ' ':
    case '\t':
        return true;
    default:
        return false;
    }
}

bool HttpParser::is_DIGIT(int c)
{
    return c >= '0' && c <= '9';
}

}