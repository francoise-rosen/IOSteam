//
//  main.cpp
//  fibristerreAssignment_V2
//
//  Created by syfo_dias on 20/02/2021.
//  Copyright © 2021 syfo_dias. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "reading.h"

/** test parsing . */
void testParsing (const std::string& file)
{
    std::ifstream ifs {file};
    if (! ifs)
        throw std::runtime_error ("bad file");
    std::string word;
    std::getline (ifs, word);
    std::string line;
    Data data;
    Data emptyData;
    if (word == "valid")
    {
        Reading reading;
        while (std::getline (ifs, line))
        {
            std::stringstream ss {line};
            if (line == "invalid")
            {
                word = line;
                break;
            }
            if (ss >> reading)
                data.push (reading, true);
        }
    }
    if (word == "invalid")
    {
        Reading reading;
        while (std::getline (ifs, line))
        {
            std::stringstream ss {line};
            if (line == "valid")
            {
                word = line;
                break;
            }
            // must not read single time
            if (ss >> reading)
                data.push (reading);
        }
    }
    for (int i = 0; i < data.size(); ++i)
        std::cout << data.getUnchecked(i) << '\n';
}

/** test name validation. */
void testTrim (const std::string& s)
{
    std::ifstream ifs {s};
    if (! ifs)
    {
        throw std::runtime_error ("bad file");
    }
    std::string word;
    ifs >> word;
    if (word == "valid")
    {
        while (true)
        {
            //getline (ifs, word);
            ifs >> word;
            if (ifs.eof() || word == "invalid") break;
            auto pass = trimAndValidate (word);
            assert (pass == true);
            
        }
    }
    if (word == "invalid")
    {
        while (true)
        {
            ifs >> word;
            if (ifs.eof()) break;
            auto pass = trimAndValidate (word);
            assert (pass == false);
        }
    }
    std::cout << "name validation tests passed\n";    
}

// test file I/O
void exportToTxt (const std::string& fileName, const Data& data)
{
    std::ofstream ofs {fileName};
    if (! ofs)
        throw std::runtime_error ("Bad file");
    for (int i = 0; i < data.size(); ++i)
    {
        ofs << data.getUnchecked (i) << '\n';
    }
}

void importFromTxt (const std::string& fileName, Data& data)
{
    std::ifstream ifs {fileName};
    if (! ifs)
        throw std::runtime_error ("Bad file");
    
    if (ifs.is_open())
    {
        std::string line;
        while (std::getline (ifs, line))
        {
            Reading reading;
            std::stringstream ss {line};
            if (ss >> reading)
            {
                data.push (reading);
            }
        }
    }
    if (! ifs.eof())
        throw std::runtime_error ("bad reading from txt");
}

//==================================================================
// OPERATIONS

bool enterMeasurement (std::istream& is, Data& data)
{
    Reading reading;
    if (is >> reading)
    {
        data.push (reading);
    }
    else if (is.bad())
        throw std::runtime_error ("Error with reading data. Termitating program.\n");
    else
    {
        std::cout << "Failed to add this reading.\nPlease, try again.\n";
    }
    return true;
}

bool removeMeasurement (std::istream& is, Data& data)
{
    std::string temp;
    if (is >> temp)
    {
        int index;
        // check if remove by index or by name
        auto isIndex = isInt (temp, index);
        if (isIndex)
        {
            if (! data.remove (index))
                throw ReadingError("Bad index, try again");
            return true;
        }
        if (! data.remove (temp))
            throw ReadingError("Bad index, try again");
    }
    return true;
}

bool swapMeasurements (std::istream& is, Data& data)
{
    std::string a, b;
    if (! (is >> a >> b))
        throw ReadingError ("Please enter 2 values to swap.\n");
    int index1, index2;
    if (isInt(a, index1) && isInt(b, index2))
    {
        if (! data.swap (index1, index2))
            throw ReadingError("Bad index. Try again.\n");
        return true;
    }
    if (! data.swap (a, b))
        throw ReadingError("Bad index. Try again.\n");
    return true;
}

bool exportToTxt (std::istream& is, Data& data)
{
    std::string fileName;
    if (! (is >> fileName))
        throw ("File name is missing");
    std::ofstream ofs {fileName};
    if (! ofs)
        throw std::runtime_error ("Bad file");
    for (int i = 0; i < data.size(); ++i)
    {
        ofs << data.getUnchecked (i) << '\n';
    }
    return true;
}

bool importFromTxt (std::istream& is, Data& data)
{
    std::string fileName;
    if (! (is >> fileName))
        throw ("File name is missing");
    std::ifstream ifs {fileName};
    if (! ifs)
        throw std::runtime_error ("Bad file");
    
    // check if we should override duplicates.
    bool replaceDuplicate = false;
    if (! is.eof())
    {
        std::string temp;
        is >> temp;
        if (temp == "override")
            replaceDuplicate = true;
    }
    
    // read and place data into data container.
    if (ifs.is_open())
    {
        std::string line;
        while (std::getline (ifs, line))
        {
            Reading reading;
            std::stringstream reading_stream {line};
            if (reading_stream >> reading)
            {
                data.push (reading, replaceDuplicate);
            }
        }
    }
    
    // failed to read till the end of the file
    if (! ifs.eof())
        throw std::runtime_error ("bad reading from txt");
    return true;
}

//==========================================================================
// INPUT

bool process (Data& readingsData)
{
    std::string line;
    getline(std::cin, line);
    std::stringstream ss {line};
    std::string key;
    
    try
    {
        ss >> key;
        if (key == "display")
        {
            for (auto p : Input::commands)
                std::cout << p.first << p.second << '\n';
            return true;
        }
        if (key == "enter")
        {
            return enterMeasurement (ss, readingsData);
        }
        if (key == "remove")
        {
            return removeMeasurement (ss, readingsData);
        }
        if (key == "quit")
        {
            std::cout << "Closing programm...\n";
            return false;
        }
        if (key == "output")
        {
            readingsData.output();
            return true;
        }
        if (key == "switch")
        {
            return swapMeasurements (ss, readingsData);
        }
        if (key == "sort")
        {
            readingsData.sort();
            return true;
        }
        if (key == "to_txt")
        {
            return exportToTxt (ss, readingsData);
        }
        if (key == "from_txt")
        {
            return importFromTxt (ss, readingsData);
        }
        
        throw ReadingError ("Unknown keyword, please try again.\n");
    }
    catch (ReadingError& e)
    {
        std::cerr << e.what() << '\n';
    }
    return true;
}


int main(int argc, const char * argv[]) {
    try {
#ifdef DEBUG
        testParsing ("test_read_data.txt");
        testTrim ("test_names.txt");
        Data testData;
        importFromTxt ("read_data.txt", testData);
        Data testData2;
        testData2.push({"BOC", 43, {5.3, 23.3}});
        testData2.push({"BOC", 1, {0.3, 256.0, -999.9}});
        testData2.push({"AFX", 6, {5.01, 2.3}});
        exportToTxt ("write_data.txt", testData2);
#else
        std::cout << "Enter display to see available commands.\n";
        Data data;
        while (true)
        {
            if (! process (data))
                break;
        }
#endif
    } catch (std::exception& e) {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}
