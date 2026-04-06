#include <iostream>
#include <string>
#include <cstdlib>
#include <limits>
#include <ctime>
#include <fstream>
#include <vector>
#include <sstream>
#include <conio.h>
#include <algorithm>

using namespace std;

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

const int MAX_USERS = 100;

// ================= USER STRUCT =================
struct User {
    string name;
    long long cellNumber;
    int password;
    double balance;
};

struct Transaction {
    long long userCell;
    string type;
    string details;
    double amount;
    string timestamp;
};

User users[MAX_USERS];
int userCount = 0;
vector<Transaction> transactions;

// ========== DECLARATIONS ==========
void showMenu();
void login();
void signup();
void displayUserInfo(int);
bool readCellInput(const string&, long long&, bool allowBack = true);
bool readAmountInput(const string&, double&, bool allowBack = true);
int getPasswordInput();
void showInstructions();
void returnToMainMenu();
void clearScreen();
string getCurrentTimestamp();
int generateCaptcha();

void saveUserData();
void loadUserData();
void saveTransactionData();
void loadTransactionData();
void displayTransactionHistory(int);

// ================= MAIN =================
int main() {
    srand(time(0));
    loadUserData();
    loadTransactionData();
    
    while (true) {
        showMenu();
    }
    return 0;
}

// ================= SCREEN =================
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void showHeader() {
    cout << "\n\t\t\t--------------------------------------\n";
    cout << "\t\t\t\tSMART CASH APP\n";
    cout << "\t\t\t--------------------------------------\n";
}

void returnToMainMenu() {
    cout << "\n\t\t\tPress any key to continue...";
    _getch();
    clearScreen();
}

// ================= PASSWORD INPUT =================
int getPasswordInput() {
    string passStr = "";
    char ch;
    
    cout << "\t\t\tEnter Password: ";
    
    while (true) {
        ch = _getch();
        if (ch == 13) break;
        if (ch == 8) {
            if (!passStr.empty()) {
                passStr.pop_back();
                cout << "\b \b";
            }
        }
        else if (isdigit(ch)) {
            passStr += ch;
            cout << "*";
        }
    }
    
    cout << endl;
    
    if (passStr.empty()) return -1;
    return stoi(passStr);
}

// ================= MAIN MENU =================
void showMenu() {
    clearScreen();
    showHeader();
    
    int choice;
    
    cout << "\t\t\t1. Login\n";
    cout << "\t\t\t2. Signup\n";
    cout << "\t\t\t3. Instructions\n";
    cout << "\t\t\t4. Exit\n";
    cout << "\n\t\t\tEnter Choice: ";
    cin >> choice;
    
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << RED << "\t\t\tInvalid choice!" << RESET << endl;
        returnToMainMenu();
        return;
    }
    
    switch (choice) {
        case 1: login(); break;
        case 2: signup(); break;
        case 3: showInstructions(); break;
        case 4:
            saveUserData();
            saveTransactionData();
            cout << GREEN << "\t\t\tProgram Closed Successfully!" << RESET << endl;
            exit(0);
        default:
            cout << RED << "\t\t\tInvalid Option!" << RESET << endl;
            returnToMainMenu();
    }
}

// ================= INSTRUCTIONS =================
void showInstructions() {
    clearScreen();
    showHeader();
    
    cout << "\tInstructions:\n";
    cout << "\t1. Signup before login.\n";
    cout << "\t2. Cell number must be 11 digits.\n";
    cout << "\t3. Password must be 4+ digits.\n";
    cout << "\t4. Press 1 at any time to go back.\n";
    
    returnToMainMenu();
}

// ================= LOGIN =================
void login() {
    if (userCount == 0) {
        cout << RED << "\t\t\tNo users found. Signup first!" << RESET << endl;
        returnToMainMenu();
        return;
    }
    
    long long cell;
    int pass;
    
    cout << "\n\t\t\t**** LOGIN PAGE ****\n";
    
    if (!readCellInput("\t\t\tEnter Cell No (or 1 to go back): ", cell)) return;
    
    pass = getPasswordInput();
    if (pass == -1) {
        cout << RED << "\t\t\tInvalid password!" << RESET << endl;
        returnToMainMenu();
        return;
    }
    
    for (int i = 0; i < userCount; i++) {
        if (users[i].cellNumber == cell && users[i].password == pass) {
            cout << GREEN << "\t\t\tLogin Successful! Welcome " << users[i].name << RESET << endl;
            returnToMainMenu();
            displayUserInfo(i);
            return;
        }
    }
    
    cout << RED << "\t\t\tInvalid credentials!" << RESET << endl;
    returnToMainMenu();
}

// ================= SIGNUP =================
void signup() {
    if (userCount >= MAX_USERS) {
        cout << RED << "\t\t\tUser limit reached!" << RESET << endl;
        returnToMainMenu();
        return;
    }
    
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    string name;
    long long cell;
    int pass;
    
    cout << "\n\t\t\t**** SIGNUP PAGE ****\n";
    
    while (true) {
        cout << "\t\t\tEnter Name (or 1 to go back): ";
        getline(cin, name);
        if (name == "1") return;
        if (!name.empty()) break;
        cout << RED << "\t\t\tName cannot be empty!" << RESET << endl;
    }
    
    while (true) {
        if (!readCellInput("\t\t\tEnter Cell (11 digits) or 1 to go back: ", cell)) return;
        
        bool exists = false;
        for (int i = 0; i < userCount; i++)
            if (users[i].cellNumber == cell) exists = true;
        
        if (exists) {
            cout << RED << "\t\t\tThis number already exists!" << RESET << endl;
            continue;
        }
        break;
    }
    
    int cap = generateCaptcha(), verify;
    while (true) {
        cout << "\t\t\tEnter Captcha " << cap << ": ";
        cin >> verify;
        if (verify == 1) return;
        if (verify != cap) {
            cout << RED << "\t\t\tIncorrect captcha!" << RESET << endl;
            continue;
        }
        break;
    }
    
    while (true) {
        pass = getPasswordInput();
        if (pass == -1 || to_string(pass).length() < 4) {
            cout << RED << "\t\t\tPassword must be 4+ digits!" << RESET << endl;
            continue;
        }
        break;
    }
    
    users[userCount++] = {name, cell, pass, 0.0};
    saveUserData();
    
    cout << GREEN << "\t\t\tSignup Successful!" << RESET << endl;
    returnToMainMenu();
}

// ================= DASHBOARD =================
void displayUserInfo(int index) {
    int choice;
    
    while (true) {
        clearScreen();
        showHeader();
        
        cout << "\t\t\tWelcome " << users[index].name
        << " | Balance: " << users[index].balance << "\n";
        
        cout << "\t\t\t1. Send Money\n";
        cout << "\t\t\t2. Check Balance\n";
        cout << "\t\t\t3. Deposit\n";
        cout << "\t\t\t4. Transaction History\n";
        cout << "\t\t\t5. Logout\n";
        cout << "\t\t\tEnter Choice: ";
        cin >> choice;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << RED << "\t\t\tInvalid choice!" << RESET << endl;
            returnToMainMenu();
            continue;
        }
        
        // SEND MONEY
        if (choice == 1) {
            while (true) {
                long long recCell;
                
                if (!readCellInput("\t\t\tEnter Recipient Cell (or 1 to go back): ", recCell))
                    break;

                // ? Self-transfer prevention FIX
                if (recCell == users[index].cellNumber) {
                    cout << RED << "\t\t\tYou cannot send money to your own number!" << RESET << endl;
                    continue;
                }
                
                int recIndex = -1;
                for (int i = 0; i < userCount; i++)
                    if (users[i].cellNumber == recCell) recIndex = i;
                
                if (recIndex == -1) {
                    cout << RED << "\t\t\tUser not found!" << RESET << endl;
                    continue;
                }
                
                double amount;
                if (!readAmountInput("\t\t\tEnter Amount (or 1 to go back): ", amount)) break;
                
                if (amount > users[index].balance) {
                    cout << RED << "\t\t\tInsufficient balance!" << RESET << endl;
                    continue;
                }
                
                int confirm;
                cout << "\t\t\tDo you want to send money to "<< users[recIndex].name<< "?  Yes(1)  No(0): ";
                cin >> confirm;
                
                if (confirm != 1) {
                    cout << RED << "\t\t\tTransaction Cancelled!" << RESET << endl;
                    break;
                }
                
                users[index].balance -= amount;
                users[recIndex].balance += amount;
                
                string ts = getCurrentTimestamp();
                
                transactions.push_back({users[index].cellNumber, "Sent", "to " + users[recIndex].name, amount, ts});
                transactions.push_back({users[recIndex].cellNumber, "Received", "from " + users[index].name, amount, ts});
                
                saveUserData();
                saveTransactionData();
                
                cout << GREEN << "\t\t\tMoney Sent Successfully!" << RESET << endl;
                break;
            }
            returnToMainMenu();
        }
        
        // CHECK BALANCE
        else if (choice == 2) {
            cout << "\t\t\tYour Balance: " << users[index].balance << endl;
            returnToMainMenu();
        }
        
        // DEPOSIT
        else if (choice == 3) {
            double dep;
            if (readAmountInput("\t\t\tEnter Amount to Deposit: ", dep)) {
                users[index].balance += dep;
                transactions.push_back({users[index].cellNumber, "Deposit", "N/A", dep, getCurrentTimestamp()});
                saveUserData();
                saveTransactionData();
                cout << GREEN << "\t\t\tDeposit Successful!" << RESET << endl;
            }
            returnToMainMenu();
        }
        
        // HISTORY
        else if (choice == 4) {
            displayTransactionHistory(index);
            returnToMainMenu();
        }
        
        // LOGOUT
        else if (choice == 5) {
            cout << GREEN << "\t\t\tLogged out!" << RESET << endl;
            returnToMainMenu();
            return;
        }
        
        else {
            cout << RED << "\t\t\tInvalid Option!" << RESET << endl;
            returnToMainMenu();
        }
    }
}

// ================= TRANSACTION HISTORY (FIXED) =================
void displayTransactionHistory(int index) {
    cout << "\n\t\t\t----- TRANSACTION HISTORY -----\n";

    long long currentUser = users[index].cellNumber;
    bool found = false;

    for (int i = transactions.size() - 1; i >= 0; i--) {
        if (transactions[i].userCell == currentUser) {

            cout << "\t" 
                 << transactions[i].timestamp << " | "
                 << transactions[i].type << " "
                 << transactions[i].amount << " "
                 << transactions[i].details 
                 << endl;

            found = true;
        }
    }

    if (!found) {
        cout << "\t\tNo transaction history found.\n";
    }
}

// ================= UTILITIES =================
string getCurrentTimestamp() {
    time_t now = time(0);
    string t = ctime(&now);
    t.pop_back();
    return t;
}

int generateCaptcha() {
    return rand() % 9000 + 1000;
}

// ================= FILE HANDLING =================
void saveUserData() {
    ofstream out("users.txt");
    for (int i = 0; i < userCount; i++) {
        out << users[i].name << "|"
        << users[i].cellNumber << "|"
        << users[i].password << "|"
        << users[i].balance << "\n";
    }
}

void loadUserData() {
    ifstream in("users.txt");
    string line;
    
    while (getline(in, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        User u;
        string temp;
        
        getline(ss, u.name, '|');
        getline(ss, temp, '|'); u.cellNumber = stoll(temp);
        getline(ss, temp, '|'); u.password = stoi(temp);
        getline(ss, temp, '|'); u.balance = stod(temp);
        
        users[userCount++] = u;
    }
}

void saveTransactionData() {
    ofstream out("transactions.txt");
    for (auto &t : transactions)
        out << t.userCell << "|" << t.type << "|"
        << t.details << "|" << t.amount << "|" << t.timestamp << "\n";
}

// ================= FIXED TRANSACTION LOADING =================
void loadTransactionData() {
    ifstream in("transactions.txt");
    string line;

    while (getline(in, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        Transaction t;
        string temp;

        getline(ss, temp, '|');
        if (temp.empty()) continue;
        t.userCell = stoll(temp);

        getline(ss, t.type, '|');
        getline(ss, t.details, '|');

        getline(ss, temp, '|');
        t.amount = stod(temp);

        getline(ss, t.timestamp);

        while (!t.timestamp.empty() && 
              (t.timestamp.back() == '\r' || t.timestamp.back() == ' '))
            t.timestamp.pop_back();

        transactions.push_back(t);
    }
}

// ================= PHONE VALIDATION =================
bool readCellInput(const string &prompt, long long &outCell, bool allowBack) {
    while (true) {
        cout << prompt;
        string s;
        cin >> s;
        
        if (allowBack && s == "1") return false;
        
        if (s.length() != 11 || !all_of(s.begin(), s.end(), ::isdigit)) {
            cout << RED << "\t\t\tCell number must be 11 digits!" << RESET << endl;
            continue;
        }
        
        outCell = stoll(s);
        return true;
    }
}

// ================= AMOUNT INPUT =================
bool readAmountInput(const string &prompt, double &outAmount, bool allowBack) {
    while (true) {
        cout << prompt;
        string s;
        cin >> s;
        
        if (allowBack && s == "1") return false;
        
        bool valid = true;
        int dot = 0;
        
        for (char c : s) {
            if (c == '.') dot++;
            else if (!isdigit(c)) valid = false;
            if (dot > 1) valid = false;
        }
        
        if (!valid) {
            cout << RED << "\t\t\tInvalid amount!" << RESET << endl;
            continue;
        }
        
        outAmount = stod(s);
        return true;
    }
}
