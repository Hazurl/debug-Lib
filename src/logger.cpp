#include "logger.h"

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

// ===== FORMAT =====

Format::Format(std::string const& fmt) {
    build_abstract_msg(fmt);
}

void Format::build_abstract_msg(std::string const& fmt) {
    abstract_msg = {};

    bool escape = false;
    std::string cur = "";
    unsigned int size = fmt.length();

    for (unsigned int pos = 0; pos < size; ++pos) {
        char const& c = fmt.at(pos);
        if (escape) {
            cur += std::string(1, c);
            escape = false;
        } else if (c == '{') {
            if (cur != "") {
                abstract_msg.push_back( Format::Var(cur) );
                cur = "";
            }
            abstract_msg.push_back( getMetaVar(fmt, pos) );
        } else if (c == '\\') {
            escape = true;
        } else  {
            cur += std::string (1, c);
        }
    }

    if(cur != "")
        abstract_msg.push_back( Format::Var(cur) );
}

Format::Var Format::getMetaVar(std::string const& fmt, unsigned int& pos) {
    ++pos;
    unsigned int size = fmt.length();
    std::string meta = "";
    bool escape = false;
    for (; pos < size; pos++) {
        char const& c = fmt.at(pos);
        if (escape) {
            escape = false;
            meta += std::string(1, c);
        } else if (c == '\\')
            escape = true;
        else if (c == '}') {
            break;
        } else
            meta += std::string(1, c);
    }
    if (pos >= size)
        return Format::Var("{" + meta);

    if (meta == "log" || meta == "logger" || meta == "name")
                                                                        return Format::Var(Format::Var::Type::LOG_NAME);
    if (meta == "func" || meta == "function")
                                                                        return Format::Var(Format::Var::Type::FUNC);
    if (meta == "file")
                                                                        return Format::Var(Format::Var::Type::FILE);
    if (meta == "line")
                                                                        return Format::Var(Format::Var::Type::LINE);
    if (meta == "lvl" || meta == "level")
                                                                        return Format::Var(Format::Var::Type::LEVEL);
    if (meta == "day")
                                                                        return Format::Var(Format::Var::Type::DAY);
    if (meta == "mon" || meta == "month")
                                                                        return Format::Var(Format::Var::Type::MONTH);
    if (meta == "year")
                                                                        return Format::Var(Format::Var::Type::YEAR);
    if (meta == "hour")
                                                                        return Format::Var(Format::Var::Type::HOURS);
    if (meta == "min" || meta == "minute")
                                                                        return Format::Var(Format::Var::Type::MIN);
    if (meta == "sec" || meta == "second")
                                                                        return Format::Var(Format::Var::Type::SEC);
    if (meta == "mil" || meta == "milli" || meta == "millisecond")
                                                                        return Format::Var(Format::Var::Type::MIL);
    if (meta == "mic" || meta == "microsec" || meta == "microsecond")
                                                                        return Format::Var(Format::Var::Type::USEC);
    if (meta == "msg" || meta == "message")
                                                                        return Format::Var(Format::Var::Type::MSG);
    if (meta == "beg" || meta == "begin")
                                                                        return Format::Var(Format::Var::Type::BEG);
    if (meta == "end")
                                                                        return Format::Var(Format::Var::Type::END);
    if (meta == "n" || meta == "endl" || meta == "newline")
                                                                        return Format::Var(Format::Var::Type::NEW_LINE);
    if (meta == "color" || meta == "col")
                                                                        return Format::Var(Format::Var::Type::COLOR);
    if (meta == "clear" || meta == "clr")
                                                                        return Format::Var(Format::Var::Type::CLEAR);
    if (meta == "red")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::RED);
    if (meta == "grn" || meta == "green")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::GREEN);
    if (meta == "blu" || meta == "blue")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::BLUE);
    if (meta == "mag" || meta == "magenta")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::MAGENTA);
    if (meta == "cya" || meta == "cyan")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::CYAN);
    if (meta == "yel" || meta == "yellow")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::YELLOW);
    if (meta == "whi" || meta == "white")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::WHITE);
    if (meta == "bld" || meta == "bold")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::BOLD);
    if (meta == "udl" || meta == "undl" || meta == "underline")
                                                                        return Format::Var(Format::Var::Type::META, Formatting::UNDERLINE);
    try {
                                                                        return Format::Var(std::stol(meta)); 
    } catch (std::invalid_argument const&) {}
                                                                        return Format::Var("");
}

std::string Format::formate(std::vector<Message> msgs, bool with_color) const {
    if (msgs.empty())
        return "";

    std::string out = "";
    long pos_left = 0;

    Message& main = msgs[0];
    unsigned long msg_pos = 0;
    Message& cur = msgs[msg_pos];

    bool end_by_endl = true;
    bool go_to_end = false;

    decltype(abstract_msg)::const_iterator beg;

    for (auto itr = abstract_msg.begin(); itr != abstract_msg.end(); ++itr) {
        end_by_endl = false;
/*
            STRING, FUNC, FILE, LINE, LEVEL,
            DAY, MONTH, YEAR, HOURS, MIN, SEC, MIL, USEC,
            MSG, BEG, END, NEW_LINE, POS,
            COLOR, META
*/
        if (go_to_end) {
            if (itr->type == Format::Var::Type::END) {
                go_to_end = false;
            }
            continue;
        }

        std::string tmp;

        switch(itr->type) {
            case Format::Var::Type::STRING : out += itr->str; pos_left += itr->str.size(); break;
            case Format::Var::Type::LOG_NAME : out += main.name; pos_left += main.name.size(); break;
            case Format::Var::Type::FUNC : out += cur.func; pos_left += cur.func.size(); break;
            case Format::Var::Type::FILE : out += cur.file; pos_left += cur.file.size(); break;
            case Format::Var::Type::LINE : tmp = std::to_string(cur.line); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::LEVEL : tmp = Level::to_string(cur.level); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::DAY : tmp = fill(std::to_string(cur.time.tm_mday), '0', 2); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::MONTH : tmp = fill(std::to_string(cur.time.tm_mon), '0', 2); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::YEAR : tmp = std::to_string(1900 + cur.time.tm_year); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::HOURS : tmp = fill(std::to_string(cur.time.tm_hour), '0', 2); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::MIN : tmp = fill(std::to_string(cur.time.tm_min), '0', 2); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::SEC : tmp = fill(std::to_string(cur.time.tm_sec), '0', 2); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::MIL : tmp = fill(std::to_string(cur.usec / 1000), '0', 3); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::USEC : tmp = fill(std::to_string(cur.usec % 1000), '0', 3); out += tmp; pos_left += tmp.size(); break;
            case Format::Var::Type::MSG : out += cur.msg; pos_left += cur.msg.size(); break;
            case Format::Var::Type::NEW_LINE : out += "\n"; pos_left = 0; end_by_endl = true; break;
            case Format::Var::Type::POS : for (; itr->pos > pos_left; out += " ") pos_left++; break;
            case Format::Var::Type::COLOR : if (with_color) out += main.color;  break;
            case Format::Var::Type::CLEAR : if (with_color) out += Formatting::CLEAR; break;
            case Format::Var::Type::META : if (with_color) out += itr->str; break;

            case Format::Var::Type::BEG:
                beg = itr;
                if (++msg_pos >= msgs.size())
                    go_to_end = true;
                else
                    cur = msgs[msg_pos];
            break;

            case Format::Var::Type::END:
                if (msg_pos + 1 < msgs.size())
                    itr = beg - 1;
            break;

            default: break;
        }
    }

    if (! end_by_endl)
        out += "\n";
    return out;
}

std::string Format::fill(std::string str, char filler, unsigned int size) const {
    for (; size > str.size(); str += std::string(1, filler));
    return str;
}

// ===== HANDLER =====

Handler::Handler () : _color(true),
    _commonFormat("[ {hour}:{min}:{sec} ] {col}from {func} ({line}){clr} : {msg}"),
    _exFormat("{red}{bld}═════════════════════════════════════════════════════════════════════════════════════════════════════{clr}{endl}{endl}"
              "From {bold}{func}{clr} in {bold}{file}{clr} (line {bold}{line}{clr}){endl}"
              "The {day}/{mon}/{year} at {hour}:{min}:{sec} (exactly {mil} {mic} µs){endl}"
              "{red}{bld}/!\\\\ {udl}Exception throwed{clr}{bld}{red} >> {clr}{red}{msg}{clr}{endl}{endl}"
              "{red}{bld}═════════════════════════════════════════════════════════════════════════════════════════════════════{clr}{endl}"),
    _stackFormat("{endl}{bld}{name}{clr}{30}<{clr} {grn}{bld}Stack Trace{clr} >{clr}{endl}"
                 "{beg}{5}{file}@{line} {30}{bld}{udl}{func}{clr}{endl}{end}{endl}"),
    _enteringFormat("{grn}{bld}Entering{clr} {func} ({bld}{msg}{clr})"),
    _exitingFormat("{grn}{bld}Exiting{clr} {func} (return {bld}{msg}{clr})") {}
Handler::~Handler() {}

void Handler::write(Message const& msg, Format const& fmt) {
    writeMulti({msg}, fmt);
}

void Handler::writeMulti(std::vector<Message> const& msgs, Format const& fmt) {
    if (_color)
        append(fmt.formate(msgs, true) + Formatting::CLEAR);
    else 
        append(fmt.formate(msgs, false));
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
    if (!os.is_open())
        throw std::runtime_error("pb");
}

FileHandler::~FileHandler() {
    if (os.is_open())
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

std::map<std::string, Logger> Logger::loggers = {};

Logger& Logger::get(std::string const& name) {
    Logger* parent = nullptr;
    size_t pos = name.rfind(".");
    if (pos != std::string::npos && pos != (name.size() - 1)) {
        std::string parentName = name;
        parentName.erase(pos);
        parent = &Logger::get(parentName);
    }

    if (loggers.find(name) == loggers.end())
        loggers.insert( { name, Logger(name, parent) } );
    return loggers[name];
}

Logger::Logger(std::string const& name, Logger* parent) : name(name), parent(parent) {}
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

void Logger::entering(std::string const& file, std::string const& func, long line, std::vector<std::string> params) {
    tm t = getTime();
    long usec = get_usec();
    stackTr.push_back( {file, func, line, params, usec, t } );

    if (this->level <= Level::TRACE) {
        std::string ps = "";
        if (!params.empty()) {
            bool first = true;
            for (auto p : params) {
                if (!first)
                    ps += ", ";
                else
                    first = false;
                ps += p;
            }
        } else 
            ps = "void";
        for (auto& hi : handlers)
            hi.h->enter( {name, ps, func, file, line, level, getColor(Level::TRACE), usec, t });
        if (parent)
            parent->entering(file, func, line, params);
    }
}

void Logger::exiting(std::string const& file, std::string const& func, long line, std::string const& obj) {
    if (this->level <= Level::TRACE) {
        for (auto& hi : handlers)
            hi.h->exit({name, obj, stackTr.back().func, stackTr.back().file, line, level, getColor(Level::TRACE), get_usec(), getTime() });

        if (parent)
            parent->exiting(file, func, line, obj);
    }

    if (!stackTr.empty())
        stackTr.pop_back();
}

void Logger::stackTrace(std::string const& file, std::string const& func, long line, int depth) {
    if (this->level <= Level::TRACE) {
        std::string ps = "";
        auto itr = stackTr.rbegin();
        bool first = true;
        std::vector<Message> msgs;
        msgs.push_back({name, "", func, file, line, level, Formatting::GREEN, get_usec(), getTime() });
        unsigned int num = 0;
        while (itr != stackTr.rend() && (--depth) != 0) {
            msgs.push_back({ name, std::to_string(num++), itr->func, itr->file, itr->line, level, getColor(Level::TRACE), itr->usec, itr->time });

            if (!first)
                ps += ", ";
            else
                first = false;
            ps += itr->func;
            itr++;
        }

        msgs[0].msg = ps;
        for (auto& hi : handlers)
            hi.h->stack( msgs );

        if (parent)
            parent->stackTrace(file, func, line, depth);
    }
}

void Logger::log(std::string const& file, std::string const& func, long line, unsigned int level, std::string const& msg) {
    if (this->level <= level) {
        for (auto& hi : handlers)
            hi.h->common( {name, msg, func, file, line, level, getColor(level), get_usec(), getTime() } );

        if (parent)
            parent->log(file, func, line, level, msg);
    }
}

void Logger::error(std::string const& file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::ERROR, msg);
}

void Logger::warn(std::string const& file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::WARNING, msg);
}

void Logger::config(std::string const& file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::CONFIG, msg);
}

void Logger::trace(std::string const& file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::TRACE, msg);
}

void Logger::debug(std::string const& file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::DEBUG, msg);
}

void Logger::info(std::string const& file, std::string const& func, long line, std::string const& msg) {
    log(file, func, line, Level::INFO, msg);
}

const char* Logger::getColor(unsigned int i) {
    const char* col = Formatting::WHITE;
    for (auto p : colorsLevel) {
        if (p.second >= i)
            col = p.first;
        else
            return col;
    }

    return Formatting::WHITE;
}

template<> std::string stringify<std::string> (std::string t)                { return t; }
template<> std::string stringify<int> (int t)                                { return std::to_string(t); }
template<> std::string stringify<long double> (long double t)                { return std::to_string(t); }
template<> std::string stringify<long> (long t)                              { return std::to_string(t); }
template<> std::string stringify<unsigned long> (unsigned long t)            { return std::to_string(t); }
template<> std::string stringify<float> (float t)                            { return std::to_string(t); }
template<> std::string stringify<double> (double t)                          { return std::to_string(t); }
template<> std::string stringify<long long> (long long t)                    { return std::to_string(t); }
template<> std::string stringify<unsigned> (unsigned t)                      { return std::to_string(t); }
template<> std::string stringify<unsigned long long> (unsigned long long t)  { return std::to_string(t); }
template<> std::string stringify<bool> (bool t)                              { return t ? std::string("true") : std::string("false"); }
