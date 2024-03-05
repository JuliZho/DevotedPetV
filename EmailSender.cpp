#include "EmailSender.h"
#include <curl/curl.h>
#include <iostream>

EmailSender::EmailSender(const OutputFormatter *format) : errorFormat(format)
{
    initializeCurl();
}

EmailSender::~EmailSender()
{
    cleanupCurl();
}

void EmailSender::initializeCurl()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void EmailSender::cleanupCurl()
{
    curl_global_cleanup();
}

size_t EmailSender::readEmailBodyCallback(char *buffer, size_t size, size_t nitems, void *userdata)
{
    std::string *emailBody = reinterpret_cast<std::string *>(userdata);
    size_t totalSize = size * nitems;
    size_t dataSize = std::min(emailBody->size(), totalSize);
    if (dataSize > 0)
    {
        std::copy(emailBody->begin(), emailBody->begin() + dataSize, buffer);
        emailBody->erase(0, dataSize);
    }
    return dataSize;
}

bool EmailSender::sendEmail(const std::string &recipientEmail, const std::string &userName)
{
    std::string subject = "Hi from Devoted Pet!";
    std::string body = "Subject: " + subject + "\r\n";
    body += "Content-Type: text/plain; charset=\"UTF-8\"\r\n";
    body += "MIME-Version: 1.0\r\n";
    body += "\r\n"; // End of headers
    body += "Dear " + userName + ", thank you for registering.";

    CURL *curl;
    CURLcode res;
    struct curl_slist *recipients = NULL;

    curl_global_init(CURL_GLOBAL_ALL);

    curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_USERNAME, "ydevotedpets@gmail.com");
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "zxnq eqby cbgt qyal");
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, "ydevotedpets@gmail.com");
        recipients = curl_slist_append(recipients, recipientEmail.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, readEmailBodyCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &body);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L); // Enable uploading

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            errorFormat->formatAndPrint("Error occurred. The email was not sent.\n");
            curl_slist_free_all(recipients);
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return false;
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
    return true;
}