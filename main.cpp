#include <iostream>
#include <string>
#include <queue>
#include <vector>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <sstream>
using namespace std;

enum Status { Available, Borrowed, Reserved };

struct Book {
    int id;
    string title;
    string author;
    Status status;
    int edition;
    int year;
    Book* next;
};

struct Member {
    int id;
    string name;
    vector<Book*> borrowedBooks;
    queue<Book*> reservations;
    Member* next;
};

struct Transaction {
    int memberID;
    int bookID;
    string borrowDate;
    string dueDate;
    string returnDate;
    Transaction* next;
};


Book* bookHead = nullptr;
Member* memberHead = nullptr;
Transaction* transactionHead = nullptr;


string getCurrentDate() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}

string calculateDueDate(int days) {
    time_t now = time(0) + (days * 24 * 60 * 60);
    tm* ltm = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", ltm);
    return string(buffer);
}


void addBook() {
    Book* newBook = new Book;
    cout << "Enter Book ID: ";
    cin >> newBook->id;
    cin.ignore();
    cout << "Enter Title: ";
    getline(cin, newBook->title);
    cout << "Enter Author: ";
    getline(cin, newBook->author);
    cout << "Enter Edition: ";
    cin >> newBook->edition;
    cout << "Enter Year: ";
    cin >> newBook->year;
    newBook->status = Available;
    newBook->next = bookHead;
    bookHead = newBook;
    cout << "Book added successfully!\n";
}

void displayBooks() {
    if (!bookHead) {
        cout << "No books available.\n";
        return;
    }
    Book* current = bookHead;
    while (current) {
        cout << "ID: " << current->id
             << ", Title: " << current->title
             << ", Author: " << current->author
             << ", Edition: " << current->edition
             << ", Year: " << current->year
             << ", Status: " << (current->status == Available ? "Available" : (current->status == Borrowed ? "Borrowed" : "Reserved"))
             << endl;
        current = current->next;
    }
}

void removeBook() {
    int id;
    cout << "Enter Book ID to remove: ";
    cin >> id;
    Book* current = bookHead;
    Book* prev = nullptr;

    while (current && current->id != id) {
        prev = current;
        current = current->next;
    }

    if (!current) {
        cout << "Book not found.\n";
        return;
    }

    if (prev) {
        prev->next = current->next;
    } else {
        bookHead = current->next;
    }

    delete current;
    cout << "Book removed successfully!\n";
}


void addMember() {
    Member* newMember = new Member;
    cout << "Enter Member ID: ";
    cin >> newMember->id;
    cin.ignore();
    cout << "Enter Member Name: ";
    getline(cin, newMember->name);
    newMember->next = memberHead;
    memberHead = newMember;
    cout << "Member added successfully!\n";
}

void displayMembers() {
    if (!memberHead) {
        cout << "No members available.\n";
        return;
    }

    Member* current = memberHead;
    while (current) {
        cout << "ID: " << current->id << ", Name: " << current->name << endl;
        current = current->next;
    }
}


void borrowBook(int memberID, int bookID) {
    Book* book = bookHead;
    while (book && book->id != bookID) {
        book = book->next;
    }

    if (!book) {
        cout << "Book not found.\n";
        return;
    }

    if (book->status != Available) {
        cout << "Book is not available.\n";
        return;
    }

    Member* member = memberHead;
    while (member && member->id != memberID) {
        member = member->next;
    }

    if (!member) {
        cout << "Member not found.\n";
        return;
    }
    
    member->borrowedBooks.push_back(book);
    book->status = Borrowed;

    Transaction* transaction = new Transaction;
    transaction->memberID = memberID;
    transaction->bookID = bookID;
    transaction->borrowDate = getCurrentDate();
    transaction->dueDate = calculateDueDate(14); 
    transaction->returnDate = "";
    transaction->next = transactionHead;
    transactionHead = transaction;

    cout << "Book borrowed successfully! Due date: " << transaction->dueDate << "\n";
}


void returnBook(int memberID, int bookID) {
    Transaction* transaction = transactionHead;
    while (transaction && (transaction->memberID != memberID || transaction->bookID != bookID)) {
        transaction = transaction->next;
    }

    if (!transaction) {
        cout << "Transaction not found.\n";
        return;
    }

    if (!transaction->returnDate.empty()) {
        cout << "Book already returned.\n";
        return;
    }

    transaction->returnDate = getCurrentDate();

    Book* book = bookHead;
    while (book && book->id != bookID) {
        book = book->next;
    }

    if (book) {
        book->status = Available;
    }

    cout << "Book returned successfully on " << transaction->returnDate << "\n";
}



void saveToFile() {
    ofstream bookFile("books.txt");
    Book* current = bookHead;
    while (current) {
        bookFile << current->id << ","
                 << current->title << ","
                 << current->author << ","
                 << current->status << ","
                 << current->edition << ","
                 << current->year << "\n";
        current = current->next;
    }
    bookFile.close();
    cout << "Data saved to books.txt\n";
}



void loadFromFile() {
    ifstream bookFile("books.txt");
    if (!bookFile) {
        cout << "File not found. Starting with empty data.\n";
        return;
    }

    string line;
    while (getline(bookFile, line)) {
        Book* newBook = new Book;
        stringstream ss(line);
        string token;

        getline(ss, token, ',');
        newBook->id = stoi(token);

        getline(ss, token, ',');
        newBook->title = token;

        getline(ss, token, ',');
        newBook->author = token;

        getline(ss, token, ',');
        newBook->status = static_cast<Status>(stoi(token));

        getline(ss, token, ',');
        newBook->edition = stoi(token);

        getline(ss, token, ',');
        newBook->year = stoi(token);

        newBook->next = bookHead;
        bookHead = newBook;
    }
    bookFile.close();
    cout << "Data loaded from books.txt\n";
}

void sortBooksByYear() {
    vector<Book*> bookList;
    Book* current = bookHead;
    while (current) {
        bookList.push_back(current);
        current = current->next;
    }

    sort(bookList.begin(), bookList.end(), [](Book* a, Book* b) {
        return a->year < b->year;
    });

    bookHead = nullptr;
    for (auto& book : bookList) {
        book->next = bookHead;
        bookHead = book;
    }
    cout << "Books sorted by year.\n";
}


void searchBooksByTitle() {
    string title;
    cout << "Enter book title to search: ";
    cin.ignore();
    getline(cin, title);

    Book* current = bookHead;
    bool found = false;
    while (current) {
        if (current->title.find(title) != string::npos) {
            cout << "ID: " << current->id
                 << ", Title: " << current->title
                 << ", Author: " << current->author
                 << ", Edition: " << current->edition
                 << ", Year: " << current->year
                 << ", Status: " << (current->status == Available ? "Available" : (current->status == Borrowed ? "Borrowed" : "Reserved"))
                 << endl;
            found = true;
        }
        current = current->next;
    }

    if (!found) {
        cout << "No books found with the title \"" << title << "\".\n";
    }
}


void mainMenu() {
    int choice;
    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n2. Display Books\n3. Remove Book\n4. Add Member\n5. Display Members\n";
        cout << "6. Borrow Book\n7. Return Book\n8. Save Data\n9. Load Data\n10. Sort Books\n11. Search Books\n12. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            addBook();
            break;
        case 2:
            displayBooks();
            break;
        case 3:
            removeBook();
            break;
        case 4:
            addMember();
            break;
        case 5:
            displayMembers();
            break;
        case 6: {
            int memberID, bookID;
            cout << "Enter Member ID: ";
            cin >> memberID;
            cout << "Enter Book ID: ";
            cin >> bookID;
            borrowBook(memberID, bookID);
            break;
        }
        case 7: {
            int memberID, bookID;
            cout << "Enter Member ID: ";
            cin >> memberID;
            cout << "Enter Book ID: ";
            cin >> bookID;
            returnBook(memberID, bookID);
            break;
        }
        case 8:
            saveToFile();
            break;
        case 9:
            loadFromFile();
            break;
        case 10:
            sortBooksByYear();
            break;
        case 11:
            searchBooksByTitle();
            break;
        case 12:
            cout << "Exiting...\n";
            break;
        default:
            cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 12);
}

int main() {
    mainMenu();
    return 0;
}
