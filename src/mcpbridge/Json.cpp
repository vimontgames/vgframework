#include "mcpbridge/Precomp.h"
#include "mcpbridge/Json.h"

#include <cstdio>
#include <cmath>
#include <cstring>

namespace vg::mcpbridge
{
    //--------------------------------------------------------------------------------------
    const Json & Json::operator[](const std::string & _key) const
    {
        static const Json s_null;
        if (m_type != Type::Object)
            return s_null;
        auto it = m_object.find(_key);
        return it == m_object.end() ? s_null : it->second;
    }

    //--------------------------------------------------------------------------------------
    // Serialization
    //--------------------------------------------------------------------------------------
    static void appendEscaped(std::string & _out, const std::string & _s)
    {
        _out += '"';
        for (char c : _s)
        {
            switch (c)
            {
                case '"':  _out += "\\\""; break;
                case '\\': _out += "\\\\"; break;
                case '\b': _out += "\\b";  break;
                case '\f': _out += "\\f";  break;
                case '\n': _out += "\\n";  break;
                case '\r': _out += "\\r";  break;
                case '\t': _out += "\\t";  break;
                default:
                    if ((unsigned char)c < 0x20)
                    {
                        char buf[8];
                        snprintf(buf, sizeof(buf), "\\u%04x", (unsigned char)c);
                        _out += buf;
                    }
                    else
                    {
                        _out += c;
                    }
                    break;
            }
        }
        _out += '"';
    }

    //--------------------------------------------------------------------------------------
    static void appendNumber(std::string & _out, double _n)
    {
        if (std::isnan(_n) || std::isinf(_n))
        {
            _out += "null";
            return;
        }

        // Integer-valued => no decimal point; otherwise up to 9 significant decimals.
        if (_n == std::floor(_n) && std::fabs(_n) < 1e15)
        {
            char buf[32];
            snprintf(buf, sizeof(buf), "%lld", (long long)_n);
            _out += buf;
        }
        else
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "%.9g", _n);
            _out += buf;
        }
    }

    //--------------------------------------------------------------------------------------
    void Json::dumpTo(std::string & _out, int _indent, int _depth) const
    {
        const bool pretty = _indent > 0;
        const std::string nl = pretty ? "\n" : "";
        const std::string pad  = pretty ? std::string((size_t)_indent * (_depth + 1), ' ') : "";
        const std::string pad0 = pretty ? std::string((size_t)_indent * _depth, ' ') : "";

        switch (m_type)
        {
            case Type::Null:   _out += "null"; break;
            case Type::Bool:   _out += m_bool ? "true" : "false"; break;
            case Type::Number: appendNumber(_out, m_number); break;
            case Type::String: appendEscaped(_out, m_string); break;

            case Type::Array:
            {
                if (m_array.empty()) { _out += "[]"; break; }
                _out += "[" + nl;
                for (size_t i = 0; i < m_array.size(); ++i)
                {
                    _out += pad;
                    m_array[i].dumpTo(_out, _indent, _depth + 1);
                    if (i + 1 < m_array.size()) _out += ",";
                    _out += nl;
                }
                _out += pad0 + "]";
                break;
            }

            case Type::Object:
            {
                if (m_object.empty()) { _out += "{}"; break; }
                _out += "{" + nl;
                size_t i = 0;
                for (const auto & kv : m_object)
                {
                    _out += pad;
                    appendEscaped(_out, kv.first);
                    _out += pretty ? ": " : ":";
                    kv.second.dumpTo(_out, _indent, _depth + 1);
                    if (++i < m_object.size()) _out += ",";
                    _out += nl;
                }
                _out += pad0 + "}";
                break;
            }
        }
    }

    //--------------------------------------------------------------------------------------
    std::string Json::dump(int _indent) const
    {
        std::string out;
        dumpTo(out, _indent, 0);
        return out;
    }

    //--------------------------------------------------------------------------------------
    // Parsing (recursive descent)
    //--------------------------------------------------------------------------------------
    namespace
    {
        struct Parser
        {
            const char * p;
            const char * end;
            std::string  error;

            void skipWs()
            {
                while (p < end && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r'))
                    ++p;
            }

            bool fail(const char * _msg) { if (error.empty()) error = _msg; return false; }

            bool parseValue(Json & _out)
            {
                skipWs();
                if (p >= end) return fail("unexpected end of input");

                switch (*p)
                {
                    case '{': return parseObject(_out);
                    case '[': return parseArray(_out);
                    case '"':
                    {
                        std::string s;
                        if (!parseString(s)) return false;
                        _out = Json(s);
                        return true;
                    }
                    case 't':
                        if (end - p >= 4 && strncmp(p, "true", 4) == 0)  { p += 4; _out = Json(true);  return true; }
                        return fail("invalid literal");
                    case 'f':
                        if (end - p >= 5 && strncmp(p, "false", 5) == 0) { p += 5; _out = Json(false); return true; }
                        return fail("invalid literal");
                    case 'n':
                        if (end - p >= 4 && strncmp(p, "null", 4) == 0)  { p += 4; _out = Json();       return true; }
                        return fail("invalid literal");
                    default:
                        return parseNumber(_out);
                }
            }

            bool parseString(std::string & _out)
            {
                if (*p != '"') return fail("expected string");
                ++p;
                _out.clear();
                while (p < end)
                {
                    char c = *p++;
                    if (c == '"')
                        return true;
                    if (c == '\\')
                    {
                        if (p >= end) return fail("bad escape");
                        char e = *p++;
                        switch (e)
                        {
                            case '"':  _out += '"';  break;
                            case '\\': _out += '\\'; break;
                            case '/':  _out += '/';  break;
                            case 'b':  _out += '\b'; break;
                            case 'f':  _out += '\f'; break;
                            case 'n':  _out += '\n'; break;
                            case 'r':  _out += '\r'; break;
                            case 't':  _out += '\t'; break;
                            case 'u':
                            {
                                if (end - p < 4) return fail("bad \\u escape");
                                unsigned code = 0;
                                for (int i = 0; i < 4; ++i)
                                {
                                    char h = *p++;
                                    code <<= 4;
                                    if      (h >= '0' && h <= '9') code |= (unsigned)(h - '0');
                                    else if (h >= 'a' && h <= 'f') code |= (unsigned)(h - 'a' + 10);
                                    else if (h >= 'A' && h <= 'F') code |= (unsigned)(h - 'A' + 10);
                                    else return fail("bad hex digit");
                                }
                                // Encode as UTF-8 (BMP only; surrogate pairs passed through as-is).
                                if (code < 0x80)
                                {
                                    _out += (char)code;
                                }
                                else if (code < 0x800)
                                {
                                    _out += (char)(0xC0 | (code >> 6));
                                    _out += (char)(0x80 | (code & 0x3F));
                                }
                                else
                                {
                                    _out += (char)(0xE0 | (code >> 12));
                                    _out += (char)(0x80 | ((code >> 6) & 0x3F));
                                    _out += (char)(0x80 | (code & 0x3F));
                                }
                                break;
                            }
                            default: return fail("unknown escape");
                        }
                    }
                    else
                    {
                        _out += c;
                    }
                }
                return fail("unterminated string");
            }

            bool parseNumber(Json & _out)
            {
                const char * start = p;
                if (p < end && (*p == '-' || *p == '+')) ++p;
                bool any = false;
                while (p < end && ((*p >= '0' && *p <= '9') || *p == '.' || *p == 'e' || *p == 'E' || *p == '-' || *p == '+'))
                {
                    any = true;
                    ++p;
                }
                if (!any) return fail("invalid number");
                std::string tok(start, p);
                _out = Json(atof(tok.c_str()));
                return true;
            }

            bool parseArray(Json & _out)
            {
                _out = Json::makeArray();
                ++p; // '['
                skipWs();
                if (p < end && *p == ']') { ++p; return true; }
                while (true)
                {
                    Json v;
                    if (!parseValue(v)) return false;
                    _out.push(v);
                    skipWs();
                    if (p >= end) return fail("unterminated array");
                    if (*p == ',') { ++p; continue; }
                    if (*p == ']') { ++p; return true; }
                    return fail("expected ',' or ']'");
                }
            }

            bool parseObject(Json & _out)
            {
                _out = Json::makeObject();
                ++p; // '{'
                skipWs();
                if (p < end && *p == '}') { ++p; return true; }
                while (true)
                {
                    skipWs();
                    if (p >= end || *p != '"') return fail("expected object key");
                    std::string key;
                    if (!parseString(key)) return false;
                    skipWs();
                    if (p >= end || *p != ':') return fail("expected ':'");
                    ++p;
                    Json v;
                    if (!parseValue(v)) return false;
                    _out.set(key, v);
                    skipWs();
                    if (p >= end) return fail("unterminated object");
                    if (*p == ',') { ++p; continue; }
                    if (*p == '}') { ++p; return true; }
                    return fail("expected ',' or '}'");
                }
            }
        };
    }

    //--------------------------------------------------------------------------------------
    bool Json::parse(const std::string & _text, Json & _out, std::string & _error)
    {
        Parser parser;
        parser.p = _text.c_str();
        parser.end = parser.p + _text.size();

        _out = Json();
        if (!parser.parseValue(_out))
        {
            _error = parser.error.empty() ? "parse error" : parser.error;
            _out = Json();
            return false;
        }

        parser.skipWs();
        if (parser.p != parser.end)
        {
            _error = "trailing characters after JSON value";
            _out = Json();
            return false;
        }
        return true;
    }
}
