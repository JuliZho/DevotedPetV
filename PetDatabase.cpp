#include "PetDatabase.h"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iomanip>
#include <sstream>
#include <stdexcept>

PetDatabase::PetDatabase(const std::string &dbName)
{
    int rc = sqlite3_open(dbName.c_str(), &db);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        throw std::runtime_error("Failed to open database");
    }

    // Create tables
    createTables();
}

std::string PetDatabase::hashPassword(const std::string &password)
{
    // Initialize the EVP context
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx)
    {
        throw std::runtime_error("Error initializing EVP context");
    }

    // Initialize the digest
    const EVP_MD *md = EVP_sha256();

    // Initialize the hash
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;

    // Initialize the password data
    const unsigned char *pwdData = reinterpret_cast<const unsigned char *>(password.c_str());
    size_t pwdLen = password.length();

    // Hash the password
    if (1 != EVP_DigestInit_ex(mdctx, md, NULL))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Error initializing digest");
    }
    if (1 != EVP_DigestUpdate(mdctx, pwdData, pwdLen))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Error updating digest");
    }
    if (1 != EVP_DigestFinal_ex(mdctx, hash, &hashLen))
    {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Error finalizing digest");
    }

    EVP_MD_CTX_free(mdctx);

    // Convert the hash to a hex string
    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; ++i)
    {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return ss.str();
}

// Helper method to execute SQL queries
int PetDatabase::executeQuery(const std::string &query)
{
    char *errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    return rc;
}

// Destructor closes the database
PetDatabase::~PetDatabase()
{
    sqlite3_close(db);
}

// Method to create tables
void PetDatabase::createTables()
{
    const std::string createUsersTableSQL = "CREATE TABLE IF NOT EXISTS users ("
                                            "user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                            "username TEXT,"
                                            "email TEXT UNIQUE,"
                                            "password_hash TEXT"
                                            ");";

    const std::string createPetsTableSQL = "CREATE TABLE IF NOT EXISTS pets ("
                                           "pet_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                           "type TEXT,"
                                           "name TEXT,"
                                           "breed TEXT,"
                                           "age INTEGER,"
                                           "hunger_level INTEGER,"
                                           "happiness_level INTEGER,"
                                           "user_id INTEGER,"
                                           "FOREIGN KEY(user_id) REFERENCES users(user_id)"
                                           ");";

    const std::string createInteractionsTableSQL = "CREATE TABLE IF NOT EXISTS interactions ("
                                                   "interaction_id INTEGER PRIMARY KEY AUTOINCREMENT,"
                                                   "interaction_type TEXT,"
                                                   "interaction_time TIMESTAMP,"
                                                   "user_id INTEGER,"
                                                   "pet_id INTEGER,"
                                                   "FOREIGN KEY(user_id) REFERENCES users(user_id),"
                                                   "FOREIGN KEY(pet_id) REFERENCES pets(pet_id)"
                                                   ");";

    executeQuery(createUsersTableSQL);
    executeQuery(createPetsTableSQL);
    executeQuery(createInteractionsTableSQL);
}

bool PetDatabase::insertUser(const std::string &username, const std::string &email, const std::string &password)
{
    std::string hashedPassword = hashPassword(password); // Hash the password
    const std::string insertUserSQL = "INSERT INTO users (username, email, password_hash) VALUES (?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, insertUserSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashedPassword.c_str(), -1, SQLITE_STATIC); // Store hashed password

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

void PetDatabase::displayUsersWithColor(const std::string &colorCode)
{
    const std::string selectUsersSQL = "SELECT * FROM users;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, selectUsersSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    std::cout << colorCode;

    std::cout << "User Info:" << std::endl;
    std::cout << "----------" << std::endl;
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        int userId = sqlite3_column_int(stmt, 0);
        const unsigned char *userEmail = sqlite3_column_text(stmt, 1);
        const unsigned char *userPasswordHash = sqlite3_column_text(stmt, 2);

        std::cout << "User ID: " << userId << std::endl;
        std::cout << "Email: " << userEmail << std::endl;
        std::cout << "Password hash: " << userPasswordHash << std::endl;
        std::cout << "-----------" << std::endl;
    }

    std::cout << "\x1b[0m";

    sqlite3_finalize(stmt);
}

bool PetDatabase::doesUserExist(const std::string &email)
{
    const std::string checkUserSQL = "SELECT email FROM users WHERE email = '" + email + "';";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, checkUserSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW)
    {

        sqlite3_finalize(stmt);
        return true;
    }
    else if (rc == SQLITE_DONE)
    {

        sqlite3_finalize(stmt);
        return false;
    }
    else
    {

        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }
}

bool PetDatabase::getPet(int &petId,
                         std::string &petType,
                         std::string &petName,
                         std::string &petBreed,
                         int &petAge,
                         int &petHunger,
                         int &petHappiness,
                         int user_id,
                         const OutputFormatter &outputFormatter,
                         const OutputFormatter &errorFormatter,
                         const OutputFormatter &thankyouFormatter)
{
    const std::string selectPetSQL = "SELECT pet_id, type, name, breed, age, hunger_level, happiness_level FROM pets JOIN users ON pets.user_id = users.user_id WHERE users.user_id = ?";

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, selectPetSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, user_id);

    if (sqlite3_step(stmt) == SQLITE_ROW)
    {
        petId = sqlite3_column_int(stmt, 0);
        petType = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
        petName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));
        petBreed = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 3));
        petAge = sqlite3_column_int(stmt, 4);
        petHunger = sqlite3_column_int(stmt, 5);
        petHappiness = sqlite3_column_int(stmt, 6);

        sqlite3_finalize(stmt);

        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool PetDatabase::verifyUser(const std::string &email, const std::string &password, int &user_id, std::string &userName)
{
    std::string hashedPassword = hashPassword(password); // Hash the provided password
    const std::string checkUserSQL = "SELECT password_hash, user_id, username FROM users WHERE email = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, checkUserSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    const unsigned char *storedHash = sqlite3_column_text(stmt, 0);
    std::string storedPassword(reinterpret_cast<const char *>(storedHash));

    userName = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 2));

    user_id = sqlite3_column_int(stmt, 1);

    sqlite3_finalize(stmt);

    // Compare the hashed passwords
    return hashedPassword == storedPassword;
}

bool PetDatabase::addPet(const std::string &name, const std::string &type, const std::string &breed, int age, int hunger_level, int happiness_level, int user_id, const OutputFormatter &outputFormatter, const OutputFormatter &errorFormatter, const OutputFormatter &thankyouFormatter)
{
    // Check if a pet already exists for the given user
    if (doesPetExistForUser(user_id))
    {
        std::cerr << "Error: User already has a pet." << std::endl;
        return false;
    }

    const std::string addPetSQL = "INSERT INTO pets (name, type, breed, age, hunger_level, happiness_level, user_id) VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, addPetSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, breed.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, age);
    sqlite3_bind_int(stmt, 5, hunger_level);
    sqlite3_bind_int(stmt, 6, happiness_level);
    sqlite3_bind_int(stmt, 7, user_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool PetDatabase::updatePet(const std::string &name, const std::string &type, const std::string &breed,
                            int age, int hunger_level, int happiness_level, int pet_id)
{

    const std::string addPetSQL = "UPDATE pets set name = ?, type = ?, breed = ?, age = ?, hunger_level = ?, happiness_level = ? where pet_id = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, addPetSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    // Bind parameters to the SQL statement
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, type.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, breed.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, age);
    sqlite3_bind_int(stmt, 5, hunger_level);
    sqlite3_bind_int(stmt, 6, happiness_level);
    sqlite3_bind_int(stmt, 7, pet_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);
    return true;
}

bool PetDatabase::doesPetExistForUser(int user_id)
{

    const std::string checkPetSQL = "SELECT COUNT(*) FROM pets WHERE user_id = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, checkPetSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    sqlite3_bind_int(stmt, 1, user_id);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    int count = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    return count > 0;
}

int PetDatabase::hungerLevel(const std::string &email)
{

    const std::string getHungerLevelSQL = "SELECT hunger_level FROM pets JOIN users ON pets.user_id = users.user_id WHERE users.email = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, getHungerLevelSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    int hungerLevel = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    return hungerLevel;
}

int PetDatabase::happinessLevel(const std::string &email)
{

    const std::string getHappinessLevelSQL = "SELECT happiness_level FROM pets JOIN users ON pets.user_id = users.user_id WHERE users.email = ?;";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db, getHappinessLevelSQL.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);

    // Execute the SQL statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW)
    {
        std::cerr << "Error executing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    int happinessLevel = sqlite3_column_int(stmt, 0);

    sqlite3_finalize(stmt);

    return happinessLevel;
}

PetDatabase::operator bool() const
{
    return isOpen();
}

bool PetDatabase::isOpen() const
{
    return db != nullptr;
}
