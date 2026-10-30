#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
using namespace std;

const int MAX_BOOKS = 100;
const int MAX_MEMBERS = 100;
const int MAX_RECORDS = 500;

struct Book {
    int id;
    string title;
    string author;
    int issued;     // 1 = issued, 0 = available
    int issuedTo;   // member id, 0 if not issued
};

struct Member {
    int id;
    string name;
};

struct Record {
    int bookId;
    int memberId;
    int returned;   // 1 = returned, 0 = still with member
};

Book books[MAX_BOOKS];
Member members[MAX_MEMBERS];
Record records[MAX_RECORDS];
int bookCount = 0;
int memberCount = 0;
int recordCount = 0;

// ---------- helper functions ----------

int readInt() {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(1000, '\n');
        cout << "Invalid input. Enter a number: ";
    }
    cin.ignore(1000, '\n');
    return x;
}

string readLine(string message) {
    string s;
    while (true) {
        cout << message;
        getline(cin, s);
        if (s != "") {
            return s;
        }
        cout << "Input cannot be empty.\n";
    }
}

string toLower(string s) {
    for (int i = 0; i < s.length(); i++) {
        s[i] = tolower(s[i]);
    }
    return s;
}

int findBook(int id) {
    for (int i = 0; i < bookCount; i++) {
        if (books[i].id == id) {
            return i;
        }
    }
    return -1;
}

int findMember(int id) {
    for (int i = 0; i < memberCount; i++) {
        if (members[i].id == id) {
            return i;
        }
    }
    return -1;
}

// ---------- file handling ----------

void saveData() {
    ofstream bf("books.txt");
    for (int i = 0; i < bookCount; i++) {
        bf << books[i].id << " " << books[i].issued << " " << books[i].issuedTo << "\n";
        bf << books[i].title << "\n";
        bf << books[i].author << "\n";
    }
    bf.close();

    ofstream mf("members.txt");
    for (int i = 0; i < memberCount; i++) {
        mf << members[i].id << "\n";
        mf << members[i].name << "\n";
    }
    mf.close();

    ofstream rf("records.txt");
    for (int i = 0; i < recordCount; i++) {
        rf << records[i].bookId << " " << records[i].memberId << " " << records[i].returned << "\n";
    }
    rf.close();
}

void loadData() {
    ifstream bf("books.txt");
    if (bf) {
        while (bookCount < MAX_BOOKS && bf >> books[bookCount].id >> books[bookCount].issued >> books[bookCount].issuedTo) {
            bf.ignore();
            getline(bf, books[bookCount].title);
            getline(bf, books[bookCount].author);
            bookCount++;
        }
        bf.close();
    }

    ifstream mf("members.txt");
    if (mf) {
        while (memberCount < MAX_MEMBERS && mf >> members[memberCount].id) {
            mf.ignore();
            getline(mf, members[memberCount].name);
            memberCount++;
        }
        mf.close();
    }

    ifstream rf("records.txt");
    if (rf) {
        while (recordCount < MAX_RECORDS && rf >> records[recordCount].bookId >> records[recordCount].memberId >> records[recordCount].returned) {
            recordCount++;
        }
        rf.close();
    }
}

// ---------- main features ----------

void addBook() {
    if (bookCount >= MAX_BOOKS) {
        cout << "Library is full. Cannot add more books.\n";
        return;
    }
    Book b;
    if (bookCount == 0) {
        b.id = 1;
    } else {
        b.id = books[bookCount - 1].id + 1;
    }
    b.title = readLine("Enter book title: ");
    b.author = readLine("Enter author name: ");
    b.issued = 0;
    b.issuedTo = 0;

    books[bookCount] = b;
    bookCount++;
    saveData();
    cout << "Book added. Book ID = " << b.id << "\n";
}

void addMember() {
    if (memberCount >= MAX_MEMBERS) {
        cout << "Cannot add more members.\n";
        return;
    }
    Member m;
    if (memberCount == 0) {
        m.id = 1;
    } else {
        m.id = members[memberCount - 1].id + 1;
    }
    m.name = readLine("Enter member name: ");

    members[memberCount] = m;
    memberCount++;
    saveData();
    cout << "Member added. Member ID = " << m.id << "\n";
}

void issueBook() {
    if (recordCount >= MAX_RECORDS) {
        cout << "Record list is full. Cannot issue more books.\n";
        return;
    }
    cout << "Enter member ID: ";
    int memberId = readInt();
    int m = findMember(memberId);
    if (m == -1) {
        cout << "Member not found.\n";
        return;
    }

    cout << "Enter book ID: ";
    int bookId = readInt();
    int b = findBook(bookId);
    if (b == -1) {
        cout << "Book not found.\n";
        return;
    }
    if (books[b].issued == 1) {
        cout << "Sorry, this book is already issued.\n";
        return;
    }

    books[b].issued = 1;
    books[b].issuedTo = memberId;

    records[recordCount].bookId = bookId;
    records[recordCount].memberId = memberId;
    records[recordCount].returned = 0;
    recordCount++;

    saveData();
    cout << "\"" << books[b].title << "\" issued to " << members[m].name << ".\n";
}

void returnBook() {
    cout << "Enter book ID to return: ";
    int bookId = readInt();
    int b = findBook(bookId);
    if (b == -1) {
        cout << "Book not found.\n";
        return;
    }
    if (books[b].issued == 0) {
        cout << "This book was not issued.\n";
        return;
    }

    // mark the matching record as returned
    for (int i = 0; i < recordCount; i++) {
        if (records[i].bookId == bookId && records[i].returned == 0) {
            records[i].returned = 1;
            break;
        }
    }

    books[b].issued = 0;
    books[b].issuedTo = 0;
    saveData();
    cout << "\"" << books[b].title << "\" returned successfully.\n";
}

void searchBook() {
    cout << "Search by:\n1. Title\n2. Author\nEnter choice: ";
    int choice = readInt();
    if (choice != 1 && choice != 2) {
        cout << "Invalid choice.\n";
        return;
    }

    string keyword = toLower(readLine("Enter keyword: "));
    int found = 0;

    for (int i = 0; i < bookCount; i++) {
        string text;
        if (choice == 1) {
            text = toLower(books[i].title);
        } else {
            text = toLower(books[i].author);
        }

        if (text.find(keyword) != string::npos) {
            cout << "ID: " << books[i].id << " | " << books[i].title
                 << " | " << books[i].author << " | ";
            if (books[i].issued == 1) {
                cout << "Issued\n";
            } else {
                cout << "Available\n";
            }
            found++;
        }
    }

    if (found == 0) {
        cout << "No matching books found.\n";
    }
}

void displayBooks() {
    if (bookCount == 0) {
        cout << "No books in the library.\n";
        return;
    }
    cout << "\n--- All Books ---\n";
    for (int i = 0; i < bookCount; i++) {
        cout << "ID: " << books[i].id << " | " << books[i].title
             << " | " << books[i].author << " | ";
        if (books[i].issued == 1) {
            cout << "Issued to member " << books[i].issuedTo << "\n";
        } else {
            cout << "Available\n";
        }
    }
}

void displayMembers() {
    if (memberCount == 0) {
        cout << "No members registered.\n";
        return;
    }
    cout << "\n--- All Members ---\n";
    for (int i = 0; i < memberCount; i++) {
        cout << "ID: " << members[i].id << " | " << members[i].name << "\n";
    }
}

void displayRecords() {
    if (recordCount == 0) {
        cout << "No borrowing records yet.\n";
        return;
    }
    cout << "\n--- Borrowing Records ---\n";
    for (int i = 0; i < recordCount; i++) {
        int b = findBook(records[i].bookId);
        int m = findMember(records[i].memberId);
        cout << "Book: " << books[b].title << " | Member: " << members[m].name << " | ";
        if (records[i].returned == 1) {
            cout << "Returned\n";
        } else {
            cout << "Not returned\n";
        }
    }
}

int main() {
    loadData();
    int choice;

    do {
        cout << "\n===== LIBRARY MANAGEMENT SYSTEM =====\n";
        cout << "1. Add Book\n";
        cout << "2. Add Member\n";
        cout << "3. Issue Book\n";
        cout << "4. Return Book\n";
        cout << "5. Search Book (title/author)\n";
        cout << "6. View All Books\n";
        cout << "7. View All Members\n";
        cout << "8. View Borrowing Records\n";
        cout << "9. Exit\n";
        cout << "Enter your choice: ";
        choice = readInt();

        switch (choice) {
            case 1: addBook(); break;
            case 2: addMember(); break;
            case 3: issueBook(); break;
            case 4: returnBook(); break;
            case 5: searchBook(); break;
            case 6: displayBooks(); break;
            case 7: displayMembers(); break;
            case 8: displayRecords(); break;
            case 9: cout << "Thank you. Goodbye!\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 9);

    return 0;
}
