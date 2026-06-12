/*
 * ============================================================
 *   BANK ACCOUNT MANAGEMENT SYSTEM
 *   A University Project demonstrating:
 *   - Object-Oriented Programming (OOP)
 *   - Classes and Objects
 *   - File Handling
 *   - Input Validation
 *   - Menu-Driven Console Application
 * ============================================================
 *
 *  HOW TO COMPILE:
 *    g++ -o bank main.cpp
 *
 *  HOW TO RUN:
 *    ./bank       (Linux/macOS)
 *    bank.exe     (Windows)
 *
 *  DATA FILES CREATED:
 *    users.dat    - stores registered user credentials
 *    accounts.dat - stores all bank account data
 * ============================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <limits>

using namespace std;

// ============================================================
//  HELPER: Get current date and time as a string
// ============================================================
string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d",
             1900 + ltm->tm_year,
             1 + ltm->tm_mon,
             ltm->tm_mday,
             ltm->tm_hour,
             ltm->tm_min);
    return string(buffer);
}

// ============================================================
//  CLASS: Transaction
//  Represents a single deposit or withdrawal event.
// ============================================================
class Transaction {
private:
    string type;         // "DEPOSIT" or "WITHDRAWAL"
    double amount;       // Amount involved
    string dateTime;     // When it happened
    double balanceAfter; // Account balance after this transaction

public:
    // Constructor
    Transaction(string type, double amount, string dateTime, double balanceAfter)
        : type(type), amount(amount), dateTime(dateTime), balanceAfter(balanceAfter) {}

    // --- Getters ---
    string getType()         const { return type; }
    double getAmount()       const { return amount; }
    string getDateTime()     const { return dateTime; }
    double getBalanceAfter() const { return balanceAfter; }

    // Serialize to a single comma-separated line for file storage
    string serialize() const {
        return type + "," + to_string(amount) + "," + dateTime + "," + to_string(balanceAfter);
    }

    // Deserialize from a comma-separated line read from file
    static Transaction deserialize(const string& line) {
        stringstream ss(line);
        string token;
        vector<string> parts;
        while (getline(ss, token, ',')) {
            parts.push_back(token);
        }
        // Expected format: TYPE,AMOUNT,DATETIME,BALANCE_AFTER
        if (parts.size() < 4) return Transaction("UNKNOWN", 0.0, "UNKNOWN", 0.0);
        try {
            return Transaction(parts[0], stod(parts[1]), parts[2], stod(parts[3]));
        } catch (...) {
            return Transaction("UNKNOWN", 0.0, "UNKNOWN", 0.0);
        }
    }

    // Display this transaction as a formatted table row
    void display() const {
        cout << "  "
             << left  << setw(14) << type
             << right << setw(10) << fixed << setprecision(2) << amount
             << "    "
             << left  << setw(20) << dateTime
             << right << setw(12) << fixed << setprecision(2) << balanceAfter
             << "\n";
    }
};

// ============================================================
//  CLASS: BankAccount
//  Holds all data and operations for a single bank account.
// ============================================================
class BankAccount {
private:
    int    accountNumber;       // Unique auto-generated number
    string holderName;          // Account holder's full name
    double balance;             // Current balance
    string accountType;         // "SAVINGS" or "CHECKING"
    vector<Transaction> transactions; // Full transaction history

public:
    // Default constructor (needed by vector)
    BankAccount() : accountNumber(0), balance(0.0) {}

    // Parameterized constructor
    BankAccount(int accNum, const string& name, double initialBalance, const string& type)
        : accountNumber(accNum), holderName(name),
          balance(initialBalance), accountType(type) {}

    // --- Getters ---
    int    getAccountNumber() const { return accountNumber; }
    string getHolderName()    const { return holderName; }
    double getBalance()       const { return balance; }
    string getAccountType()   const { return accountType; }

    // Access the transaction list (used when loading from file)
    void addTransaction(const Transaction& t) {
        transactions.push_back(t);
    }

    // --- Banking Operations ---

    // Deposit: validates amount, updates balance, records transaction
    bool deposit(double amount) {
        if (amount <= 0) {
            cout << "\n  [ERROR] Deposit amount must be greater than zero.\n";
            return false;
        }
        balance += amount;
        transactions.push_back(Transaction("DEPOSIT", amount, getCurrentDateTime(), balance));
        cout << "\n  [SUCCESS] $" << fixed << setprecision(2) << amount << " deposited.\n";
        cout << "  New Balance : $" << fixed << setprecision(2) << balance << "\n";
        return true;
    }

    // Withdraw: validates amount and sufficient funds, updates balance, records transaction
    bool withdraw(double amount) {
        if (amount <= 0) {
            cout << "\n  [ERROR] Withdrawal amount must be greater than zero.\n";
            return false;
        }
        if (amount > balance) {
            cout << "\n  [ERROR] Insufficient funds.\n";
            cout << "  Available Balance : $" << fixed << setprecision(2) << balance << "\n";
            return false;
        }
        balance -= amount;
        transactions.push_back(Transaction("WITHDRAWAL", amount, getCurrentDateTime(), balance));
        cout << "\n  [SUCCESS] $" << fixed << setprecision(2) << amount << " withdrawn.\n";
        cout << "  Remaining Balance : $" << fixed << setprecision(2) << balance << "\n";
        return true;
    }

    // Print a formatted account details block
    void displayDetails() const {
        cout << "\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |           ACCOUNT DETAILS                |\n";
        cout << "  +------------------------------------------+\n";
        cout << "  |  Account Number  : " << left << setw(22) << accountNumber << "|\n";
        cout << "  |  Account Holder  : " << left << setw(22) << holderName    << "|\n";
        cout << "  |  Account Type    : " << left << setw(22) << accountType   << "|\n";
        ostringstream oss;
        oss << fixed << setprecision(2) << balance;
        cout << "  |  Current Balance : $" << left << setw(21) << oss.str()   << "|\n";
        cout << "  +------------------------------------------+\n";
    }

    // Print a formatted transaction history table
    void displayTransactionHistory() const {
        cout << "\n  === Transaction History  (Account #" << accountNumber << " — " << holderName << ") ===\n";
        cout << "  " << string(62, '-') << "\n";
        cout << "  "
             << left  << setw(14) << "TYPE"
             << right << setw(10) << "AMOUNT($)"
             << "    "
             << left  << setw(20) << "DATE & TIME"
             << right << setw(12) << "BALANCE($)"
             << "\n";
        cout << "  " << string(62, '-') << "\n";

        if (transactions.empty()) {
            cout << "  (No transactions on record)\n";
        } else {
            for (const auto& t : transactions) {
                t.display();
            }
        }
        cout << "  " << string(62, '-') << "\n";
    }

    // Serialize the account + all its transactions for file storage.
    // Format:
    //   accNum|holderName|balance|type|txCount
    //   TYPE,amount,datetime,balanceAfter   (repeated txCount times)
    string serialize() const {
        string result =
            to_string(accountNumber) + "|" +
            holderName + "|" +
            to_string(balance) + "|" +
            accountType + "|" +
            to_string(transactions.size()) + "\n";

        for (const auto& t : transactions) {
            result += t.serialize() + "\n";
        }
        return result;
    }
};

// ============================================================
//  CLASS: User
//  Handles user registration and login with file-based storage.
// ============================================================
class User {
private:
    string username;
    string USER_FILE = "users.dat";

    // Very simple obfuscation so passwords are not stored in plain text.
    // NOT intended as real security — just a demonstration of the concept.
    string obfuscate(const string& pwd) const {
        string result = pwd;
        for (char& c : result) {
            c = static_cast<char>(c ^ 0x5A); // XOR with a fixed key
        }
        return result;
    }

public:
    User() {}

    string getUsername() const { return username; }

    // Register a new user; returns false if username is already taken.
    bool registerUser(const string& uname, const string& pwd) {
        // Check for duplicate username
        ifstream inFile(USER_FILE);
        if (inFile.is_open()) {
            string line;
            while (getline(inFile, line)) {
                size_t pos = line.find(':');
                if (pos != string::npos && line.substr(0, pos) == uname) {
                    inFile.close();
                    return false; // Username already exists
                }
            }
            inFile.close();
        }

        // Append the new user record
        ofstream outFile(USER_FILE, ios::app);
        if (!outFile.is_open()) {
            cout << "  [ERROR] Could not open user file for writing.\n";
            return false;
        }
        outFile << uname << ":" << obfuscate(pwd) << "\n";
        outFile.close();
        return true;
    }

    // Attempt login; returns true on success.
    bool login(const string& uname, const string& pwd) {
        ifstream inFile(USER_FILE);
        if (!inFile.is_open()) {
            return false; // No users registered yet
        }

        string line;
        while (getline(inFile, line)) {
            size_t pos = line.find(':');
            if (pos == string::npos) continue;

            string storedUser = line.substr(0, pos);
            string storedHash = line.substr(pos + 1);

            if (storedUser == uname && storedHash == obfuscate(pwd)) {
                username = uname;
                inFile.close();
                return true;
            }
        }
        inFile.close();
        return false;
    }
};

// ============================================================
//  CLASS: BankSystem
//  Top-level controller. Manages all accounts, handles menus,
//  and delegates to BankAccount / User objects.
// ============================================================
class BankSystem {
private:
    vector<BankAccount> accounts;     // All loaded accounts
    const string ACCOUNTS_FILE = "accounts.dat";
    int  nextAccountNumber;           // Auto-increment counter
    User currentUser;                 // Currently logged-in user

    // ----------------------------------------------------------------
    //  FILE I/O
    // ----------------------------------------------------------------

    // Load all accounts from the data file into memory.
    void loadAccounts() {
        accounts.clear();
        ifstream file(ACCOUNTS_FILE);
        if (!file.is_open()) {
            nextAccountNumber = 1001;
            return;
        }

        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;

            // Parse account header line: accNum|name|balance|type|txCount
            stringstream ss(line);
            string token;
            vector<string> parts;
            while (getline(ss, token, '|')) {
                parts.push_back(token);
            }
            if (parts.size() < 5) continue;

            try {
                int    accNum  = stoi(parts[0]);
                string name    = parts[1];
                double balance = stod(parts[2]);
                string type    = parts[3];
                int    txCount = stoi(parts[4]);

                BankAccount acc(accNum, name, balance, type);

                // Read the following txCount transaction lines
                for (int i = 0; i < txCount; i++) {
                    if (getline(file, line) && !line.empty()) {
                        acc.addTransaction(Transaction::deserialize(line));
                    }
                }

                accounts.push_back(acc);
            } catch (...) {
                continue;
            }
        }
        file.close();

        // Set the next account number to max existing + 1
        nextAccountNumber = 1001;
        for (const auto& acc : accounts) {
            if (acc.getAccountNumber() >= nextAccountNumber) {
                nextAccountNumber = acc.getAccountNumber() + 1;
            }
        }
    }

    // Persist all accounts back to the data file.
    void saveAccounts() const {
        ofstream file(ACCOUNTS_FILE);
        if (!file.is_open()) {
            cout << "  [ERROR] Could not save account data.\n";
            return;
        }
        for (const auto& acc : accounts) {
            file << acc.serialize();
        }
        file.close();
    }

    // Find an account by number; returns nullptr if not found.
    BankAccount* findAccount(int accNum) {
        for (auto& acc : accounts) {
            if (acc.getAccountNumber() == accNum) {
                return &acc;
            }
        }
        return nullptr;
    }

    // ----------------------------------------------------------------
    //  UI HELPERS
    // ----------------------------------------------------------------

    void printHeader() const {
        cout << "\n";
        cout << "  ==========================================\n";
        cout << "      BANK ACCOUNT MANAGEMENT SYSTEM       \n";
        cout << "  ==========================================\n";
    }

    void printSeparator() const {
        cout << "  ------------------------------------------\n";
    }

    // Wait for the user to press Enter before continuing.
    void pause() const {
        cout << "\n  Press Enter to continue...";
        string dummy;
        getline(cin, dummy);
    }

    // Flush bad input and show an error.
    void clearInput() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // Read a validated positive double from stdin.
    double readPositiveDouble(const string& prompt) {
        double value;
        while (true) {
            cout << "  " << prompt;
            if (cin >> value && value > 0) {
                string leftover;
                getline(cin, leftover);
                return value;
            }
            cout << "  [ERROR] Please enter a valid positive number.\n";
            clearInput();
        }
    }

    // Read a validated non-negative double from stdin.
    double readNonNegativeDouble(const string& prompt) {
        double value;
        while (true) {
            cout << "  " << prompt;
            if (cin >> value && value >= 0) {
                string leftover;
                getline(cin, leftover);
                return value;
            }
            cout << "  [ERROR] Please enter a valid non-negative number.\n";
            clearInput();
        }
    }

    // Read a validated int from stdin.
    int readInt(const string& prompt) {
        int value;
        while (true) {
            cout << "  " << prompt;
            if (cin >> value) {
                string leftover;
                getline(cin, leftover);
                return value;
            }
            cout << "  [ERROR] Please enter a valid integer.\n";
            clearInput();
        }
    }

    // ----------------------------------------------------------------
    //  MENU ACTIONS
    // ----------------------------------------------------------------

    void createAccount() {
        cout << "\n  === CREATE NEW ACCOUNT ===\n";
        printSeparator();

        string name;
        cout << "  Enter account holder's name : ";
        getline(cin, name);

        if (name.empty()) {
            cout << "  [ERROR] Name cannot be empty.\n";
            return;
        }

        cout << "\n  Select account type:\n";
        cout << "    1. Savings\n";
        cout << "    2. Checking\n";
        int typeChoice = readInt("Your choice (1 or 2): ");
        string accType = (typeChoice == 2) ? "CHECKING" : "SAVINGS";

        double initialDeposit = readNonNegativeDouble("Enter initial deposit amount ($) [0 = none]: ");

        // Create and store the new account
        BankAccount newAcc(nextAccountNumber, name, initialDeposit, accType);
        int assignedNumber = nextAccountNumber;
        nextAccountNumber++;
        accounts.push_back(newAcc);
        saveAccounts();

        cout << "\n  [SUCCESS] Account created successfully!\n";
        printSeparator();
        cout << "  Account Number : " << assignedNumber << "\n";
        cout << "  Account Holder : " << name << "\n";
        cout << "  Account Type   : " << accType << "\n";
        cout << "  Opening Balance: $" << fixed << setprecision(2) << initialDeposit << "\n";
        cout << "  *** Keep your account number safe! ***\n";
    }

    void depositMoney() {
        cout << "\n  === DEPOSIT MONEY ===\n";
        printSeparator();

        int accNum = readInt("Enter Account Number: ");
        BankAccount* acc = findAccount(accNum);
        if (!acc) {
            cout << "  [ERROR] Account #" << accNum << " not found.\n";
            return;
        }

        cout << "  Account Holder : " << acc->getHolderName() << "\n";
        double amount = readPositiveDouble("Deposit amount ($): ");

        if (acc->deposit(amount)) {
            saveAccounts();
        }
    }

    void withdrawMoney() {
        cout << "\n  === WITHDRAW MONEY ===\n";
        printSeparator();

        int accNum = readInt("Enter Account Number: ");
        BankAccount* acc = findAccount(accNum);
        if (!acc) {
            cout << "  [ERROR] Account #" << accNum << " not found.\n";
            return;
        }

        cout << "  Account Holder : " << acc->getHolderName() << "\n";
        cout << "  Current Balance: $" << fixed << setprecision(2) << acc->getBalance() << "\n";
        double amount = readPositiveDouble("Withdraw amount ($): ");

        if (acc->withdraw(amount)) {
            saveAccounts();
        }
    }

    void checkBalance() {
        cout << "\n  === CHECK BALANCE ===\n";
        printSeparator();

        int accNum = readInt("Enter Account Number: ");
        BankAccount* acc = findAccount(accNum);
        if (!acc) {
            cout << "  [ERROR] Account #" << accNum << " not found.\n";
            return;
        }

        cout << "\n  Account #" << accNum << " — " << acc->getHolderName() << "\n";
        cout << "  Account Type   : " << acc->getAccountType() << "\n";
        cout << "  Current Balance: $" << fixed << setprecision(2) << acc->getBalance() << "\n";
    }

    void viewAccountDetails() {
        cout << "\n  === VIEW ACCOUNT DETAILS ===\n";
        printSeparator();

        int accNum = readInt("Enter Account Number: ");
        BankAccount* acc = findAccount(accNum);
        if (!acc) {
            cout << "  [ERROR] Account #" << accNum << " not found.\n";
            return;
        }

        acc->displayDetails();
    }

    void viewTransactionHistory() {
        cout << "\n  === TRANSACTION HISTORY ===\n";
        printSeparator();

        int accNum = readInt("Enter Account Number: ");
        BankAccount* acc = findAccount(accNum);
        if (!acc) {
            cout << "  [ERROR] Account #" << accNum << " not found.\n";
            return;
        }

        acc->displayTransactionHistory();
    }

    void deleteAccount() {
        cout << "\n  === DELETE ACCOUNT ===\n";
        printSeparator();

        int accNum = readInt("Enter Account Number: ");

        auto it = find_if(accounts.begin(), accounts.end(),
            [accNum](const BankAccount& a) {
                return a.getAccountNumber() == accNum;
            });

        if (it == accounts.end()) {
            cout << "  [ERROR] Account #" << accNum << " not found.\n";
            return;
        }

        cout << "  Account Holder : " << it->getHolderName() << "\n";
        cout << "  Balance        : $" << fixed << setprecision(2) << it->getBalance() << "\n";
        cout << "\n  WARNING: This action cannot be undone.\n";

        char confirm;
        cout << "  Are you sure? (y/n): ";
        cin >> confirm;
        clearInput();

        if (confirm == 'y' || confirm == 'Y') {
            accounts.erase(it);
            saveAccounts();
            cout << "  [SUCCESS] Account #" << accNum << " has been deleted.\n";
        } else {
            cout << "  Deletion cancelled.\n";
        }
    }

    // ----------------------------------------------------------------
    //  MAIN MENU
    // ----------------------------------------------------------------
    void showMainMenu() const {
        printHeader();
        cout << "  Logged in as : " << currentUser.getUsername() << "\n";
        printSeparator();
        cout << "  1. Create Account\n";
        cout << "  2. Deposit Money\n";
        cout << "  3. Withdraw Money\n";
        cout << "  4. Check Balance\n";
        cout << "  5. View Account Details\n";
        cout << "  6. Transaction History\n";
        cout << "  7. Delete Account\n";
        cout << "  8. Exit\n";
        printSeparator();
    }

    // ----------------------------------------------------------------
    //  LOGIN / REGISTER MENU
    // ----------------------------------------------------------------
    bool loginMenu() {
        User user;
        int choice;

        while (true) {
            printHeader();
            cout << "  1. Login\n";
            cout << "  2. Register New User\n";
            cout << "  3. Exit\n";
            printSeparator();
            choice = readInt("  Enter your choice: ");

            if (choice == 1) {
                // --- LOGIN ---
                string uname, pwd;
                cout << "\n  === LOGIN ===\n";
                cout << "  Username : ";
                cin >> uname;
                cout << "  Password : ";
                cin >> pwd;
                clearInput();

                if (user.login(uname, pwd)) {
                    currentUser = user;
                    cout << "\n  [SUCCESS] Welcome back, " << uname << "!\n";
                    pause();
                    return true;
                } else {
                    cout << "\n  [ERROR] Invalid username or password.\n";
                    pause();
                }

            } else if (choice == 2) {
                // --- REGISTER ---
                string uname, pwd, confirm;
                cout << "\n  === REGISTER ===\n";
                cout << "  Choose a username : ";
                cin >> uname;

                if (uname.empty()) {
                    cout << "  [ERROR] Username cannot be empty.\n";
                    clearInput();
                    pause();
                    continue;
                }

                cout << "  Choose a password : ";
                cin >> pwd;
                cout << "  Confirm password  : ";
                cin >> confirm;
                clearInput();

                if (pwd != confirm) {
                    cout << "\n  [ERROR] Passwords do not match.\n";
                    pause();
                    continue;
                }
                if (pwd.length() < 4) {
                    cout << "\n  [ERROR] Password must be at least 4 characters.\n";
                    pause();
                    continue;
                }

                if (user.registerUser(uname, pwd)) {
                    cout << "\n  [SUCCESS] Registration successful! Please log in.\n";
                } else {
                    cout << "\n  [ERROR] Username '" << uname << "' is already taken.\n";
                }
                pause();

            } else if (choice == 3) {
                return false; // User chose to exit

            } else {
                cout << "  [ERROR] Invalid choice. Please enter 1, 2, or 3.\n";
                pause();
            }
        }
    }

public:
    // Constructor: load saved accounts from file on startup.
    BankSystem() {
        loadAccounts();
    }

    // Entry point: show login screen, then run the main menu loop.
    void run() {
        if (!loginMenu()) {
            cout << "\n  Goodbye!\n\n";
            return;
        }

        int choice;
        do {
            showMainMenu();
            choice = readInt("  Enter your choice: ");

            switch (choice) {
                case 1: createAccount();          break;
                case 2: depositMoney();           break;
                case 3: withdrawMoney();          break;
                case 4: checkBalance();           break;
                case 5: viewAccountDetails();     break;
                case 6: viewTransactionHistory(); break;
                case 7: deleteAccount();          break;
                case 8:
                    cout << "\n  Thank you for using the Bank Account Manager.\n";
                    cout << "  Goodbye, " << currentUser.getUsername() << "!\n\n";
                    break;
                default:
                    cout << "  [ERROR] Invalid choice. Please enter a number from 1 to 8.\n";
                    break;
            }

            if (choice != 8) {
                pause();
            }

        } while (choice != 8);
    }
};

// ============================================================
//  MAIN FUNCTION
// ============================================================
int main() {
    BankSystem bank;
    bank.run();
    return 0;
}
