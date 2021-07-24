#include "log.h"

Logger* LoggerWrapper::getLogger( const std::string& str )
{
    std::unique_lock<std::mutex> lock(m_wrapperMutex);
        if( m_loggers.find(str) == m_loggers.end() )
        {
            if( str.empty() )
                m_loggers.insert( std::make_pair( str, new Logger() ) );
            else
                m_loggers.insert( std::make_pair( str, new Logger( str ) ) );
        }
        return m_loggers.at( str );
}

std::map<std::string,Logger*> LoggerWrapper::m_loggers;
std::mutex LoggerWrapper::m_wrapperMutex;

Logger::Logger( std::ostream* stream )
: m_out( stream )
, m_level( INFO )
{
}

Logger::Logger( std::string fileName )
: m_fileName( fileName )
, m_out( new std::ofstream( fileName ) )
, m_level( INFO )
{
}

std::string Logger::timeNow()
{
    time_t rawtime;
    struct tm* timeinfo;
    time ( &rawtime );
    timeinfo = localtime( &rawtime );
    char buff[32];
    strftime( buff, sizeof( buff ), "%d/%b/%Y, %H:%M:%S", timeinfo );
    return buff;
}

std::string Logger::logLevelAsString( ELogLevel level )
{
    switch( level )
    {
        case INFO: return "INFO";break;
        case DEBUG: return "DEBUG";break;
        case WARNING: return "WARNING";break;
        case ERROR: return "ERROR"; break;
        default: return "";
    }
}

void Logger::setPrefix( const std::string& prefix )
{
    m_prefix = prefix;
}

void Logger::setLogLevel( ELogLevel level )
{
    m_level = level;
}

void Logger::logTo( const std::string& content )
{   
    std::unique_lock<std::mutex> lock( m_mutex );
    *m_out << timeNow() << "; " << logLevelAsString( m_level ) << "; "
    << m_prefix << "(" << std::this_thread::get_id() << ")" << ": " << content << std::endl;
}

StreamValue Logger::operator()( ELogLevel level )
{
    setLogLevel( level );
    return StreamValue( this );
}

Logger::~Logger()
{
    if( !m_fileName.empty() )
        delete m_out;
}

StreamValue::~StreamValue()
{
    std::string str = m_ss.str();
    if(!str.empty())
        m_parent->logTo( str );
}
