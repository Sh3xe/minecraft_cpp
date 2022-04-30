#pragma once

#include <string>
#include <array>

inline std::string to_string( const std::string &str ) { return str; }

namespace sd
{
	enum class LogLevel
	{
		trace,
		debug,
		info,
		warn,
		error,
		fatal
	};

	class Logger
	{
	public:
		Logger(const Logger&)           = delete;
		Logger operator=(const Logger&) = delete;

		inline static Logger &get()
		{
			static Logger instance;
			return instance;
		}

		inline void set_level_enabled( LogLevel level, bool enabled)
		{
			m_enabled_levels[ static_cast<size_t>(level) ] = enabled;
		}

		inline bool get_level_enabled( LogLevel level ) const
		{
			return m_enabled_levels[static_cast<size_t>(level)];
		}

		template <typename ...types>
		void log(LogLevel log_level, types&& ...args)
		{
			using ::to_string;
			using std::to_string;
			
			if ( !m_enabled_levels[static_cast<size_t>(log_level)] ) return;

			switch(log_level)
			{
				case LogLevel::debug:
					send_to_streams( { "[DEBUG]:", to_string(args)... } );
					break;
				case LogLevel::error:
					send_to_streams( { "[ERROR]:", to_string(args)... } );
					break;
				case LogLevel::fatal:
					send_to_streams( { "[FATAL]:", to_string(args)... } );
					break;
				case LogLevel::info:
					send_to_streams( { "[INFO ]:", to_string(args)... } );
					break;
				case LogLevel::trace:
					send_to_streams( { "[TRACE]:", to_string(args)... } );
					break;
				case LogLevel::warn:
					send_to_streams( { "[WARN ]:", to_string(args)... } );
					break;
				default: break;
			}
		}

	private:
		void send_to_streams( std::initializer_list<std::string> init_list );

		Logger()
		{
			m_enabled_levels = {true, true, true, true, true, true};
		}

		std::array<bool, 6> m_enabled_levels;

	};

} // namespace sd

#define SD_LOGGER sd::Logger::get()

#if defined( NDEBUG )

	#define SD_DEBUG(...)
	#define SD_TRACE(...)

#else

	#define SD_DEBUG(...) sd::Logger::get().log( sd::LogLevel::debug, __VA_ARGS__ )
	#define SD_TRACE(...) sd::Logger::get().log( sd::LogLevel::trace, __VA_ARGS__ )

#endif

#define SD_INFO(...)  sd::Logger::get().log( sd::LogLevel::info, __VA_ARGS__ )
#define SD_WARN(...)  sd::Logger::get().log( sd::LogLevel::warn, __VA_ARGS__ )
#define SD_ERROR(...) sd::Logger::get().log( sd::LogLevel::error, __VA_ARGS__ )
#define SD_FATAL(...) sd::Logger::get().log( sd::LogLevel::fatal, __VA_ARGS__ )