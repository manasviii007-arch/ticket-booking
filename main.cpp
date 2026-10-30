/*
 * ============================================================================
 *  Ticket Booking Queue Simulation
 *  Data Structures Lab - Project-Based Learning (PBL)
 * ----------------------------------------------------------------------------
 *  A FIFO (First-In, First-Out) queue implemented from scratch using a
 *  Singly Linked List. No STL containers (std::queue, std::list, std::vector)
 *  are used; every node is allocated with `new` and released with `delete`.
 *
 *  Complexity summary
 *  ------------------
 *    enqueue()       : O(1)  - the rear pointer gives direct access to the tail
 *    dequeue()       : O(1)  - the front pointer gives direct access to the head
 *    peek()          : O(1)
 *    displayQueue()  : O(n)  - must visit every node
 *    ~TicketQueue()  : O(n)  - frees every remaining node
 *
 *  Compile : g++ -std=c++11 -Wall -Wextra -o ticket_queue main.cpp
 *  Run     : ./ticket_queue
 * ============================================================================
 */

#include <iostream>
#include <string>
#include <cctype>

// ----------------------------------------------------------------------------
//  Node: one ticket (one waiting customer) in the queue.
// ----------------------------------------------------------------------------
struct Node {
    int         ticketId;      // Auto-generated unique ticket number
    std::string customerName;  // Name of the customer
    std::string destination;   // Travel destination requested
    Node*       next;          // Pointer to the next ticket (towards the rear)

    // Constructor: 'next' starts as nullptr, so a new node is always a valid
    // tail node (it points to nothing until another node is linked after it).
    Node(int id, const std::string& name, const std::string& dest)
        : ticketId(id), customerName(name), destination(dest), next(nullptr) {}
};

// ----------------------------------------------------------------------------
//  TicketQueue: FIFO queue managed through explicit front and rear pointers.
//
//      front                                   rear
//        |                                      |
//        v                                      v
//     [1001] --> [1002] --> [1003] --> [1004] --> nullptr
//   (served next)                         (joined last)
// ----------------------------------------------------------------------------
class TicketQueue {
private:
    Node* front;  // Oldest ticket: the next one to be processed
    Node* rear;   // Newest ticket: the last one that was booked
    int   count;  // Number of tickets currently waiting

public:
    // Constructor: an empty queue has both pointers set to nullptr.
    TicketQueue() : front(nullptr), rear(nullptr), count(0) {}

    // Destructor: frees every node still in the queue so no heap memory leaks
    // when the program ends.
    ~TicketQueue() {
        while (!isEmpty()) {
            Node* temp = front;     // Remember the current head
            front = front->next;    // Move front forward to the next node
            delete temp;            // Release the old head back to the heap
        }
        rear = nullptr;             // Queue is empty, so rear must be null too
    }

    // Copying a queue would make two objects own the same nodes and cause a
    // double delete. The copy operations are therefore explicitly disabled.
    TicketQueue(const TicketQueue&)            = delete;
    TicketQueue& operator=(const TicketQueue&) = delete;

    bool isEmpty() const { return front == nullptr; }
    int  size()    const { return count; }

    // ------------------------------------------------------------------------
    //  enqueue: add a new ticket at the REAR of the queue.   Time: O(1)
    // ------------------------------------------------------------------------
    void enqueue(int id, const std::string& name, const std::string& dest) {
        // Step 1: allocate a new node on the heap (dynamic memory allocation).
        Node* newNode = new Node(id, name, dest);

        if (isEmpty()) {
            // Case A: the queue is empty. The new node is both the first and
            // the last node, so front and rear both point to it.
            front = newNode;
            rear  = newNode;
        } else {
            // Case B: the queue already has nodes.
            //   1) Link the current last node to the new node.
            //   2) Move rear to the new node, which is now the last node.
            rear->next = newNode;
            rear       = newNode;
        }
        ++count;

        std::cout << "\n  [OK] Ticket #" << id << " booked for " << name
                  << " (Destination: " << dest << ").\n"
                  << "       Position in queue: " << count << "\n";
    }

    // ------------------------------------------------------------------------
    //  dequeue: process and remove the ticket at the FRONT.   Time: O(1)
    // ------------------------------------------------------------------------
    void dequeue() {
        // Underflow check: nothing to remove from an empty queue.
        if (isEmpty()) {
            std::cout << "\n  [!] Queue is empty. There are no tickets to process.\n";
            return;
        }

        // Step 1: keep a temporary pointer to the node being removed. Without
        // it, moving 'front' would lose the only reference to that node and
        // leak its memory.
        Node* temp = front;

        std::cout << "\n  [CONFIRMED] Ticket #" << temp->ticketId
                  << " | Customer: " << temp->customerName
                  << " | Destination: " << temp->destination << "\n";

        // Step 2: advance front to the second node (or nullptr if there was
        // only one node).
        front = front->next;

        // Step 3: critical edge case. If the queue just became empty, rear
        // would still point to the node we are about to delete (a dangling
        // pointer). Reset it to nullptr.
        if (front == nullptr) {
            rear = nullptr;
        }

        // Step 4: return the removed node's memory to the heap.
        delete temp;
        temp = nullptr;  // Avoid leaving a dangling local pointer behind
        --count;

        std::cout << "       Tickets still waiting: " << count << "\n";
    }

    // ------------------------------------------------------------------------
    //  peek: view the front ticket WITHOUT removing it.   Time: O(1)
    // ------------------------------------------------------------------------
    void peek() const {
        if (isEmpty()) {
            std::cout << "\n  [!] Queue is empty. No customer is waiting.\n";
            return;
        }
        // Read-only access through 'front'. No pointers are changed.
        std::cout << "\n  Next in line:\n"
                  << "    Ticket ID   : " << front->ticketId     << "\n"
                  << "    Customer    : " << front->customerName << "\n"
                  << "    Destination : " << front->destination  << "\n";
    }

    // ------------------------------------------------------------------------
    //  displayQueue: print all pending tickets, front to rear.   Time: O(n)
    // ------------------------------------------------------------------------
    void displayQueue() const {
        if (isEmpty()) {
            std::cout << "\n  [!] Queue is empty. No pending tickets.\n";
            return;
        }

        std::cout << "\n  ---------------- PENDING TICKETS ----------------\n";

        // Use a separate traversal pointer so 'front' itself is never moved.
        Node* current = front;
        int position = 1;
        while (current != nullptr) {
            std::cout << "  " << position << ". Ticket #" << current->ticketId
                      << " | " << current->customerName
                      << " -> " << current->destination;
            if (current == front) std::cout << "   <-- FRONT";
            if (current == rear)  std::cout << "   <-- REAR";
            std::cout << "\n";

            current = current->next;  // Hop to the next node in the chain
            ++position;
        }
        std::cout << "  -------------------------------------------------\n"
                  << "  Total pending: " << count << "\n";
    }
};

// ----------------------------------------------------------------------------
//  Helper functions for safe console input
// ----------------------------------------------------------------------------

// Returns a copy of 's' without leading or trailing whitespace.
static std::string trim(const std::string& s) {
    std::size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        ++start;
    }
    std::size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        --end;
    }
    return s.substr(start, end - start);
}

// Keeps asking until the user types a non-empty line. Returns false if the
// input stream is closed (for example Ctrl+D / Ctrl+Z), so the caller can exit.
static bool readNonEmpty(const std::string& prompt, std::string& out) {
    std::string line;
    while (true) {
        std::cout << prompt;
        if (!std::getline(std::cin, line)) return false;
        line = trim(line);
        if (!line.empty()) {
            out = line;
            return true;
        }
        std::cout << "  [!] Input cannot be empty. Please try again.\n";
    }
}

static void printMenu() {
    std::cout << "\n==============================================\n"
              << "        TICKET BOOKING QUEUE SIMULATION\n"
              << "==============================================\n"
              << "  1. Book a new ticket\n"
              << "  2. Process / confirm the next ticket\n"
              << "  3. View the queue\n"
              << "  4. Peek at the next customer\n"
              << "  5. Exit\n"
              << "----------------------------------------------\n"
              << "  Enter your choice (1-5): ";
}

// ----------------------------------------------------------------------------
//  main: interactive CLI driven by a while loop and a switch statement.
// ----------------------------------------------------------------------------
int main() {
    TicketQueue queue;      // Queue object lives on the stack; its nodes live
                            // on the heap and are freed by the destructor.
    int  nextTicketId = 1001;  // Auto-incrementing ticket number
    bool running = true;

    std::cout << "Welcome to the Ticket Booking System!\n";

    while (running) {
        printMenu();

        std::string input;
        if (!std::getline(std::cin, input)) break;  // Input stream closed
        input = trim(input);

        // Accept only a single digit from '1' to '5'; anything else is invalid.
        int choice = (input.size() == 1 && std::isdigit(static_cast<unsigned char>(input[0])))
                         ? input[0] - '0'
                         : 0;

        switch (choice) {
            case 1: {
                std::string name, destination;
                if (!readNonEmpty("  Enter customer name : ", name))        { running = false; break; }
                if (!readNonEmpty("  Enter destination   : ", destination)) { running = false; break; }

                // The ID is generated automatically and incremented after use.
                queue.enqueue(nextTicketId++, name, destination);
                break;
            }
            case 2:
                queue.dequeue();
                break;
            case 3:
                queue.displayQueue();
                break;
            case 4:
                queue.peek();
                break;
            case 5:
                std::cout << "\n  Exiting. " << queue.size()
                          << " pending ticket(s) will be released from memory.\n";
                running = false;
                break;
            default:
                std::cout << "\n  [!] Invalid choice. Please enter a number from 1 to 5.\n";
                break;
        }
    }

    std::cout << "  Thank you for using the Ticket Booking System. Goodbye!\n";
    return 0;  // 'queue' goes out of scope here and its destructor frees all nodes.
}
