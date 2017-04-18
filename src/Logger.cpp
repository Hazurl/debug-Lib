#include "Logger.h"

using namespace haz;

// ===== LEVEL =====

std::string Level::to_string(unsigned int i) {
    if (i >= OFF)       return "off";
    if (i >= ERROR)     return "error";
    if (i >= WARNING)   return "warning";
    if (i >= CONFIG)    return "config";
    if (i >= TRACE)     return "trace";
    if (i >= DEBUG)     return "debug";
    if (i >= INFO)      return "info";
                        return "detail";
}


// ===== HANDLER =====

Handler::Handler () : _color(true), _indentation(2), _format("({file} : {line} -> {func}) {col}{bld}{lvl}{clr} : {msg}{endl}") {}
Handler::~Handler() {}

void Handler::write(Message const& msg) {
    bool escape = false;
    bool meta = false;
    bool num = false;

    std::string out = "";
    std::string var = "";

    for (char const& c : _format) {
        if (num) {
            if (c >= '0' && c <= '9') {
                var += c;
            } else if (c == '}') {
                for (unsigned int i = std::stoi(var); i > out.size(); out += ' ')
                num = false;
                var = "";
            } else {
                var = "";
                num = false;
                out += c;
            }

            escape = false;
        } else if (meta && c != '}') {
            if (c >= '0' && c <= '9' && var == "") {
                num = true;
                meta = false;
            }
            var += c;
            escape = false;
        } else if (meta) {
            meta = false;

            if (var == "lvl")
                out += Level::to_string(msg.level);

            if (var == "line")
                out += std::to_string(msg.line);

            if (var == "func")
                out += msg.func;

            if (var == "file")
                out += msg.file;

            if (var == "col" && _color)
                out += msg.color;

            if (var == "bld" && _color)
                out += BLD;

            if (var == "udl" && _color)
                out += UDL;

            time_t rawtime;
            time(&rawtime);
            tm* t = localtime(&rawtime);

            if (var == "hour") {
                if (t->tm_hour < 10)
                    out += '0';
                out += std::to_string(t->tm_hour);
            }

            if (var == "min") {
                if (t->tm_min < 10)
                    out += '0';
                out += std::to_string(t->tm_min);
            }

            if (var == "sec") {
                if (t->tm_sec < 10)
                    out += '0';
                out += std::to_string(t->tm_sec);
            }

            if (var == "mic") {
                struct timeval tv;
                gettimeofday (&tv, nullptr);
                out += std::to_string(tv.tv_usec % 1000);
            }

            if (var == "mil") {
                struct timeval tv;
                gettimeofday (&tv, nullptr);
                out += std::to_string(tv.tv_usec / 1000);
            }

            if (var == "year")
                out += std::to_string(1900 + t->tm_year);

            if (var == "mon") {
                if (t->tm_mon < 10)
                    out += '0';
                out += std::to_string(t->tm_mon);
            }

            if (var == "day") {
                if (t->tm_mday < 10)
                    out += '0';
                out += std::to_string(t->tm_mday);
            }

            if (var == "clr" && _color)
                out += RST;

            if (var == "msg")
                out += msg.msg;

            if (var == "endl")
                out += '\n';

            var = "";
            escape = false;
        } else {
            if (c == '{' && !escape) {
                meta = true;
            } else if (c == '\\' && !escape) {
                escape = true;
            } else {
                out += c;
                escape = false;
            }
        }
    }

    append(out);
}

ConsoleHandler::ConsoleHandler() {}
ConsoleHandler::~ConsoleHandler() {}

void ConsoleHandler::append(std::string const& msg) {
    std::cout << msg;
}

FileHandler::FileHandler(const char* filename, bool append) {
    if (append)
        os.open(filename, std::ios::out | std::ios::app);
    else 
        os.open(filename, std::ios::out);
}

FileHandler::~FileHandler() {
    if (os)
        os.close();
}

void FileHandler::append(std::string const& msg) {
    os << msg;
}

StreamHandler::StreamHandler(std::ostream const& os) : os(os.rdbuf())  {}
StreamHandler::~StreamHandler() {}

void StreamHandler::append(std::string const& msg) {
    os << msg;
}

// ===== LOGGER =====

std::map<const char*, Logger> Logger::loggers = {};

Logger& Logger::get(const char* name) {
    if (loggers.find(name) == loggers.end())
        loggers.insert( { name, Logger(name) } );
    return loggers[name];
}

Logger::Logger(const char* name) : name(name) {}
Logger::~Logger() { clearHandlers(); }

Logger& Logger::clearHandlers() {
    for (auto& hi : handlers)
        if (hi.destroy_by_logger)
            delete hi.h;
    return *this;
}

Logger& Logger::addHandler(Handler* h, bool destroy_by_logger) {
    handlers.push_back({h, destroy_by_logger});
    return *this;
}

Logger& Logger::setLevel(int l) {
    level = l;
    return *this;
}

Logger& Logger::setColorsLevel(std::vector< std::pair<const char*, unsigned int> > vcl) {
    colorsLevel = vcl;
    return *this;
}

bool Logger::isEnabled(unsigned int l) {
    return l >= level;
}

void Logger::entering(const char* file, std::string const& func, long line, std::vector<std::string> params) {
    stackTr.push( {file, func, line, params} );
    trace(file, func, line, "Entering " + func);
}

void Logger::exiting(const char* file, std::string const& func, long line, std::string const& obj) {
    stackTr.pop();
    trace(file, func, line, "Exiting " + func + " : " + obj);
}

void Logger::stackTrace(const char* file, std::string const& func, long line, int depth) {
    trace(file, func, line, "Stack trace (" + std::to_string(depth) + ") >>");
}

void Logger::log(const char* file, std::string const& func, long line, unsigned int level, std::string const& msg) {
    for (auto& hi : handlers)
        hi.h->write( {msg, func, file, line, level, getColor(level) } );
}

void Logger::error(const char* file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::ERROR, msg);
}

void Logger::warn(const char* file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::WARNING, msg);
}

void Logger::config(const char* file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::CONFIG, msg);
}

void Logger::trace(const char* file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::TRACE, msg);
}

void Logger::debug(const char* file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::DEBUG, msg);
}

void Logger::info(const char* file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::INFO, msg);
}

const char* Logger::getColor(unsigned int i) {
    const char* col = Color::WHITE;
    for (auto p : colorsLevel) {
        if (p.second >= i)
            col = p.first;
        else
            return col;
    }

    return Color::WHITE;
}
