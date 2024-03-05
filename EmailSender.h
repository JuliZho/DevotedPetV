#ifndef EMAILSENDER_H
#define EMAILSENDER_H
#include <curl/curl.h>
#include <string>
#include "Formatter.h"

class EmailSender
{
private:
    CURL *curl;

    void initializeCurl();
    void cleanupCurl();
    static size_t readEmailBodyCallback(char *buffer, size_t size, size_t nitems, void *userdata);
    const OutputFormatter *errorFormat;

public:
    EmailSender(const OutputFormatter *format);
    ~EmailSender();

    bool sendEmail(const std::string &recipientEmail, const std::string &userName);
};

#endif // EMAILSENDER_H