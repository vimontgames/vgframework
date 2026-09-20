#pragma once

// Minimal self-contained JSON (parse + serialize) for the MCP bridge transport.
// Not a general-purpose library: supports the subset used by docs/data-contract.md
// (objects, arrays, strings, doubles, bools, null). No unicode escapes beyond \uXXXX
// pass-through for BMP, no comments.

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace vg::mcpbridge
{
    class Json
    {
    public:
        enum class Type { Null, Bool, Number, String, Array, Object };

        Json() : m_type(Type::Null) {}
        Json(bool _b) : m_type(Type::Bool), m_bool(_b) {}
        Json(double _n) : m_type(Type::Number), m_number(_n) {}
        Json(int _n) : m_type(Type::Number), m_number((double)_n) {}
        Json(const char * _s) : m_type(Type::String), m_string(_s) {}
        Json(const std::string & _s) : m_type(Type::String), m_string(_s) {}

        static Json makeArray()  { Json j; j.m_type = Type::Array;  return j; }
        static Json makeObject() { Json j; j.m_type = Type::Object; return j; }

        Type type() const { return m_type; }
        bool isNull()   const { return m_type == Type::Null; }
        bool isObject() const { return m_type == Type::Object; }
        bool isArray()  const { return m_type == Type::Array; }
        bool isNumber() const { return m_type == Type::Number; }
        bool isString() const { return m_type == Type::String; }
        bool isBool()   const { return m_type == Type::Bool; }

        // Accessors with defaults (never throw).
        bool                asBool  (bool _def = false) const   { return m_type == Type::Bool   ? m_bool   : _def; }
        double              asNumber(double _def = 0.0) const   { return m_type == Type::Number ? m_number : _def; }
        const std::string & asString(const std::string & _def = std::string()) const { return m_type == Type::String ? m_string : _def; }

        bool                has(const std::string & _key) const { return m_type == Type::Object && m_object.find(_key) != m_object.end(); }
        const Json &        operator[](const std::string & _key) const;      // returns a static null Json if absent
        const std::vector<Json> & items() const { return m_array; }          // for arrays

        // Builders (only valid on the matching type).
        void set(const std::string & _key, const Json & _value) { m_object[_key] = _value; }
        void push(const Json & _value)                          { m_array.push_back(_value); }

        std::string dump(int _indent = 0) const;                             // _indent == 0 => compact

        // Returns false and leaves *this Null on parse error (sets _error).
        static bool parse(const std::string & _text, Json & _out, std::string & _error);

    private:
        void dumpTo(std::string & _out, int _indent, int _depth) const;

        Type                     m_type;
        bool                     m_bool = false;
        double                   m_number = 0.0;
        std::string              m_string;
        std::vector<Json>        m_array;
        std::map<std::string, Json> m_object;
    };
}
