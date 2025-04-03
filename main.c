#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_BOOKS 100

// Struct representing a book
typedef struct Book {
    int id;
    char title[100];
    char author[50];
    int is_available;
    struct Book *left, *right;
} Book;

// Struct representing a user
typedef struct User {
    int id;
    char name[50];
    struct BorrowedBook *borrowed_books;  // Linked list of borrowed books
} User;

// Linked list node for borrowed books
typedef struct BorrowedBook {
    int book_id;
    struct BorrowedBook *next;
} BorrowedBook;

// Queue Node for waitlist
typedef struct WaitlistNode {
    int user_id;
    struct WaitlistNode *next;
} WaitlistNode;

// Prototypes
Book* addBook(Book* root, int id, char* title, char* author);
Book* searchBook(Book* root, int id);
Book* deleteBook(Book* root, int id);
void borrowBook(User users[], Book *book, int user_id, WaitlistNode **waitlist);
void returnBook(User users[], Book *book, int user_id, WaitlistNode **waitlist);
void displayBooks(Book* root);
void displayBorrowedBooks(User user);
void displayWaitlist(WaitlistNode *waitlist);

// Helper functions
User* searchUser(User users[], int user_id);

// Main function
int main() {
    Book *root = NULL;  // Root of the book BST
    User users[MAX_USERS];
    WaitlistNode *waitlist[MAX_BOOKS] = {NULL};  // Array of waitlists for each book
    int user_count = 0;

    // Sample Books
    root = addBook(root, 1, "The Great Gatsby", "F. Scott Fitzgerald");
    root = addBook(root, 2, "1984", "George Orwell");
    root = addBook(root, 3, "To Kill a Mockingbird", "Harper Lee");

    // Sample Users
    users[user_count].id = 1;
    strcpy(users[user_count++].name, "Arsh");
    users[user_count].id = 2;
    strcpy(users[user_count++].name, "Kaushik");

    int choice, book_id, user_id;
    while (1) {
        printf("\nLibrary Management System:\n");
        printf("1. Display Books\n");
        printf("2. Search Book by ID\n");
        printf("3. Add Book\n");
        printf("4. Remove Book\n");
        printf("5. Borrow Book\n");
        printf("6. Return Book\n");
        // printf("7. Display User Borrowed Books\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                displayBooks(root);
                break;
            case 2:
                printf("Enter Book ID to search: ");
                scanf("%d", &book_id);
                Book* book = searchBook(root, book_id);
                if (book)
                    printf("Book Found: %s by %s\n", book->title, book->author);
                else
                    printf("Book not found\n");
                break;
            case 3:
                printf("Enter Book ID: ");
                scanf("%d", &book_id);
                char title[100], author[50];
                printf("Enter Title: ");
                scanf(" %[^\n]", title);
                printf("Enter Author: ");
                scanf(" %[^\n]", author);
                root = addBook(root, book_id, title, author);
                printf("Book added successfully.\n");
                break;
            case 4:
                printf("Enter Book ID to remove: ");
                scanf("%d", &book_id);
                root = deleteBook(root, book_id);
                printf("Book removed successfully.\n");
                break;
            case 5:
                printf("Enter User ID: ");
                scanf("%d", &user_id);
                printf("Enter Book ID: ");
                scanf("%d", &book_id);
                book = searchBook(root, book_id);
                if (book) {
                    borrowBook(users, book, user_id, &waitlist[book_id]);
                } else {
                    printf("Book not found\n");
                }
                break;
            case 6:
                printf("Enter User ID: ");
                scanf("%d", &user_id);
                printf("Enter Book ID: ");
                scanf("%d", &book_id);
                book = searchBook(root, book_id);
                if (book) {
                    returnBook(users, book, user_id, &waitlist[book_id]);
                } else {
                    printf("Book not found\n");
                }
                break;
            // case 7:
            //     printf("Enter User ID: ");
            //     scanf("%d", &user_id);
            //     User *user = searchUser(users, user_id);
            //     if (user) {
            //         displayBorrowedBooks(*user);
            //     } else {
            //         printf("User not found\n");
            //     }
            //     break;
            case 7:
                exit(0);
            default:
                printf("Invalid choice\n");
        }
    }
    return 0;
}

/** Function Definitions **/

// Helper to search user by ID
User* searchUser(User users[], int user_id) {
    for (int i = 0; i < MAX_USERS; i++) {
        if (users[i].id == user_id) {
            return &users[i];
        }
    }
    return NULL;
}

// Add book to BST
Book* addBook(Book* root, int id, char* title, char* author) {
    if (root == NULL) {
        root = (Book*)malloc(sizeof(Book));
        root->id = id;
        strcpy(root->title, title);
        strcpy(root->author, author);
        root->is_available = 1;
        root->left = root->right = NULL;
    } else if (id < root->id) {
        root->left = addBook(root->left, id, title, author);
    } else {
        root->right = addBook(root->right, id, title, author);
    }
    return root;
}

// Search for a book by ID
Book* searchBook(Book* root, int id) {
    if (root == NULL || root->id == id) {
        return root;
    } else if (id < root->id) {
        return searchBook(root->left, id);
    } else {
        return searchBook(root->right, id);
    }
}

// Delete a book from the BST
Book* deleteBook(Book* root, int id) {
    if (root == NULL) return root;
    if (id < root->id) {
        root->left = deleteBook(root->left, id);
    } else if (id > root->id) {
        root->right = deleteBook(root->right, id);
    } else {
        if (root->left == NULL) {
            Book *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Book *temp = root->left;
            free(root);
            return temp;
        }
        Book* temp = root->right;
        while (temp && temp->left != NULL) {
            temp = temp->left;
        }
        root->id = temp->id;
        strcpy(root->title, temp->title);
        strcpy(root->author, temp->author);
        root->right = deleteBook(root->right, temp->id);
    }
    return root;
}

// Borrow a book
void borrowBook(User users[], Book *book, int user_id, WaitlistNode **waitlist) {
    if (book->is_available) {
        book->is_available = 0;
        BorrowedBook *borrowed = (BorrowedBook*)malloc(sizeof(BorrowedBook));
        borrowed->book_id = book->id;
        borrowed->next = users[user_id].borrowed_books;
        users[user_id].borrowed_books = borrowed;
        printf("Book borrowed successfully!\n");
    } else {
        printf("Book is currently borrowed. Adding to waitlist...\n");
        WaitlistNode *node = (WaitlistNode*)malloc(sizeof(WaitlistNode));
        node->user_id = user_id;
        node->next = *waitlist;
        *waitlist = node;
    }
}

// Return a book
void returnBook(User users[], Book *book, int user_id, WaitlistNode **waitlist) {
    BorrowedBook *prev = NULL, *current = users[user_id].borrowed_books;
    while (current != NULL && current->book_id != book->id) {
        prev = current;
        current = current->next;
    }
    if (current != NULL) {
        if (prev == NULL) {
            users[user_id].borrowed_books = current->next;
        } else {
            prev->next = current->next;
        }
        free(current);
        book->is_available = 1;
        printf("Book returned successfully!\n");

        // Serve the waitlist if any
        if (*waitlist != NULL) {
            int next_user_id = (*waitlist)->user_id;
            WaitlistNode *temp = *waitlist;
            *waitlist = (*waitlist)->next;
            free(temp);
            borrowBook(users, book, next_user_id, waitlist);
        }
    } else {
        printf("User does not have this book borrowed.\n");
    }
}

// Display all books (In-order traversal of BST)
void displayBooks(Book* root) {
    if (root != NULL) {
        displayBooks(root->left);
        printf("ID: %d, Title: %s, Author: %s, %s\n", root->id, root->title, root->author,
               root->is_available ? "Available" : "Borrowed");
        displayBooks(root->right);
    }
}

// Display borrowed books for a user
// void displayBorrowedBooks(User user) {
//     BorrowedBook *current = user.borrowed_books;
//     if (current == NULL) {
//         printf("No books borrowed by this user.\n");
//     } 
//     else {
//         printf("Borrowed Books: \n");
//         while (current != NULL) {
//             printf("Book ID: %d\n", current->book_id);
//             current = current->next;
//         }
//     }
// }

