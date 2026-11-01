// Case Study: Library Management System (OOP prototype in C++)
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <stdexcept>
using namespace std;

// ---------- Abstract class for everything the library lends ----------
class Item {
protected:
    int id;
    string title;
    bool issued;
public:
    Item(int i, string t) : id(i), title(t), issued(false) {}
    virtual ~Item() {}
    int    getId() const    { return id; }
    string getTitle() const { return title; }
    bool   isIssued() const { return issued; }
    void   setIssued(bool s) { issued = s; }
    virtual string type() const = 0;
    virtual int loanDays() const = 0;
    virtual double finePerDay() const = 0;
};

class Book : public Item {
public:
    Book(int i, string t) : Item(i, t) {}
    string type() const override { return "Book"; }
    int loanDays() const override { return 14; }
    double finePerDay() const override { return 2.0; }
};

class Magazine : public Item {
public:
    Magazine(int i, string t) : Item(i, t) {}
    string type() const override { return "Magazine"; }
    int loanDays() const override { return 7; }
    double finePerDay() const override { return 1.0; }
};

// ---------- Member ----------
class Member {
    int id;
    string name;
    int borrowed;
    static const int LIMIT = 2;
public:
    Member(int i, string n) : id(i), name(n), borrowed(0) {}
    int    getId() const   { return id; }
    string getName() const { return name; }
    bool canBorrow() const { return borrowed < LIMIT; }
    void borrow()  { borrowed++; }
    void giveBack() { borrowed--; }
    int  count() const { return borrowed; }
};

// ---------- Library (controller) ----------
class Library {
    vector<Item*> items;
    vector<Member*> members;
    Item* findItem(int id) {
        for (Item* x : items) if (x->getId() == id) return x;
        throw runtime_error("Item not found");
    }
    Member* findMember(int id) {
        for (Member* m : members) if (m->getId() == id) return m;
        throw runtime_error("Member not found");
    }
public:
    ~Library() {
        for (Item* x : items) delete x;
        for (Member* m : members) delete m;
    }
    void addItem(Item* x)     { items.push_back(x); }
    void addMember(Member* m) { members.push_back(m); }

    void issue(int memberId, int itemId) {
        Member* m = findMember(memberId);
        Item* x = findItem(itemId);
        if (x->isIssued())   throw runtime_error("'" + x->getTitle() + "' is already issued");
        if (!m->canBorrow()) throw runtime_error(m->getName() + " has reached the borrowing limit");
        x->setIssued(true);
        m->borrow();
        cout << "Issued  : " << x->getTitle() << " (" << x->type() << ") to "
             << m->getName() << " for " << x->loanDays() << " days\n";
    }

    void giveBackItem(int memberId, int itemId, int daysKept) {
        Member* m = findMember(memberId);
        Item* x = findItem(itemId);
        if (!x->isIssued()) throw runtime_error("'" + x->getTitle() + "' was not issued");
        int late = daysKept > x->loanDays() ? daysKept - x->loanDays() : 0;
        double fine = late * x->finePerDay();
        x->setIssued(false);
        m->giveBack();
        cout << fixed << setprecision(2);
        cout << "Returned: " << x->getTitle() << " by " << m->getName()
             << " | late days: " << late << " | fine: Rs." << fine << "\n";
    }

    void catalog() const {
        cout << "\n" << left << setw(6) << "ID" << setw(10) << "Type"
             << setw(28) << "Title" << "Status\n" << string(54, '-') << "\n";
        for (const Item* x : items)
            cout << left << setw(6) << x->getId() << setw(10) << x->type()
                 << setw(28) << x->getTitle() << (x->isIssued() ? "Issued" : "Available") << "\n";
    }
};

int main() {
    Library lib;
    lib.addItem(new Book(101, "Let Us C++"));
    lib.addItem(new Book(102, "Data Structures"));
    lib.addItem(new Book(103, "Operating Systems"));
    lib.addItem(new Magazine(201, "Digit Monthly"));
    lib.addMember(new Member(1, "Randeep Singh"));
    lib.addMember(new Member(2, "Aman Verma"));

    cout << "===== LIBRARY MANAGEMENT SYSTEM - DEMO =====\n";
    lib.catalog();
    cout << "\n--- Issuing items ---\n";
    const int steps[][2] = { {1,101}, {1,201}, {1,102}, {2,101}, {2,103} };
    for (auto& s : steps) {
        try { lib.issue(s[0], s[1]); }
        catch (const exception& e) { cout << "Error   : " << e.what() << "\n"; }
    }
    cout << "\n--- Returning items ---\n";
    try {
        lib.giveBackItem(1, 101, 20);   // 6 days late
        lib.giveBackItem(1, 201, 5);    // on time
        lib.giveBackItem(1, 102, 3);    // never issued
    } catch (const exception& e) { cout << "Error   : " << e.what() << "\n"; }
    lib.catalog();
    return 0;
}
