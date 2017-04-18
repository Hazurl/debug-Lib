#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <ctime>
#include <sys/time.h>
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <list>

#define RST  "\033[0m"
#define BLD  "\033[1m"
#define UDL "\33[4m"

#define log(l, m) _log(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (l), (m));

#define error(m) _error(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define warn(m) _warn(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define config(m) _config(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define trace(m) _trace(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define debug(m) _debug(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));
#define info(m) _info(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (m));

#define entering(p) _entering(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (p));
#define exiting(p...) _exiting(__FILE__, getScopedClassMethod(__PRETTY_FUNCTION__), __LINE__, (p));


namespace haz {

class Level {
public:
    static const unsigned int 
        OFF = 1000,
        ERROR = 600,
        WARNING = 500,
        CONFIG = 400,
        TRACE = 300,
        DEBUG = 200,
        INFO = 100;

    static std::string to_string(unsigned int i);
};


class Color {
public:
    static constexpr char const* RED = "\033[31m";
    static constexpr char const* GREEN = "\033[32m"; 
    static constexpr char const* YELLOW = "\033[33m"; 
    static constexpr char const* BLUE = "\033[34m";
    static constexpr char const* MAGENTA = "\033[35m"; 
    static constexpr char const* CYAN = "\033[36m";
    static constexpr char const* WHITE = "\033[37m";
};

struct Message {
    std::string msg;
    std::string func;
    std::string file;
    long line;
    unsigned int level;
    const char* color;
};


class Handler {
public:
    Handler* color(bool b) { _color = b; return this; }
    Handler* indentation(unsigned int i) { _indentation = i; return this; }
    Handler* format(std::string const& f) { _format = f; return this; }

    Handler ();
    virtual ~Handler();
    void write (Message const& msg);

protected:
    virtual void append (std::string const& msg) = 0;

private:
    bool _color;
    unsigned int _indentation;
    std::string _format;

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
        Logger(const char* name = "");
        ~Logger();

        static Logger& get(const char* name);

        Logger& clearHandlers();
        Logger& addHandler(Handler* h, bool destroy_by_logger = true);
        
        Logger& setLevel(int l);
        Logger& setColorsLevel(std::vector< std::pair<const char*, unsigned int> > vcl);

        bool isEnabled(unsigned int l);

        void _entering(const char* file, std::string const& func, long line, std::vector<std::string> params);
        void _exiting(const char* file, std::string const& func, long line, std::string const& obj);
        void _stackTrace(const char* file, std::string const& func, long line, int depth = 0); // 0 = all

        template<class T>
        void _throwException (const char* file, std::string const& func, long line, int level, std::string const& msg) {
            error(file, func, line, "Throw Exception >> " + msg);
            stackTrace(file, func, line);
            throw T(msg);
        }

        void _log(const char* file, std::string const& func, long line, unsigned int level, std::string const& msg);
        void _error(const char* file, std::string const& func, long line, std::string const& msg);
        void _warn(const char* file, std::string const& func, long line, std::string const& msg);
        void _config(const char* file, std::string const& func, long line, std::string const& msg);
        void _trace(const char* file, std::string const& func, long line, std::string const& msg);
        void _debug(const char* file, std::string const& func, long line, std::string const& msg);
        void _info(const char* file, std::string const& func, long line, std::string const& msg);

private:

    static std::map<const char*, Logger> loggers;

    struct stackInfo {
        const char* file;
        std::string const& func;
        long line;
        std::vector<std::string> params;
    };

    struct HandlerInfo {
        Handler* h;
        bool destroy_by_logger;
    };

    std::stack<stackInfo> stackTr;
    std::list<HandlerInfo> handlers;

    unsigned int level;
    const char* name;

    const char* getColor(unsigned int i);
    std::vector< std::pair<const char*, unsigned int> > colorsLevel;
};

static std::string getScopedClassMethod( std::string thePrettyFunction )
{
  size_t index = thePrettyFunction . find( "(" );
  if ( index == std::string::npos )
    return thePrettyFunction;  // Degenerate case 

  thePrettyFunction . erase( index );

  index = thePrettyFunction . rfind( " " );
  if ( index == std::string::npos )
    return thePrettyFunction;  // Degenerate case 

  thePrettyFunction . erase( 0, index + 1 );

  return thePrettyFunction;   // The scoped class name. 
}

/*
template<class T>
std::string stringify (T const& t) {
    std::ostringstream os;
    os << t;
    return os.str();
}

template<> std::string stringify<std::string> (std::string const& t)                { return t; }
template<> std::string stringify<int> (int const& t)                                { return std::to_string(t); }
template<> std::string stringify<long double> (long double const& t)                { return std::to_string(t); }
template<> std::string stringify<long> (long const& t)                              { return std::to_string(t); }
template<> std::string stringify<unsigned long> (unsigned long const& t)            { return std::to_string(t); }
template<> std::string stringify<float> (float const& t)                            { return std::to_string(t); }
template<> std::string stringify<double> (double const& t)                          { return std::to_string(t); }
template<> std::string stringify<long long> (long long const& t)                    { return std::to_string(t); }
template<> std::string stringify<unsigned> (unsigned const& t)                      { return std::to_string(t); }
template<> std::string stringify<unsigned long long> (unsigned long long const& t)  { return std::to_string(t); }
*/
} // namespace haz
#endif