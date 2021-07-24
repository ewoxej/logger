#ifndef LOG_H
#define LOG_H

#include <fstream>
#include <iostream>
#include <mutex>
#include <string>
#include <sstream>
#include <map>

#undef DEBUG

enum ELogLevel
{
    INFO,
    DEBUG,
    WARNING,
    ERROR
};
class Logger;
class LoggerWrapper
{
public:
    static Logger* getLogger( const std::string& str );
private:
    static std::map<std::string,Logger*> m_loggers;
};

class StreamValue
{
  private:
    std::stringstream m_ss;
    Logger* m_parent;
  public:
    StreamValue( Logger* parent ) : m_parent( parent ){}
    StreamValue( const StreamValue& copy ){ m_parent = copy.m_parent; }
    ~StreamValue();
    template <typename T>
    StreamValue & operator << ( const T& data )
    {
        m_ss << data;
        return *this;
    }
};

class Logger
{
public:
    
    Logger( std::ostream* stream = &std::cout ): m_out( stream ) {}
    Logger( std::string fileName );
    ~Logger();
    
    void setPrefix( const std::string& prefix = std::string() );
    void setLogLevel( ELogLevel level );
    StreamValue operator()( ELogLevel level );
    operator std::ostream&() { return *m_out; }
    void logTo( const std::string& content );
    template<typename T>
    Logger& operator<<( const T& data )
    {
        setLogLevel( INFO );
        std::stringstream ss;
        ss<<data;
        logTo( ss.str() );
        return *this;
    }
private:
    std::ostream* m_out;
    std::mutex m_mutex;
    std::string m_prefix;
    ELogLevel m_level;
    std::string m_fileName;
    
    static std::string timeNow();
    static std::string logLevelAsString( ELogLevel level );
};

inline Logger& getLogger( const std::string& fileName = std::string(),
                          const std::string& prefix = std::string() )
{
    auto inst = LoggerWrapper::getLogger( fileName );
    inst->setPrefix( prefix );
    return *inst;
}

#define auto auto&
#endif //LOG_H
