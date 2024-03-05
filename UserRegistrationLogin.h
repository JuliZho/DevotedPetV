#ifndef USER_REGISTRATION_H
#define USER_REGISTRATION_H
#include "PetDatabase.h"
#include "Formatter.h"

class UserRegistrationLogin
{
private:
    PetDatabase *petDatabase;
    std::string userEmail;
    std::string userName;
    std::string password;
    std::string passwordRepeat;

public:
    UserRegistrationLogin(PetDatabase *database);
    bool registerUser(const OutputFormatter &outputFormatter, const OutputFormatter &erroFormatter, const OutputFormatter &thankyouFormatter);
    bool loginUser(const OutputFormatter &outputFormatter, const OutputFormatter &errorFormatter, const OutputFormatter &thankyouFormatter, int &user_id, std::string &userName);
};

#endif // USER_REGISTRATION_H
