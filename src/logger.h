#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <sys/time.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>

#define LOG(l, m...) log(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (l), m )

#define ERROR(m...) error(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, m )
#define WARN(m...) warn(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, m )
#define CONFIG(m...) config(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, m )
#define TRACE(m...) trace(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, m )
#define DEBUG(m...) debug(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, m )
#define INFO(m...) info(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, m )

#define ENTERING(p...) entering(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, p )
#define EXITING(s...) exiting(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__ , s )
#define RET(s...) ret(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__ , s)
#define RET_STR(s...) ret(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__ , stringify(s), s)

#define STRACKTRACE(d...) stackTrace(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__ , d )
#define THROWEXCEPTION(t, m...) throwException<t> (__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__ , m , false );

namespace haz {

class Level {
public:
    static const unsigned int 
        OFF = 1000,
        EXCEPTION = 700,
        ERROR = 600,
        WARNING = 500,
        CONFIG = 400,
        TRACE = 300,
        DEBUG = 200,
        INFO = 100,
        ALL = 0;

    static std::string to_string(unsigned int i);
};


class Formatting {
public:
    static constexpr char const* RED = "\033[31m";
    static constexpr char const* GREEN = "\033[32m"; 
    static constexpr char const* YELLOW = "\033[33m"; 
    static constexpr char const* BLUE = "\033[34m";
    static constexpr char const* MAGENTA = "\033[35m"; 
    static constexpr char const* CYAN = "\033[36m";
    static constexpr char const* WHITE = "\033[37m";
    static constexpr char const* CLEAR = "\033[0m";
    static constexpr char const* BOLD = "\033[1m";
    static constexpr char const* UNDERLINE = "\033[4m";
};

struct Message {
    std::string name;
    std::string msg;
    std::string func;
    std::string file;
    long line;
    unsigned int level;
    const char* color;
    long usec;
    tm time;
};


class Format {
public:
    Format(std::string const& fmt);
    //~Format();

    std::string formate(std::vector<Message> msgs, bool with_color) const;

private:
    class Var { 
    public:
        enum class Type {
            STRING, LOG_NAME, FUNC, FILE, LINE, LEVEL,
            DAY, MONTH, YEAR, HOURS, MIN, SEC, MIL, USEC,
            MSG, BEG, END, NEW_LINE, POS,
            COLOR, META, CLEAR
        };

        Type type = Type::STRING;
        std::string str = "";
        long pos = 0;

        Var(std::string const& s) : type(Type::STRING), str(s) {}
        Var(long const& l) : type(Type::POS), pos(l) {}
        Var(Type const& t, std::string const& s = "") : type(t), str(s) {}
    };

    std::vector<Var> abstract_msg;

    void build_abstract_msg(std::string const& fmt);
    Var getMetaVar(std::string const& fmt, unsigned int& pos);
    std::string fill(std::string str, char filler, unsigned int size) const;
};


class Handler {
public:
    Handler* color(bool b) { _color = b; return this; }
    
    Handler* commonFormat(Format const& f) { _commonFormat = f; return this; }
    Handler* exFormat(Format const& f) { _exFormat = f; return this; }
    Handler* stackFormat(Format const& f) { _stackFormat = f; return this; }
    Handler* enteringFormat(Format const& f) { _enteringFormat = f; return this; }
    Handler* exitingFormat(Format const& f) { _exitingFormat = f; return this; }
    Handler* commonFormat(std::string const& f) { _commonFormat = Format(f); return this; }
    Handler* exFormat(std::string const& f) { _exFormat = Format(f); return this; }
    Handler* stackFormat(std::string const& f) { _stackFormat = Format(f); return this; }
    Handler* enteringFormat(std::string const& f) { _enteringFormat = Format(f); return this; }
    Handler* exitingFormat(std::string const& f) { _exitingFormat = Format(f); return this; }

    Handler ();
    virtual ~Handler();

    void common(Message const& msg) { write (msg, _commonFormat); }
    void exception(Message const& msg) { write (msg, _exFormat); }
    void stack(std::vector<Message> const& msgs) { writeMulti (msgs, _stackFormat); }
    void enter(Message const& msg) { write (msg, _enteringFormat); }
    void exit(Message const& msg) { write (msg, _exitingFormat); }

protected:
    virtual void append (std::string const& msg) = 0;

private:
    bool _color;
    Format _commonFormat;
    Format _exFormat;
    Format _stackFormat;
    Format _enteringFormat;
    Format _exitingFormat;

    void write (Message const& msg, Format const& fmt);
    void writeMulti (std::vector<Message> const& msgs, Format const& fmt);
};

class ConsoleHandler : public Handler {
public:
    ConsoleHandler();
    ~ConsoleHandler();

private:
    void append (std::string const& msg);

};

class FileHandler : public Handler {
public:
    FileHandler(const char* filename, bool append = true);
    ~FileHandler();

private:
    void append (std::string const& msg);

    std::ofstream os;

};

class StreamHandler : public Handler {
public:
    StreamHandler(std::ostream const& os);
    ~StreamHandler();

private:
    void append (std::string const& msg);

    std::ostream os;

};


class Logger {
public:
    Logger(std::string const& name = "", Logger* parent = nullptr);
    ~Logger();

    static Logger& get(std::string const& name);

    Logger& clearHandlers();
    Logger& addHandler(Handler* h, bool destroy_by_logger = true);

    Logger& setLevel(int l);
    Logger& setColorsLevel(std::vector< std::pair<const char*, unsigned int> > vcl);

    bool isEnabled(unsigned int l);

    void entering(std::string const& file, std::string const& func, long line, std::vector<std::string> params);
    void exiting(std::string const& file, std::string const& func, long line, std::string const& obj);
    void stackTrace(std::string const& file, std::string const& func, long line, int depth = 0); // 0 = all

    template<class T>
    void throwException (std::string const& file, std::string const& func, long line, std::string const& msg, bool throw_it) {
        if (this->level <= Level::EXCEPTION) {
            for (auto& hi : handlers)
                hi.h->exception( {name, msg, func, file, line, level, Formatting::RED, get_usec(), getTime() });
            if (parent)
                parent->throwException<T>(file, func, line, msg, false);
        }

        this->stackTrace(file, func, line, 0);
        if (!throw_it) return;
        T tmp(msg);
        throw tmp;
    }

    template<class T>
    T const& ret(std::string const& file, std::string const& func, long line, std::string const& str, T const& obj_ret) {
        this->exiting(file, func, line, str);
        return obj_ret;
    }

    void ret(std::string const& file, std::string const& func, long line, std::string const& str) {
        this->exiting(file, func, line, str);
    }

    void log(std::string const& file, std::string const& func, long line, unsigned int level, std::string const& msg);
    void error(std::string const& file, std::string const& func, long line, std::string const& msg);
    void warn(std::string const& file, std::string const& func, long line, std::string const& msg);
    void config(std::string const& file, std::string const& func, long line, std::string const& msg);
    void trace(std::string const& file, std::string const& func, long line, std::string const& msg);
    void debug(std::string const& file, std::string const& func, long line, std::string const& msg);
    void info(std::string const&  file, std::string const& func, long line, std::string const& msg);

private:

    void setParent (Logger* l) { parent = l; }

    tm getTime() {
        time_t rawtime;
        time(&rawtime);
        return *localtime(&rawtime);
    }

    long get_usec() {
        struct timeval tv;
        gettimeofday (&tv, nullptr);
        return tv.tv_usec;
    }

    static std::map<std::string, Logger> loggers;

    struct stackInfo {
        std::string const& file;
        std::string func;
        long line;
        std::vector<std::string> params;
        long usec;
        tm time;
    };

    struct HandlerInfo {
        Handler* h;
        bool destroy_by_logger;
    };

    std::vector<stackInfo> stackTr;
    std::list<HandlerInfo> handlers;

    unsigned int level = Level::ALL;
    std::string name;
    Logger* parent = nullptr;

    const char* getColor(unsigned int i);
    std::vector< std::pair<const char*, unsigned int> > colorsLevel;
};

static std::string getScopedClassMethod( std::string thePrettyFunction ) {
  size_t index = thePrettyFunction.find( "(" );
  if ( index == std::string::npos )
    return thePrettyFunction;  // Degenerate case 

  thePrettyFunction.erase( index );

  index = thePrettyFunction.rfind( " " );
  if ( index == std::string::npos )
    return thePrettyFunction;  // Degenerate case 

  thePrettyFunction.erase( 0, index + 1 );

  return thePrettyFunction;   // The scoped class name. 
}


template<typename T>
std::string stringify (T t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template<> std::string stringify<std::string> (std::string t);
template<> std::string stringify<int> (int t);
template<> std::string stringify<long double> (long double t);
template<> std::string stringify<long> (long t);
template<> std::string stringify<unsigned long> (unsigned long t);
template<> std::string stringify<float> (float t);
template<> std::string stringify<double> (double t);
template<> std::string stringify<long long> (long long t);
template<> std::string stringify<unsigned> (unsigned t);
template<> std::string stringify<unsigned long long> (unsigned long long t);
template<> std::string stringify<bool> (bool t);

} // namespace haz

#endif
