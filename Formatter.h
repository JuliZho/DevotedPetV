#ifndef FORMATTER
#define FORMATTER

#include <iostream>
#include <string>
#include <memory>
#include <iomanip>
#include <sys/ioctl.h>
#include <unistd.h>

class OutputFormattingStrategy
{
public:
    virtual ~OutputFormattingStrategy() {}
    virtual void formatAndPrint(const std::string &message) const = 0;
};

class ColorAndCenterOutput : public OutputFormattingStrategy
{
private:
    std::string textColour;

public:
    ColorAndCenterOutput(const std::string &colour) : textColour(colour) {}

    void formatAndPrint(const std::string &message) const override
    {
        int terminalWidth = getTerminalWidth();
        int messageWidth = message.length();
        int paddingWidth = (terminalWidth - messageWidth) / 2;
        std::cout << std::setw(paddingWidth) << textColour << message << "\e[0m" << std::endl;
    }

private:
    int getTerminalWidth() const
    {
        struct winsize size;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        return size.ws_col;
    }
};

class ColorOutput : public OutputFormattingStrategy
{
private:
    std::string textColour;

public:
    ColorOutput(const std::string &colour) : textColour(colour) {}

    void formatAndPrint(const std::string &message) const override
    {
        std::cout << textColour << message << "\e[0m" << std::endl;
    }
};

class OutputFormatter
{
private:
    std::shared_ptr<OutputFormattingStrategy> strategy;

public:
    OutputFormatter(std::shared_ptr<OutputFormattingStrategy> strategy) : strategy(strategy) {}

    void formatAndPrint(const std::string &message) const
    {
        strategy->formatAndPrint(message);
    }
};

#endif