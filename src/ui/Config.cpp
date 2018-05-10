#include "ui/Config.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string.h>

Config::Config() {}

Config::~Config() {
    save();
    for (auto *secion : mSections) {
        delete secion;
    }
    mSections.clear();
}

bool Config::loadFromDisk(std::string path) {
    mPath = path;
    return refresh();
}

std::string Config::readString(std::string section, std::string key, bool *success) {
    *success = true;
    Section *sec = findSectionByName(section);
    if (sec == nullptr) {
        std::cout << "cannot found section: '" << section << "'" << std::endl;
        *success = false;
        return "";
    }
    auto findKey = sec->keys.find(key);
    if (findKey == sec->keys.end()) {
        std::cout << "cannot found key: '" << key << "' in section '" << section << "'" << std::endl;
        *success = false;
        return "";
    }
    return findKey->second;
}

int Config::readInt(std::string section, std::string key, bool *success) {
    *success = true;
    std::string valueStr = readString(section, key, success);
    std::stringstream conv;
    conv << valueStr;
    int value;
    conv >> value;
    if (conv.fail()) {
        *success = false;
    }
    return value;
}

float Config::readFloat(std::string section, std::string key, bool *success) {
    *success = true;
    std::string valueStr = readString(section, key, success);
    std::stringstream conv;
    conv << valueStr;
    float value;
    conv >> value;
    if (conv.fail() || conv.bad()) {
        *success = false;
    }
    return value;
}

bool Config::write(std::string section, std::string key, std::string value) {
    Section *findSection = findSectionByName(section);
    if (findSection == nullptr) {
        findSection = new Section;
        findSection->name = section;
        findSection->keys.insert(std::make_pair(key, value));
        mSections.push_back(findSection);
    } else {
        auto findKey = findSection->keys.find(key);
        if (findKey != findSection->keys.end()) {
            findKey->second = value;
        } else {
            findSection->keys.insert(std::make_pair(key, value));
        }
    }
    return true;
}

bool Config::write(std::string section, std::string key, int value) {
    std::stringstream conv;
    conv << value;
    return write(section, key, conv.str());
}

bool Config::write(std::string section, std::string key, float value) {
    std::stringstream conv;
    conv << value;
    return write(section, key, conv.str());
}

bool Config::deleteSection(std::string section, bool force) {
    Section *findSection = findSectionByName(section);
    if (findSection == nullptr) {
        std::cout << "cannot found section: '" << section << "'" << std::endl;
        return false;
    }
    if (findSection->keys.size() == 0) {
        mSections.remove(findSection);
        delete findSection;
        return true;
    } else {
        if (force) {
            mSections.remove(findSection);
            delete findSection;
            return true;
        } else {
            std::cout << "cannot delete no empty section: '" << section << "'" << std::endl;
            return false;
        }
    }
}

bool Config::deleteKey(std::string section, std::string key) {
    Section *findSection = findSectionByName(section);
    if (findSection == nullptr) {
        std::cout << "cannot found section: '" << section << "'" << std::endl;
        return false;
    }
    auto findKey = findSection->keys.find(key);
    if (findKey == findSection->keys.end()) {
        std::cout << "cannot found key: '" << key << "' in section '" << section << "'" << std::endl;
        return false;
    }
    findSection->keys.erase(findKey);
    return true;
}

bool Config::refresh() {
    std::ifstream iniFile(mPath, std::ios::in);
    if (!iniFile.is_open()) {
        std::cout << "File is not exits" << std::endl;
        return false;
    }
    std::stringstream inibuffer;
    inibuffer << iniFile.rdbuf();
    std::string iniStr = inibuffer.str();
    return parser(iniStr.c_str(), iniStr.size());
}

bool Config::save() {
    std::ofstream writeFile(mPath, std::ios::out);
    if (!writeFile.is_open()) {
        return false;
    }
    for (auto *section : mSections) {
        writeFile << '[' << section->name << "]\n";
        for (auto key : section->keys) {
            writeFile << key.first << "=" << key.second << '\n';
        }
        writeFile << '\n';
    }
    return true;
}

bool Config::parser(const char *iniBuffer, int size) {
    char *line = new char[1024]{0};
    int index = 0;
    for (int i = 0; i < size; i++) {
        char ch = iniBuffer[i];
        if (ch == '\r' || ch == '\n') {
            line[index] = '\0';
            if (!parserLine(line, strlen(line))) {
                std::cout << "parser line failed:\n" << line << std::endl;
                return false;
            }
            index = 0;
        } else {
            line[index] = ch;
            index++;
        }
    }
    return true;
}

bool Config::parserLine(const char *lineStr, int size) {
    if (size <= 0) {
        return true;
    }
    const char *ptr = lineStr;
    const char *end = lineStr + size;
    if (*ptr == '#' || *ptr == ';') {
        return true;
    } else if (*ptr == '[') {
        ptr++;
        Section *section = parserSection(ptr, end - ptr);
        if (section == nullptr) {
            return false;
        } else {
            mCurrentSection = section;
            if (findSectionByName(mCurrentSection->name) == nullptr) {
                mSections.push_back(mCurrentSection);
            }
            return true;
        }
    } else {
        return parserKeys(mCurrentSection, ptr, end - ptr);
    }
}

Section *Config::parserSection(const char *sectionStr, int size) {
    if (size <= 0) {
        return nullptr;
    }
    const char *ptr = sectionStr;
    const char *end = sectionStr + size;
    if (*(sectionStr + size - 1) != ']') {
        std::cout << "section doesn't end with ']'" << std::endl;
        return nullptr;
    }
    Section *section = new Section;
    while (ptr != end - 1) {
        section->name.push_back(*ptr);
        ptr++;
    }
    return section;
}

bool Config::parserKeys(Section *section, const char *keyStr, int size) {
    if (section == nullptr) {
        return false;
    }
    const char *ptr = keyStr;
    const char *end = keyStr + size;
    const char *equalPtr = nullptr;
    while (ptr != end) {
        if (*ptr == '=') {
            equalPtr = ptr;
            break;
        }
        ptr++;
    }
    if (equalPtr == nullptr) {
        std::cout << "key doesn't contains '='" << std::endl;
        return false;
    }
    if (equalPtr == end - 1) {
        std::cout << "'=' right should not empty" << std::endl;
        return false;
    }
    const char *tempPtr = keyStr;
    std::string keyName;
    while (tempPtr != equalPtr) {
        keyName.push_back(*tempPtr);
        tempPtr++;
    }
    tempPtr = equalPtr + 1;
    std::string keyValue;
    while (tempPtr != end) {
        keyValue.push_back(*tempPtr);
        tempPtr++;
    }

    auto findKey = section->keys.find(keyName);
    if (findKey != section->keys.end()) {
        findKey->second = keyValue;
    } else {
        section->keys.insert(std::make_pair(keyName, keyValue));
    }
    return true;
}

Section *Config::findSectionByName(std::string name) {
    Section *section = nullptr;
    for (Section *sec : mSections) {
        if (sec->name == name) {
            section = sec;
            break;
        }
    }
    return section;
}
