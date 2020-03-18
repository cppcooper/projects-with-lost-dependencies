/*
v1.1.0
The MIT License (MIT)

Copyright (c) <2014> <Josh S, Cooper>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.*/

#ifndef _DBG_LOG_
#define _DBG_LOG_
#include "lib_linkers\dbglinker.h"

#include <assert.h>
#include <cstdio>

#include <vector>

#include <fstream>
#include <sstream>
#include <mutex>
#include <thread>
#include <functional>

#ifndef LOG_MAX_LEVEL
	#define LOG_MAX_LEVEL (dbg::logFATAL + dbg::logERROR + dbg::logWARNING + dbg::logINFO + dbg::logDEBUG1 + dbg::logDEBUG2 + dbg::logDEBUG3 + dbg::logDEBUG4 )
#endif


#include <chrono>
#include <string>
#include <map>

inline std::string monthConversion( std::string month )
{
	static std::map<std::string, std::string> converter;
	converter["Jan "] = "01";
	converter["Feb "] = "02";
	converter["Mar "] = "03";
	converter["Apr "] = "04";
	converter["May "] = "05";
	converter["Jun "] = "06";
	converter["Jul "] = "07";
	converter["July "] = "07";
	converter["Aug "] = "08";
	converter["Sep "] = "09";
	converter["Sept "] = "09";
	converter["Oct "] = "10";
	converter["Nov "] = "11";
	converter["Dec "] = "12";

	return converter[month];
}

inline std::string NowTime()
{
	std::string result;
	std::string Month;
	std::string WeekDay;
	std::string Day;
	std::string Time;
	std::string Year;
	size_t current_pos = 0;

	std::chrono::high_resolution_clock clock;
	auto timeSinceEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(clock.now().time_since_epoch()).count();
	int ms_time;
	ms_time = timeSinceEpoch;
	ms_time = ms_time % 1000;
	time_t the_time = clock.to_time_t( clock.now() );

	//result = ctime( &the_time );
	//find xx:xx:xx then remove and place at the end of the string
	size_t last_pos = 0;
	current_pos = result.find_first_of( ' ', last_pos );
	WeekDay = result.substr( last_pos, current_pos - last_pos );
		
	last_pos = current_pos;
	current_pos = result.find_first_of( ' ', last_pos+1 );
	Month = result.substr( last_pos+1, current_pos - last_pos );

	last_pos = current_pos;
	current_pos = result.find_first_of( ' ', last_pos+1 );
	Day = result.substr( last_pos+1, ( current_pos-1 ) - last_pos );

	last_pos = current_pos;
	current_pos = result.find_first_of( ' ', last_pos+1 );
	Time = result.substr( last_pos+1, ( current_pos-1 ) - last_pos ) + "." + std::to_string( ms_time );

	last_pos = current_pos;
	Year = result.substr( last_pos+1, ( result.size() - 2 ) - last_pos );
		

	result = Year + "-" + monthConversion(Month) + "-" + Day + " - " + Time;
	return result;
}


namespace dbg
{
	//Enums representing the different log levels
	// Implemented as bits in a Byte as to facilitate turning specific levels on and off with a #define macro
	enum LogLevel 
	{ 
		logFATAL = 1 << 0,			logERROR = 1 << 1, 
		logWARNING = 1 << 2,		logINFO = 1 << 3, 
		logDEBUG1 = 1 << 4,		logDEBUG2 = 1 << 5, 
		logDEBUG3 = 1 << 6,		logDEBUG4 = 1 << 7 
	};

	//Forward declaration for friend statement
	class FileLog_Mgr;

	/*
	Logger template class

	Implemented with a built-in Object of the OutputPolicy class
	Logger expects very little of this Policy class itself as its methods' only use of the
	Output Object are where it is being overwritten with a new OutputPolicy object

	REQUIRED are an operator= overload + copy constructor
	*/
	template <typename OutputPolicy>
	class Logger
	{
		friend FileLog_Mgr;

		public:
			virtual ~Logger()
			{
			}
			Logger(){}
			Logger(const Logger& other)
			{
				Output = other.Output;
			}
			Logger& operator=(const Logger& other)
			{
				Output = other.Output;
				return *this;
			}

			inline LogLevel& ReportingLevel()
			{
				static LogLevel reportingLevel = logDEBUG4;
				return reportingLevel;
			}
			inline std::string ToString(LogLevel level)
			{
				switch ( level )
				{
					case logFATAL:
						return "\t~FATAL~\t\t";
						break;

					case logERROR:
						return "\tERROR: \t\t";
						break;

					case logWARNING:
						return "WARNING: \t";
						break;

					case logINFO:
						return "INFO:\t  ";
						break;

					case logDEBUG1:
						return "DEBUG1:\t\t";
						break;

					case logDEBUG2:
						return "DEBUG2:\t\t  ";
						break;

					case logDEBUG3:
						return "DEBUG3:\t\t    ";
						break;

					case logDEBUG4:
						return "DEBUG4:\t\t      ";
						break;
				}
			}
			inline std::ostringstream& Get(LogLevel level = logINFO)
			{
				    buffer << std::endl << " -  " << NowTime() << " - \t";
					buffer << ToString(level);
					return buffer;
			}
			inline void Lock()
			{
				Log_mutex.lock();
			}
			inline void Unlock()
			{
				Log_mutex.unlock();
			}

		protected:
			std::ostringstream buffer;
			//bool automate_mutex = false;
			std::mutex Log_mutex;
			OutputPolicy Output;  //templated output
	};

	/*
	*/
	class FileStream_Policy
	{
		public:
			virtual ~FileStream_Policy()
			{
				if ( file_stream.is_open() )
				{
					file_stream.close();
				}
			}
			FileStream_Policy(){};
			FileStream_Policy(const FileStream_Policy& other)
			{
				app = other.app;
				file_name = other.file_name;
			}
			FileStream_Policy& operator=(const FileStream_Policy& other)
			{
				app = other.app;
				file_name = other.file_name;
				return *this;
			}

			inline std::ofstream& Stream()
			{
				if ( !file_stream.is_open() )
				{
					if ( app )
					{
						file_stream.open(file_name, std::ofstream::out | std::ofstream::app);
					}
					else
					{
						file_stream.open(file_name, std::ofstream::out);
					}
				}
				return file_stream;
			}
			inline std::wstring& FileName()
			{
				return file_name;
			}
			inline void set_append(bool append)
			{
				app = append;
			}

		protected:
			bool app = false;
			std::ofstream file_stream;
			std::wstring file_name;
	};

	//Type Definition for a File Log using Logger<FileStream_Policy>
	using FileLog = Logger<FileStream_Policy>;

	class FileLog_Mgr
	{
		public:
			static int WriteInterval;
			static int StartDelay;

			static std::ostringstream& Get(int index, LogLevel level)
			{
				try
				{
					return getLogs()[index].Get(level);
				}
				catch ( int exception )
				{
					assert("Indexed Log does not exist");
					exit(-404);
				}
			}
			static void Lock(int index)
			{
				try
				{
					getLogs()[index].Lock();
				}
				catch ( int exception )
				{
					assert("Indexed Log does not exist");
					exit(-404);
				}
			}
			static void Unlock(int index)
			{
				try
				{
					getLogs()[index].Unlock();
				}
				catch ( int exception )
				{
					assert("Indexed Log does not exist");
					exit(-404);
				}
			}
			static int RegisterNewLog(std::wstring file_name, bool append = false)
			{
				if ( !ThreadRunning )
				{
					for ( int i = 0; i < getLogs().size(); ++i )
					{
						if ( getLogs()[i].Output.FileName() == file_name )
							return -2;
					}
					FileLog newLog;
					newLog.Output.set_append(append);
					newLog.Output.FileName() = file_name;
					getLogs().push_back(newLog);
					return getLogs().size() - 1;
				}
				else
				{
					Stop();
					if ( !ThreadRunning )
					{
						for ( int i = 0; i < getLogs().size(); ++i )
						{
							if ( getLogs()[i].Output.FileName() == file_name )
								return -2;
						}
						FileLog newLog;
						newLog.Output.set_append(append);
						newLog.Output.FileName() = file_name;
						getLogs().push_back(newLog);
						return getLogs().size() - 1;
						Start();
					}
					else
					{
						return -1;
					}
				}
			}
			static void set_LogReportingLevel(int log_index, LogLevel reportLevel)
			{
				if ( log_index >= 0 && log_index < getLogs().size() )
					getLogs()[log_index].ReportingLevel() = reportLevel;
			}
			static bool CheckIndex(int index)
			{
				if ( index >= 0 && index < getLogs().size() )
					return true;
				return false;
			}
			static bool macroCheck(int index, LogLevel level)
			{
				if ( index >= 0 && index < getLogs().size() )
				{
					if ( level > getLogs()[index].ReportingLevel() || !getLogs()[index].Output.Stream() )
						return false;
					return true;
				}
				return false;
			}
			static bool Start()
			{
				if ( WtL_Thread.joinable() )
					return false;

				ThreadRunning = true;
				WtL_Thread = std::thread(&FileLog_Mgr::WriteToLogs);
				std::this_thread::sleep_for(std::chrono::milliseconds(StartDelay));
				return true;
			}
			static bool Stop()
			{
				if ( WtL_Thread.joinable() )
				{
					ThreadRunning = false;
					WtL_Thread.join();
					return true;
				}
				return false;
			}

		protected:
			static std::vector<FileLog>& getLogs()
			{
				static std::vector<FileLog> Logs;
				return Logs;
			}

		private:
			virtual ~FileLog_Mgr()
			{
				Stop();
			}
			static bool ThreadRunning;
			static std::thread WtL_Thread;

			static void WriteToLogs()
			{
				while ( ThreadRunning )
				{
					for ( int i = 0; i < getLogs().size(); ++i )
					{
						getLogs()[i].Output.Stream() << getLogs()[i].buffer.str();
						getLogs()[i].buffer.str("");
						getLogs()[i].Output.Stream().flush();
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(WriteInterval));
				}

				//There might be buffered data
				for ( int i = 0; i < getLogs().size(); ++i )
				{
					getLogs()[i].Output.Stream() << getLogs()[i].buffer.str();
					getLogs()[i].buffer.str("");
					getLogs()[i].Output.Stream().flush();
				}
			}
	};
}

#define LOGFILEX_LOCK(index, level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(index, level) ); \
	else dbg::FileLog_Mgr::Get(index, level)

#define LOGFILEX_UNLOCK(index, level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(index, level) ); \
	else dbg::FileLog_Mgr::Get(index, level)

#define LOGFILEX(index, level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(index, level) ); \
	else dbg::FileLog_Mgr::Get(index, level)

#define LOGFILE1(level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(0, level) ); \
	else dbg::FileLog_Mgr::Get(0, level)

#define LOGFILE2(level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(1, level) ); \
	else dbg::FileLog_Mgr::Get(1, level)

#define LOGFILE3(level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(2, level) ); \
	else dbg::FileLog_Mgr::Get(2, level)

#define LOGFILE4(level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(3, level) ); \
	else dbg::FileLog_Mgr::Get(3, level)

#define LOGFILE5(level) \
	if ( level & ~LOG_MAX_LEVEL || !dbg::FileLog_Mgr::macroCheck(4, level) ); \
	else dbg::FileLog_Mgr::Get(4, level)

#endif