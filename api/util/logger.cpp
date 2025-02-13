#pragma once
#include <cstdio>
#include "string.h"
#include <stdlib.h>
#include <chrono>
#include <ctime>

namespace util
{
    // https://stackoverflow.com/a/10467633
    const std::string currentDateTime()
    {
        time_t now = time(0);
        struct tm tstruct;
        char buf[80];
        tstruct = *localtime(&now);
        strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

        return buf;
    }

    class Logger
    {
    private:
        bool enabled;
        std::FILE *file;

    public:
        Logger(bool enabled = false, const char *filePath = "") : enabled(enabled)
        {
            if (strlen(filePath) == 0)
            {
                this->file = std::fopen(filePath, "w");
            }
        }

        ~Logger()
        {
            if (file != nullptr)
            {
                fclose(file);
            }
        }

        void info(std::string message)
        {
            if (!this->enabled)
                return;
            printf("%s [INFO]: %s\n",
                   currentDateTime().c_str(),
                   message.c_str());
        }

        void error(std::string message)
        {
            printf("%s [ERROR]: %s\n",
                   currentDateTime().c_str(),
                   message.c_str());
        }
    };
}