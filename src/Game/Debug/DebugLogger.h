#pragma once
#include <iostream>
#include <map>

//////////////////
// Debug Logger //
/////////////////////////////////////////////////////////////////////////////
// Simple wrapper to output helpful debug logs.                            //
// Unnecessary in the final build, but it's super handy during development //
////////////////////////////////////////////////////////////////////////////
namespace Logger
{
    enum LogSource
    {
        GAME,
        MUSIC,
        UI,
        WAVWRITER,
    };

    inline std::map<LogSource, std::string> log_string =
    {
        {
            GAME, "[Game]"
        },
        {
            MUSIC, "[Music]"
        },
        {
            UI, "[UI]"
        },
        {
            WAVWRITER, "[WavWriter]"
        }
    };

    inline std::string GetLogString(const LogSource source)
    {
        return log_string[source];
    }

    inline void PrintLog(const LogSource source, std::string out)
    {
        std::cout << GetLogString(source) << " " << out << '\n';
    }
}
