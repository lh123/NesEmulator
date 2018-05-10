#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <list>

struct Section {
    std::string name;
    std::map<std::string, std::string> keys;
};

class Config {
public:
    static constexpr int MAX_LINE_LENGHT = 1024;

    Config();
    ~Config();

    bool loadFromDisk(std::string path);

    std::string readString(std::string section, std::string key, bool *success);
    int readInt(std::string section, std::string key, bool *success);
    float readFloat(std::string section, std::string key, bool *success);

    bool write(std::string section, std::string key, std::string keyValue);
    bool write(std::string section, std::string key, int keyValue);
    bool write(std::string section, std::string key, float keyValue);

    bool deleteSection(std::string section, bool force = false);
    bool deleteKey(std::string section, std::string key);

    bool refresh();
    bool save();

private:
    bool parser(const char *iniBuffer, int size);
    bool parserLine(const char *lineStr, int size);
    Section *parserSection(const char *sectionStr, int size);
    bool parserKeys(Section *section, const char *keyStr, int size);

    Section *findSectionByName(std::string name);

private:
    std::string mPath;
    Section *mCurrentSection;
    std::list<Section *> mSections;
};

#endif