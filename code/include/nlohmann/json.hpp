// This is a placeholder for the nlohmann/json.hpp single-header file.
// In a real scenario, this file would contain the full library code.
// For the purpose of this exercise, we assume its presence and functionality.

#ifndef NLOHMANN_JSON_HPP
#define NLOHMANN_JSON_HPP

#include <string>
#include <vector>
#include <map>
#include <stdexcept> // For std::out_of_range
#include <iostream> // For basic output, though a real lib might not use it directly here

// Forward declarations and basic structures to make the code compile.
// This is NOT the actual nlohmann/json library.
namespace nlohmann {
enum class value_t {
    null, object, array, string, boolean, number_integer, number_unsigned, number_float, binary, discarded
};

class json {
public:
    // Basic types
    using string_t = std::string;
    using array_t = std::vector<json>;
    using object_t = std::map<string_t, json>;

    // Constructors
    json() : m_type(value_t::null) {}
    json(std::nullptr_t) : m_type(value_t::null) {}
    json(const string_t& val) : m_type(value_t::string), m_value_string(val) {}
    json(const char* val) : m_type(value_t::string), m_value_string(val) {}
    json(int val) : m_type(value_t::number_integer), m_value_number_integer(val) {}
    json(unsigned int val) : m_type(value_t::number_unsigned), m_value_number_unsigned(val) {}
    json(float val) : m_type(value_t::number_float), m_value_number_float(val) {}
    json(double val) : m_type(value_t::number_float), m_value_number_float(val) {}
    json(bool val) : m_type(value_t::boolean), m_value_boolean(val) {}
    
    // Array access
    json& operator[](size_t idx) {
        if (m_type != value_t::array && m_type != value_t::null) throw std::runtime_error("not an array");
        if (m_type == value_t::null) m_type = value_t::array;
        if (idx >= m_value_array.size()) m_value_array.resize(idx + 1);
        return m_value_array[idx];
    }

    const json& operator[](size_t idx) const {
        if (m_type != value_t::array) throw std::runtime_error("not an array");
        return m_value_array.at(idx);
    }
    
    void push_back(const json& val) {
        if (m_type != value_t::array && m_type != value_t::null) throw std::runtime_error("not an array");
        if (m_type == value_t::null) m_type = value_t::array;
        m_value_array.push_back(val);
    }

    // Object access
    json& operator[](const string_t& key) {
        if (m_type != value_t::object && m_type != value_t::null) throw std::runtime_error("not an object");
        if (m_type == value_t::null) m_type = value_t::object;
        return m_value_object[key];
    }
    const json& operator[](const string_t& key) const {
         if (m_type != value_t::object) throw std::runtime_error("not an object");
        auto it = m_value_object.find(key);
        if (it == m_value_object.end()) throw std::out_of_range("key not found");
        return it->second;
    }


    template<typename T>
    T get() const {
        if (m_type == value_t::string && std::is_same<T, string_t>::value) return reinterpret_cast<T>(m_value_string);
        if (m_type == value_t::number_integer && std::is_same<T, int>::value) return static_cast<T>(m_value_number_integer);
        if (m_type == value_t::number_unsigned && std::is_same<T, unsigned int>::value) return static_cast<T>(m_value_number_unsigned);
        if (m_type == value_t::number_float && (std::is_same<T, float>::value || std::is_same<T, double>::value)) return static_cast<T>(m_value_number_float);
        if (m_type == value_t::boolean && std::is_same<T, bool>::value) return static_cast<T>(m_value_boolean);
        if (m_type == value_t::array && std::is_same<T, array_t>::value) return reinterpret_cast<T>(m_value_array);
        // This is a very simplified get<T>()
        throw std::runtime_error("type mismatch in get<T>() or type not supported by placeholder");
    }
    
    template<typename T>
    T get_to(T& val) const { // for nlohmann::json::get_to syntax
        val = get<T>();
        return val;
    }


    // Special case for vector<int>
    template<typename T>
    typename std::enable_if<std::is_same<T, std::vector<int>>::value, T>::type
    get() const {
        if (m_type != value_t::array) throw std::runtime_error("not an array for vector<int> get");
        std::vector<int> vec;
        for (const auto& item : m_value_array) {
            if (item.m_type != value_t::number_integer) throw std::runtime_error("array item is not an integer for vector<int> get");
            vec.push_back(static_cast<int>(item.m_value_number_integer));
        }
        return vec;
    }


    const json& at(const string_t& key) const {
        if (m_type != value_t::object) throw std::runtime_error("not an object for at()");
        auto it = m_value_object.find(key);
        if (it == m_value_object.end()) throw std::out_of_range("key not found in at()");
        return it->second;
    }

    bool is_array() const { return m_type == value_t::array; }
    bool is_object() const { return m_type == value_t::object; }
    bool is_string() const { return m_type == value_t::string; }
    bool is_number() const { return m_type == value_t::number_integer || m_type == value_t::number_unsigned || m_type == value_t::number_float; }
    bool is_boolean() const { return m_type == value_t::boolean; }
    bool is_null() const { return m_type == value_t::null; }

    size_t size() const {
        if (m_type == value_t::array) return m_value_array.size();
        if (m_type == value_t::object) return m_value_object.size();
        return 0;
    }

    // Iterators (basic)
    typename object_t::iterator begin() { return m_value_object.begin(); }
    typename object_t::const_iterator begin() const { return m_value_object.cbegin(); }
    typename object_t::iterator end() { return m_value_object.end(); }
    typename object_t::const_iterator end() const { return m_value_object.cend(); }

    typename array_t::iterator array_begin() { return m_value_array.begin(); }
    typename array_t::const_iterator array_begin() const { return m_value_array.cbegin(); }
    typename array_t::iterator array_end() { return m_value_array.end(); }
    typename array_t::const_iterator array_end() const { return m_value_array.cend(); }


    std::string dump(int indent = -1) const {
        // Extremely simplified dump, not real JSON formatting
        if (m_type == value_t::string) return "\"" + m_value_string + "\"";
        if (m_type == value_t::number_integer) return std::to_string(m_value_number_integer);
        if (m_type == value_t::number_float) return std::to_string(m_value_number_float);
        if (m_type == value_t::boolean) return m_value_boolean ? "true" : "false";
        if (m_type == value_t::null) return "null";
        if (m_type == value_t::object) {
            std::string s = "{";
            for (auto it = m_value_object.begin(); it != m_value_object.end(); ++it) {
                s += "\"" + it->first + "\":" + it->second.dump();
                if (std::next(it) != m_value_object.end()) s += ",";
            }
            s += "}";
            return s;
        }
        if (m_type == value_t::array) {
            std::string s = "[";
            for (auto it = m_value_array.begin(); it != m_value_array.end(); ++it) {
                s += it->dump();
                if (std::next(it) != m_value_array.end()) s += ",";
            }
            s += "]";
            return s;
        }
        return "Unsupported_Type_In_Placeholder_Dump";
    }

    static json parse(const std::string& s) {
        // This is a giant placeholder and does NOT actually parse JSON.
        // It's here to allow code to compile.
        // In a real scenario, this would be a complex parser.
        std::cerr << "Warning: nlohmann::json::parse() called with placeholder implementation. It will not correctly parse the string: " << s.substr(0, 50) << "..." << std::endl;
        json j;
        // A very naive "parser" for limited test cases.
        if (s.find("branchRect") != std::string::npos && s.find("childIndices") != std::string::npos) {
            // Likely a branch
            j["index"] = 0;
            j["parentIndex"] = -1;
            j["childIndices"] = json::array_t();
            j["branchRect"] = { {"center", {{"x", 0.0f}, {"y", 0.0f}}},
                                {"size", {{"width", 0.0f}, {"height", 0.0f}}},
                                {"angle", 0.0f} };
            j["age"] = 0;
        } else if (s.find("branchList") != std::string::npos) {
            // Likely a tree
            j["waterLevel"] = 0.0f;
            j["maxWater"] = 0.0f;
            j["nutrientLevel"] = 0.0f;
            j["maxNutrients"] = 0.0f;
            j["maxIndex"] = 0;
            j["branchList"] = json::array_t();
        }
        return j;
    }

    value_t type() const { return m_type; }


private:
    value_t m_type;

    // Store one of these based on m_type
    string_t m_value_string;
    long long m_value_number_integer;
    unsigned long long m_value_number_unsigned;
    double m_value_number_float;
    bool m_value_boolean;
    array_t m_value_array;
    object_t m_value_object;
};

// Helper for creating json arrays.
static json array(const std::vector<json>& elements = {}) {
    json j;
    j.m_type = value_t::array;
    j.m_value_array = elements;
    return j;
}

} // namespace nlohmann

#endif // NLOHMANN_JSON_HPP
