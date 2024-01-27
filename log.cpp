#include "log.h"
#include "common.h"

#include <iostream>
#include <string>
#include <QDebug>
#include <map>


using std::cout, std::endl, std::string, std::map;

Log::Log()
{
    m_filePath = m_defaultLogFile;
    m_debugMode = DEBUG_MODE;
    initLevelMap();
}

Log::Log(string file)
{
    m_filePath = file;
    m_debugMode = DEBUG_MODE;
    initLevelMap();
}

Log::~Log()
{
    delete m_pMap;
    m_pMap = nullptr;
}

Log *Log::setDebugMode(bool debug)
{
    m_debugMode = debug;
    return this;
}

Log *Log::setPlaceHolder(string str)
{
    m_placeHolder = str;
    return this;
}

void Log::debug(string s, initializer_list<const string> args)
{
    replace(m_placeHolder, args, s);
    log(s, LEVEL_DEBUG);
}

void Log::info(string s,  initializer_list<const string> args)
{
    replace(m_placeHolder, args, s);
    log(s, LEVEL_INFO);
}

void Log::warning(string s, initializer_list<const string> args)
{
    replace(m_placeHolder, args, s);
    log(s, LEVEL_WARNING);
}

void Log::error(string s, initializer_list<const string> args)
{
    replace(m_placeHolder, args, s);
    log(s, LEVEL_ERROR);
}

int Log::replace(string search, initializer_list<const string> replaces, string& subject)
{
    int i = 0;
    for (auto &elem : replaces) {
        auto pos = subject.find(search, i);
        if (pos != string::npos) {
            subject.replace(pos, search.length(), elem);
            i++;
        }
    }
    return i;
}

void Log::log(string str, Level level)
{
    // 如果当前错误比错误级别低，不记录相关日志.
    if (level < m_logLevel) {
        return;
    }
    str.insert(0, getValueFromLevelMap(level));
    if (m_debugMode) {
        qDebug() << str;
    } else {
        // TODO: 写入日志文件
    }
}
typedef pair<char, string> LevelPair;
void Log::initLevelMap()
{
    m_pMap = new LevelMap();
    m_pMap->insert(LevelMap::value_type(Level::LEVEL_INFO, "[INFO]"));
    m_pMap->insert(LevelPair(LEVEL_WARNING, "[WARNING]"));
    m_pMap->insert(LevelMap::value_type(LEVEL_ERROR, "[ERROR]"));
    m_pMap->insert(LevelMap::value_type(LEVEL_DEBUG, "[DEBUG]"));
}

string Log::getValueFromLevelMap(Level level)
{
    if (m_pMap == nullptr) {
        return "[unknown]";
    }
    return m_pMap->at(level);
}
