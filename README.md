# ticket-booking[README.md](https://github.com/user-attachments/files/34190054/README.md)
# 🎟️ Ticket Booking Queue Simulation

### *A First-In, First-Out Reservation Engine Built on a Custom Singly Linked-List Queue*

![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C?logo=cplusplus&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-GCC%20%2F%20g%2B%2B-orange)
![Data Structure](https://img.shields.io/badge/Data%20Structure-Linked%20List%20Queue-blueviolet)
![Complexity](https://img.shields.io/badge/Enqueue%20%2F%20Dequeue-O(1)-success)
![Course](https://img.shields.io/badge/Course-Data%20Structures%20Lab-lightgrey)

> **Project-Based Learning (PBL) Submission** — Data Structures Lab
> **Department:** AI & ML | **Group:** G1 | **Topic:** Ticket Booking / Print-Job Queue Simulation using a Linked-List-Based Queue

---

## 📑 Table of Contents

1. [Project Overview](#1--project-overview)
2. [Data Structure Architecture](#2--data-structure-architecture)
3. [Key Features](#3--key-features)
4. [Memory Trade-offs & Analysis](#4--memory-trade-offs--analysis)
5. [How to Run & Install](#5--how-to-run--install)
6. [Code Structure Blueprint](#6--code-structure-blueprint)
7. [Screenshots](#7--screenshots)
8. [Future Scope](#8--future-scope)
9. [Author & Acknowledgements](#9--author--acknowledgements)

---

## 1. 📌 Project Overview

The **Ticket Booking Queue Simulation** models a real-world reservation counter in which customers arrive at unpredictable times and must be served strictly in the order of their arrival. Each incoming booking request is placed at the back of a waiting line, and the system always serves the request at the front of the line.

The entire engine is powered by a **custom-built Queue implemented with a Singly Linked List**. No built-in queue facilities (such as `std::queue`, `std::deque`, or `std::list`) are used. Every node is allocated, linked, unlinked, and released manually, which demonstrates a working understanding of **pointer manipulation**, **dynamic memory allocation**, and **memory lifecycle management**.

### Why a Queue Is the Ideal Choice

| Real-World Requirement | How the Queue Satisfies It |
|---|---|
| **Fairness** — earlier customers must be served first | FIFO ordering guarantees *first come, first served* |
| **Unpredictable arrival rate** — demand spikes during sales or events | Linked-list nodes are allocated on demand, so capacity is never fixed in advance |
| **Fast processing** — thousands of requests per minute | Both insertion and removal are **O(1)** |
| **Only the ends matter** — no random access is needed | A queue exposes exactly two access points: `front` and `rear` |
| **Natural analogue** — print spoolers, call centres, CPU scheduling | The same abstraction applies to any "serve in arrival order" problem |

---

## 2. 🏗️ Data Structure Architecture

### 2.1 The `Node` Structure

Each waiting customer is represented by a single heap-allocated **node**. A node stores the booking payload together with a pointer to the next node in the line.

```cpp
struct Node {
    int         ticketId;      // Unique booking / token number
    std::string customerName;  // Name of the customer
    int         seats;         // Number of seats requested
    Node*       next;          // Pointer to the next node in the queue

    Node(int id, const std::string& name, int s)
        : ticketId(id), customerName(name), seats(s), next(nullptr) {}
};
```

**Design notes**

- `next` is initialised to `nullptr`, so a freshly created node is always a safe tail node.
- The node is *self-contained*: the queue stores no additional metadata per element.
- Memory for each node is acquired with `new` at enqueue time and returned with `delete` at dequeue time.

### 2.2 The `Queue` Class

The queue maintains exactly **two pointers** and an optional counter:

```cpp
class TicketQueue {
private:
    Node* front;   // Points to the oldest request (next to be served)
    Node* rear;    // Points to the newest request (most recently added)
    int   size;    // Number of requests currently waiting

public:
    TicketQueue();
    ~TicketQueue();

    void enqueue(int id, const std::string& name, int seats);
    void dequeue();
    void peek() const;
    void display() const;
    bool isEmpty() const;
    int  getSize() const;
};
```

### 2.3 How `front` and `rear` Work

```
        FRONT                                   REAR
          │                                       │
          ▼                                       ▼
       ┌──────┐     ┌──────┐     ┌──────┐     ┌──────┐
       │ T101 │────▶│ T102 │────▶│ T103 │────▶│ T104 │────▶ NULL
       └──────┘     └──────┘     └──────┘     └──────┘
     (served next)                           (joined last)
```

#### `enqueue` — Insert at the Rear

1. Allocate a new node: `Node* temp = new Node(...)`.
2. **Case A — Queue is empty** (`front == nullptr`): set both `front = rear = temp`.
3. **Case B — Queue has elements**: link the current tail to the new node (`rear->next = temp`), then advance the tail (`rear = temp`).
4. Increment `size`.

```cpp
void TicketQueue::enqueue(int id, const std::string& name, int seats) {
    Node* temp = new Node(id, name, seats);
    if (rear == nullptr) {          // Empty queue
        front = rear = temp;
    } else {                        // Non-empty queue
        rear->next = temp;
        rear = temp;
    }
    ++size;
}
```

#### `dequeue` — Remove from the Front

1. **Underflow check** — if `front == nullptr`, report that the queue is empty and return.
2. Store the current head in a temporary pointer: `Node* temp = front`.
3. Advance the head: `front = front->next`.
4. **Critical edge case** — if `front` is now `nullptr`, the queue has become empty, so `rear` **must also be reset to `nullptr`**. Omitting this step leaves `rear` as a *dangling pointer* to freed memory.
5. Release the removed node: `delete temp`.
6. Decrement `size`.

```cpp
void TicketQueue::dequeue() {
    if (front == nullptr) {
        std::cout << "Queue is empty. No bookings to process.\n";
        return;
    }
    Node* temp = front;
    front = front->next;
    if (front == nullptr) {         // Queue became empty
        rear = nullptr;             // Prevent dangling pointer
    }
    delete temp;                    // Free heap memory
    --size;
}
```

### 2.4 Time & Space Complexity

| Operation | Description | Time Complexity | Space Complexity |
|---|---|:---:|:---:|
| `enqueue()` | Insert a node at the rear | **O(1)** | O(1) |
| `dequeue()` | Remove the node at the front | **O(1)** | O(1) |
| `peek()` | Read the front element | **O(1)** | O(1) |
| `isEmpty()` | Check whether `front == nullptr` | **O(1)** | O(1) |
| `getSize()` | Return the stored counter | **O(1)** | O(1) |
| `display()` | Traverse and print all nodes | O(n) | O(1) |
| **Overall storage** | *n* queued requests | — | **O(n)** |

> **Why O(1)?** The `rear` pointer gives direct access to the tail, so enqueue never traverses the list. The `front` pointer gives direct access to the head, so dequeue never shifts elements. This is a key advantage over a *head-only* linked list, where appending would cost O(n).

---

## 3. ✨ Key Features

- 🖥️ **Interactive CLI Menu** — a looped, menu-driven interface for enqueueing a booking, processing the next booking, viewing the front request, displaying the full queue, checking its size, and exiting.
- 🧠 **Dynamic Memory Management** — nodes are allocated with `new` on demand and released with `delete` on service; the queue has **no predefined capacity**.
- 🛡️ **Error & Empty-Queue Handling** — graceful underflow detection on `dequeue`, `peek`, and `display`, with clear user feedback instead of undefined behaviour.
- 🔒 **Dangling-Pointer Safety** — `rear` is explicitly reset when the final node is removed.
- ♻️ **Leak-Free Shutdown** — a destructor walks the list and frees any remaining nodes on exit.
- ✅ **Input Validation** — rejects non-numeric menu choices and invalid seat counts without crashing.
- 🚫 **Zero Built-in Containers** — the queue is implemented entirely from scratch with raw pointers.
- 📈 **Live Queue Statistics** — the current queue length is always available in O(1).

---

## 4. ⚖️ Memory Trade-offs & Analysis

### 4.1 Linked-List Queue vs. Array-Based Queue

| Criterion | Linked-List Queue ✅ | Array-Based Queue |
|---|---|---|
| **Capacity** | Dynamic — grows and shrinks at runtime | Fixed at compile/allocation time |
| **Overflow** | Only when the system runs out of heap memory | Occurs as soon as the array is full |
| **Wasted memory** | None — exactly *n* nodes exist for *n* requests | Unused slots are reserved even when idle |
| **Per-element overhead** | One extra pointer (8 bytes on a 64-bit system) | None |
| **Enqueue / Dequeue** | O(1) | O(1) with a *circular* buffer; O(n) with naive shifting |
| **Resizing cost** | Never required | O(n) copy whenever a dynamic array must grow |
| **Memory locality** | Poor — nodes are scattered across the heap | Excellent — contiguous memory is cache-friendly |
| **Implementation complexity** | Higher (pointer discipline required) | Lower |

### 4.2 Analysis

**Why the linked list wins for ticketing.** Booking demand is highly bursty. A fixed-size array must either be over-provisioned, which wastes memory during quiet periods, or under-provisioned, which rejects customers during peak load. A linked-list queue sidesteps this dilemma: capacity follows demand exactly, so **queue overflow is eliminated as a design concern**.

**The cost.** Each node carries a `next` pointer, which adds a fixed overhead per request. For small payloads this overhead is proportionally noticeable, and scattered heap allocations reduce cache efficiency. Every `new` and `delete` also incurs allocator overhead.

**Verdict.** In a dynamic system where the number of waiting customers is unknown and variable, the **pointer overhead is a worthwhile price** for guaranteed flexibility, no wasted space, and no resizing pauses. A circular array queue would be preferable only when the maximum load is strictly bounded and raw cache performance is the priority.

---

## 5. 🚀 How to Run & Install

### 5.1 Prerequisites

- A C++ compiler supporting **C++11 or later** (GCC / g++ 7.0+ recommended)
- `make` *(optional)*
- `git`

Verify your toolchain:

```bash
g++ --version
```

### 5.2 Step-by-Step Instructions

**Step 1 — Clone the repository**

```bash
git clone https://github.com/<your-username>/<your-repo-name>.git
cd <your-repo-name>
```

**Step 2 — Compile the project**

*Option A: Direct compilation*

```bash
g++ -std=c++11 -Wall -Wextra -o ticket_queue src/main.cpp src/TicketQueue.cpp -Iinclude
```

*Option B: Using the Makefile (if provided)*

```bash
make
```

**Step 3 — Run the executable**

```bash
# Linux / macOS
./ticket_queue

# Windows (Command Prompt / PowerShell)
ticket_queue.exe
```

**Step 4 — (Optional) Check for memory leaks with Valgrind**

```bash
valgrind --leak-check=full ./ticket_queue
```

**Step 5 — (Optional) Clean build artifacts**

```bash
make clean
# or manually:
rm -f ticket_queue
```

> 💡 **Single-file variant:** if the whole program lives in one file, compile with
> `g++ -std=c++11 -Wall -o ticket_queue main.cpp`

---

## 6. 🗂️ Code Structure Blueprint

```
ticket-booking-queue-simulation/
│
├── 📁 include/
│   └── 📄 TicketQueue.h        # Node struct + TicketQueue class declaration
│
├── 📁 src/
│   ├── 📄 TicketQueue.cpp      # enqueue, dequeue, peek, display, destructor
│   └── 📄 main.cpp             # CLI menu loop and user interaction
│
├── 📁 docs/
│   ├── 📄 Project_Report.pdf   # Final PBL report
│   └── 📄 Poster.pdf           # Poster presentation
│
├── 📁 screenshots/
│   ├── 🖼️ 01_main_menu.png
│   ├── 🖼️ 02_enqueue.png
│   ├── 🖼️ 03_display_queue.png
│   ├── 🖼️ 04_dequeue.png
│   └── 🖼️ 05_empty_queue.png
│
├── 📄 Makefile                 # Build automation (optional)
├── 📄 .gitignore               # Ignores binaries and build artifacts
└── 📄 README.md                # Project documentation (this file)
```

| Component | Responsibility |
|---|---|
| `TicketQueue.h` | Declares the `Node` structure and the `TicketQueue` interface |
| `TicketQueue.cpp` | Implements all queue operations with manual pointer handling |
| `main.cpp` | Presents the interactive menu and routes user input to queue operations |
| `docs/` | Holds the report and poster for evaluation |
| `screenshots/` | Holds the terminal output images embedded below |

---

## 7. 📸 Screenshots

### 7.1 Main Menu

<!-- 📌 PLACEHOLDER: Drop your terminal screenshot of the main menu here -->
![Main Menu](screenshots/01_main_menu.png)

*Figure 1 — Interactive CLI menu on program start.*

### 7.2 Enqueue Operation (Adding Bookings)

<!-- 📌 PLACEHOLDER: Drop your screenshot showing one or more bookings being added -->
![Enqueue Operation](screenshots/02_enqueue.png)

*Figure 2 — Booking requests joining the rear of the queue.*

### 7.3 Display Queue

<!-- 📌 PLACEHOLDER: Drop your screenshot of the full queue being displayed -->
![Display Queue](screenshots/03_display_queue.png)

*Figure 3 — Current queue contents shown from front to rear.*

### 7.4 Dequeue Operation (Processing Bookings)

<!-- 📌 PLACEHOLDER: Drop your screenshot of a booking being processed/served -->
![Dequeue Operation](screenshots/04_dequeue.png)

*Figure 4 — The front booking is served and its node is freed.*

### 7.5 Empty-Queue Handling

<!-- 📌 PLACEHOLDER: Drop your screenshot of the underflow / empty queue message -->
![Empty Queue Handling](screenshots/05_empty_queue.png)

*Figure 5 — Graceful handling when no bookings remain.*

### 7.6 Valgrind Memory Report *(Optional)*

<!-- 📌 PLACEHOLDER: Drop your Valgrind "All heap blocks were freed" screenshot here -->
![Valgrind Report](screenshots/06_valgrind.png)

*Figure 6 — Verification that no memory leaks occur.*

---

## 8. 🔭 Future Scope

- **Priority Queue extension** for VIP or senior-citizen bookings
- **File persistence** to save and restore queue state between sessions
- **Timestamps and wait-time analytics** per booking
- **Generic templates** (`template <typename T>`) to reuse the queue for print-job simulation
- **Multi-counter simulation** with several queues served in parallel
- **Unit tests** using a framework such as GoogleTest or Catch2

---

## 9. 👨‍🎓 Author & Acknowledgements

| | |
|---|---|
| **Name** | `<Your Name>` |
| **Roll No.** | `<Your Roll Number>` |
| **Course** | Data Structures Lab — DS LAB G1 [2AIML] |
| **Instructor** | Akash Bhardwaj |
| **Institution** | `<Your University / College Name>` |

Special thanks to the course instructor for guidance on the PBL framework and evaluation criteria.

---

<p align="center">
  <i>Built with pointers, discipline, and a lot of <code>delete</code> statements.</i>
</p>
