#ifndef HTTP_PARSER_HPP
#define HTTP_PARSER_HPP

#include <string>
#include <vector>
#include <tuple>

#include "request.h"

namespace lite {

class HttpParser
{
public:
    /// Construct ready to parse the request method.
    HttpParser();

    /// Reset to initial parser state.
    void Reset();

    /// Result of parse.
    enum result_type
    {
        good,
        bad,
        indeterminate
    };

    /// Parse some data. The enum return value is good when a complete request has
    /// been parsed, bad if the data is invalid, indeterminate when more data is
    /// required. The InputIterator return value indicates how much of the input
    /// has been consumed.
    template <typename InputIterator>
    std::tuple<result_type, InputIterator> Parse(Request &req,
                                                 InputIterator begin, InputIterator end)
    {
        while (begin != end)
        {
            result_type result = Consume(req, *begin++);
            if (result == good || result == bad)
                return std::make_tuple(result, begin);
        }
        return std::make_tuple(indeterminate, begin);
    }

private:
    /// Handle the next character of input.
    result_type Consume(Request &req, char input);

    static bool is_OCTET(int c);
    static bool is_CHAR(int c);
    static bool is_UPALPHA(int c);
    static bool is_LOALPHA(int c);
    static bool is_ALPHA(int c);
    static bool is_DIGIT(int c);
    static bool is_CTL(int c);
    static bool is_CR(int c);
    static bool is_LF(int c);
    static bool is_SP(int c);
    static bool is_HT(int c);
    static bool is_LWS(int c);
    static bool is_TEXT(int c);
    static bool is_HEX(int c);
    static bool is_token(int c);
    static bool is_separators(int c);

    /// The current state of the parser.
    enum state
    {
        method_start,
        method,
        uri,
        http_version_h,
        http_version_t_1,
        http_version_t_2,
        http_version_p,
        http_version_slash,
        http_version_major_start,
        http_version_major,
        http_version_minor_start,
        http_version_minor,
        expecting_newline_1,
        header_line_start,
        header_lws,
        header_name,
        space_before_header_value,
        header_value,
        expecting_newline_2,
        expecting_newline_3
    } state_;
};

}
#endif