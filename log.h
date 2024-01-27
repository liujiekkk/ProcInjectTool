#ifndef LOG_H
#define LOG_H


#include <string>
#include <map>

using std::string, std::initializer_list, std::map, std::pair;

typedef map<char, string> LevelMap, *PLevelMap;
//typedef pair<Level, string> LevelPair;

class Log
{
public:

    enum Level : char
    {
        LEVEL_ALL = 0x0,
        LEVEL_DEBUG = 0x01,
        LEVEL_INFO = 0x02,
        LEVEL_WARNING = 0x04,
        LEVEL_ERROR = 0x08,
    };

    Log();

    Log(string file);

    ~Log();

    Log *setDebugMode(bool);

    Log *setPlaceHolder(string str);

    void debug(string s, initializer_list<const string> args);

    void info(string s, initializer_list<const string> args);

    void warning(string s, initializer_list<const string> args);

    void error(string s, initializer_list<const string> args);

protected:

    int replace(string, initializer_list<const string>, string&);

    void log(string, Level);

private:

    // 日志消息中字符串替换的占位符
    string m_placeHolder = "%";

    string m_defaultLogFile = "./log/main.log";

    string m_filePath;

    bool m_debugMode = false;

    Level m_logLevel = LEVEL_ALL;

    PLevelMap m_pMap;

    void initLevelMap();

    string getValueFromLevelMap(Level);
};

#endif // LOG_H
