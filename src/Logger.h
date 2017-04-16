#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <sstream>
#include <unordered_set>
#include <sys/time.h>

//          COLOR
#define RST  "\033[0m"
#define BLD  "\033[1m"
#define UDL  "\033[4m"
#define KRED  "\033[31m"
#define KGRN  "\033[32m"
#define KYEL  "\033[33m"
#define KBLU  "\033[34m"
#define KMAG  "\033[35m"
#define KCYN  "\033[36m"
#define KWHT  "\033[37m"

#define FRED(x) KRED << x << RST
#define FGRN(x) KGRN << x << RST
#define FYEL(x) KYEL << x << RST
#define FBLU(x) KBLU << x << RST
#define FMAG(x) KMAG << x << RST
#define FCYN(x) KCYN << x << RST
#define FWHT(x) KWHT << x << RST

#define BOLD(x) "\033[1m" << x << RST
#define UNDL(x) "\033[4m" << x << RST

class Transport {
public:
    Transport(std::string const& filePath);
    Transport(std::ostream& os);

    std::ostream& getStream();
    void setFormat(std::string const& format);
    void allowColor(bool c);

    void print(std::string const& msg, std::string const& mainColor, std::string const& level, bool endl = true);
    void append(std::string const& msg, bool endl = false);

    ~Transport();
private:
    std::ostream os;
    std::ofstream fos;

    std::string format = "{col}{bld}{lvl}{clr} {8}- {msg}"; // <color><bold>level</bold></color> - message            "-" is at the position 10
    bool allowCol = true;
};



class Logger {
public:
    static const unsigned int NOTHING = 0
                            , ERROR = 1
                            , WARNING = 2
                            , WARN = 2 // alias de Warning
                            , LOG = 4
                            , INFO = 8
                            , VERBOSE = 16
                            , VERB = 16 // alias de Verbose
                            , SPECIAL = 32
                            , SPE = 32 // alias de Special
                            , ALL = ERROR | WARN | LOG | INFO | VERB | SPE;

    static void error (std::string const& msg)      { Logger::get()._error(msg); }
    static void warning(std::string const& msg)     { Logger::get()._warning(msg); }
    static void warn(std::string const& msg)        { Logger::get()._warning(msg); }
    static void log(std::string const& msg)         { Logger::get()._log(msg); }
    static void info(std::string const& msg)        { Logger::get()._info(msg); }
    static void verbose(std::string const& msg)     { Logger::get()._verbose(msg); }
    static void verb(std::string const& msg)        { Logger::get()._verbose(msg); }
    static void special(std::string const& msg)     { Logger::get()._special(msg); }
    static void spe(std::string const& msg)         { Logger::get()._special(msg); }

    static void createTransport(Transport* t, std::string const& name);
    static void setTransportOf(unsigned int levels, std::string const& name);
    static void addTransportOf(unsigned int levels, std::string const& name);
    static void removeTransportOf(unsigned int levels, std::string const& name);
    static void cleanTransportOf(unsigned int levels);

    static void setFormatOf(std::string const& name, std::string const& format);
    static void allowColorOn(std::string const& name, bool allowC);

    static void showOnly(unsigned int levels = Logger::ALL);

    static void section(std::string name, unsigned int levels = Logger::ALL, unsigned int titleLevels = Logger::ALL);
    static void section_end(std::string name);
    static void showSection(std::string name);
    static void hideSection(std::string name);
    static void title(std::string name, unsigned int levels);

private:
    Logger();
    ~Logger();

// SINGLETON
    Logger(Logger const&) = delete;
    void operator=(Logger const&) = delete;
    static Logger& get() { static Logger l; return l; }

    void _error (std::string const& msg);
    void _warning(std::string const& msg);
    void _log(std::string const& msg);
    void _info(std::string const& msg);
    void _verbose(std::string const& msg);
    void _special(std::string const& msg);

    void _title(std::string name, unsigned int levels);
    unsigned int getLogsLevel();

    std::map<std::string, Transport*> transports;
    std::unordered_set<std::string> errTrans;
    std::unordered_set<std::string> warnTrans;
    std::unordered_set<std::string> logTrans;
    std::unordered_set<std::string> infoTrans;
    std::unordered_set<std::string> verbTrans;
    std::unordered_set<std::string> speTrans;

    std::unordered_set<std::string> curSections;
    std::unordered_set<std::string> sectionsHidden;
    std::map<std::string, unsigned int> sectionLogsLevel;
    unsigned int mainLogsLevel = Logger::ALL;

    bool logsLevelChanged = true;
    unsigned int curLogsLevel = Logger::ALL;
};

#endif