//
//  reading.h
//  fibristerreAssignment_V2
//
//  Created by syfo_dias on 20/02/2021.
//  Copyright © 2021 syfo_dias. All rights reserved.
//

#ifndef reading_h
#define reading_h
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <unordered_map>
#include <sstream>

namespace Input
{
    static const std::vector<std::string> commands {"display", "enter", "output", "remove", "switch", "sort", "export_txt", "export_binary", "import_txt", "import_binary", "quit"};
    enum key {Display, Enter, Output, Remove, Switch, Sort, ExportTXT, ExportBinary, ImportTXT, ImportBinary, Quit, numKeys};
    static const std::vector<char> separators {';', ','};
    constexpr static std::int32_t max_name_len = 10;
    constexpr static std::size_t max_size = 20;
}

// name trim
// trim " " from beginning and the end
// check if there's at least one alpha char.

inline bool isQuoted (const std::string& s)
{
    return (*s.begin() != '\"' || s.back() != '\"' || (s.size() < 3)) ? false : true;
}

struct NotValid
{
    bool operator() (char ch)
    {
        return (! isalpha(ch)) && (!isdigit(ch)) && (ch != '_') && (ch != '-');
    }
};

bool trimAndValidate (std::string& s)
{
    if (! isQuoted (s))
        return false;
    s.erase (s.begin()); // trim left
    s.pop_back(); // trim right
    // check if s.size(), resize it to 10
    if (s.size() > Input::max_name_len) s.resize(Input::max_name_len);
    // check if there's at least one alpha char
    auto al = std::find_if (s.begin(), s.end(), [] (const char& ch) {return std::isalpha(ch);});
    // make only alpha, digit and _ legit.
    if (al == s.end())
        return false;
    auto p = std::find_if (s.begin(), s.end(), NotValid());
    return (p != s.end()) ? false : true;
}

bool isDataValid (std::string& s, std::list<double>& data)
{
    for (int i = 0; i < s.length(); ++i)
    {
        // replace separators with whitespace
        if ( ! isdigit(s[i]) && std::find(Input::separators.begin(), Input::separators.end(), s[i]) != Input::separators.end())
            s[i] = ' ';
    }
    std::stringstream ss {s};
    if (! isdigit(s.front()) || ! isdigit(s.back()))
        return false;
    
    for (double d; ss >> d;)
        data.push_back (d);
    return ( ! ss.eof()) ? false : true;
}

bool isInt (const std::string& s, int& val)
{
    std::stringstream ss {s};
    auto notValid = find_if (s.begin(), s.end(), [] (char ch) {return ! isdigit(ch);});
    if (notValid != s.end())
        return false;
    return (ss >> val) ? true : false;
}

// STRUCT FOR PARSING A SINGULAR ENTRY
struct Reading
{
    std::string name;
    std::int32_t length {9};
    std::list<double> measurements;
    
    friend std::istream& operator>>(std::istream& is, Reading& r)
    {
        /** NAME. */
        std::string name;
        is >> name;
        auto trimmed = trimAndValidate (name);
        if (! trimmed || ! is)
        {
            is.clear (std::ios::failbit);
            return is;
        }
        r.name = name;
        
        /** LENGTH. */
        int length;
        std::string lenStr;
        is >> lenStr;
        auto validInt = isInt (lenStr, length);
        if (! validInt || ! is)
        {
            is.setstate (std::ios_base::failbit);
            return is;
        }
        r.length = length;
        
        /** MEASUREMENTS DATA */
        std::string dataStr;
        is >> dataStr;
        std::list<double> data;
        auto d = isDataValid (dataStr, data);
        if (! d || ! is.eof())
        {
            is.clear (std::ios_base::failbit);
            return is;
        }
        r.measurements = std::move (data);
        return is;
    }
    friend std::ostream& operator<<(std::ostream& os, Reading r)
    {
        os << "\"" << r.name << "\" " << r.length << " ";
        typename std::list<double>::iterator it, back;
        it = r.measurements.begin();
        back = r.measurements.end();
        
        if (std::distance (it, back) < 1)
            return os;
        --back;
        while (it != back)
        {
            os << *it << ';';
            ++it;
        }
        // print the last one without semicolon
        os << *it;
        return os;
    }
};

/** Data contains Readings. */
class Data
{
public:
    Data() {}
    bool push (const Reading& r, bool append = false)
    {
        if ((! append) && (names.count (r.name) > 0))
        {
            std::cout << "Name " << r.name << " already exists. Try another name.\n";
            return false;
        }
        if (r.measurements.size() > Input::max_size)
        {
            std::cout << "Max 20 entries for measurements allowed.\nYou have entered " << r.measurements.size() << '\n';
            return false;
        }
        names[r.name] = static_cast<int> (dataList.size()); // set a new index for this measurement
        dataList.push_back (r);
        return true;
    }
    
    bool remove (const std::string& name)
    {
        if (names.count (name) < 1)
            return false; // not found
        typename std::vector<Reading>::iterator it;
        dataList.erase (it + names[name]);
        names.erase (name);
        return true;
    }
    
    bool remove (const int& index)
    {
        if (index >= dataList.size())
            return false;
        return remove (dataList[index].name);
    }
    
    bool output() const
    {
        if (dataList.size() < 1)
            return false;
        for (auto p : dataList)
        {
            std::cout << p << '\n';
        }
        return true;
    }
    
    bool swap (const std::string a, const std::string b)
    {
        if ( (names.count(a) < 1) || (names.count(b) < 1) )
            return false;
        // locate the names in the array
        auto indexA = names[a];
        auto indexB = names[b];
        
        // swap in reading's array
        if (indexA >= dataList.size() || indexB >= dataList.size())
            throw std::runtime_error ("index is out of range");
        
        std::swap (dataList[indexA], dataList[indexB]);
        // update the names data
        names[b] = indexA;
        names[a] = indexB;
        return true;
    }
    
    bool swap (const int& a, const int& b)
    {
        if ( ((a < 0) || (a >= dataList.size()) || (b < a) || (b >= dataList.size())) )
            throw std::runtime_error ("index is out of range");
        
        // swap in reading's array
        auto name_a = dataList[a].name;
        auto name_b = dataList[b].name;
        
        std::swap (dataList[a], dataList[b]);
        // update the names data
        names[name_a] = b;
        names[name_b] = a;
        return true;
    }
    
    Reading getUnchecked (int index) const
    {
        return dataList[index];
    }
    
    std::size_t size() const
    {
        assert (dataList.size() == names.size());
        return dataList.size();
    }
    
private:
    // name - index
    std::unordered_map<std::string, int> names;
    std::vector<Reading> dataList;
};


class ReadingError
{
public:
    ReadingError (const std::string& message)
    :m{message} {}
    ReadingError (const std::string& message, const int& i)
    :m {message}, arg {i} {}
    std::string what()
    {
        return m + std::to_string(arg);
    }
    
private:
    std::string m;
    int arg;
    
};

#endif /* reading_h */
