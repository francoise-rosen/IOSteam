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
#include <sstream>

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
                data.push (reading);
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

//==================================================================
// FILE

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
        if (key == Input::commands[Input::Display])
        {
            for (int i = 1; i < Input::commands.size(); ++i)
            {
                std::cout << Input::commands[i] << '\n';
            }
            ss.ignore(1000);
            return true;
        }
        if (key == Input::commands[Input::Enter])
        {
            Reading reading;
            if (ss >> reading)
            {
                readingsData.push (reading);
            }
            else if (ss.bad())
                throw std::runtime_error ("Error with reading data. Termitating program.\n");
            else
            {
                std::cout << "Failed to add this reading.\nPlease, try again.\n";
            }
            return true;
        }
        if (key == Input::commands[Input::Remove])
        {
            std::string temp;
            if (ss >> temp)
            {
                int index;
                auto isIndex = isInt (temp, index);
                if (isIndex)
                {
                    if (! readingsData.remove (index))
                        throw ReadingError("Bad index, try again");
                    return true;
                }
                if (! readingsData.remove (temp))
                    throw ReadingError("Bad index, try again");
            }
        }
        if (key == Input::commands[Input::Quit])
        {
            std::cout << "Closing programm...\n";
            return false;
        }
        if (key == Input::commands[Input::Output])
        {
            readingsData.output();
            return true;
        }
        if (key == Input::commands[Input::Switch])
        {
            std::string a, b;
            if (! (ss >> a >> b))
                throw ReadingError ("Please enter 2 values to swap.\n");
            int index1, index2;
            if (isInt(a, index1) && isInt(b, index2))
            {
                if (! readingsData.swap (index1, index2))
                    throw ReadingError("Bad index. Try again.\n");
                return true;
            }
            if (! readingsData.swap (a, b))
                throw ReadingError("Bad index. Try again.\n");
            return true;
        }
        if (key == Input::commands[Input::ExportTXT])
        {
            std::string fileName;
            if (! (ss >> fileName))
                throw ("File name is missing");
            std::ofstream ofs {fileName};
            if (! ofs)
                throw std::runtime_error ("Bad file");
            for (int i = 0; i < readingsData.size(); ++i)
            {
                ofs << readingsData.getUnchecked (i) << '\n';
            }
            return true;
        }
        if (key == Input::commands[Input::ImportTXT])
        {
            std::string fileName;
            if (! (ss >> fileName))
                throw ("File name is missing");
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
                        readingsData.push (reading);
                    }
                }
            }
            if (! ifs.eof())
                throw std::runtime_error ("bad reading from txt");
            return true;
        }
        
        throw ReadingError ("Unknown keyword, please try again.\n");
    }
    catch (ReadingError& e)
    {
        std::cerr << e.what() << '\n';
        return true;
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
