#include <iostream>
using namespace std;

// Array based heap, isMax true gives a max heap and false gives a min heap
class Heap {
private:
    int* arr;
    int size;
    int capacity;
    bool isMax;

    // Returns true if a should sit above b in this heap, O(1)
    bool higher(int a, int b) const { return isMax ? a > b : a < b; }

    // Doubles the array when it is full, amortized O(1) per push
    void grow() {
        int* bigger = new int[capacity * 2];
        for (int i = 0; i < size; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
        capacity *= 2;
    }

    // Moves the element at i up until its parent is not lower, O(log n)
    void siftUp(int i) {
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (!higher(arr[i], arr[parent])) break;
            int t = arr[i];
            arr[i] = arr[parent];
            arr[parent] = t;
            i = parent;
        }
    }

    // Moves the element at i down until both children are not higher, O(log n)
    void siftDown(int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int best = i;
            if (left < size && higher(arr[left], arr[best])) best = left;
            if (right < size && higher(arr[right], arr[best])) best = right;
            if (best == i) break;
            int t = arr[i];
            arr[i] = arr[best];
            arr[best] = t;
            i = best;
        }
    }

public:
    Heap(bool maxHeap, int cap = 4) {
        if (cap < 1) cap = 1;
        arr = new int[cap];
        size = 0;
        capacity = cap;
        isMax = maxHeap;
    }

    ~Heap() { delete[] arr; }

    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    int getSize() const { return size; }
    bool empty() const { return size == 0; }

    // Returns the root value without removing it, caller must check empty first, O(1)
    int top() const { return arr[0]; }

    // Adds a value at the end and sifts it up, O(log n)
    void push(int value) {
        if (size == capacity) grow();
        arr[size] = value;
        siftUp(size);
        size++;
    }

    // Removes and returns the root, caller must check empty first, O(log n)
    int pop() {
        int root = arr[0];
        arr[0] = arr[size - 1];
        size--;
        if (size > 0) siftDown(0);
        return root;
    }
};

// Tracks the running median using a max heap for the lower half and a min heap for the upper half
class WaitMonitor {
private:
    Heap lower;
    Heap upper;

public:
    WaitMonitor() : lower(true), upper(false) {}

    bool hasData() const { return !lower.empty(); }
    bool lowerEmpty() const { return lower.empty(); }
    bool upperEmpty() const { return upper.empty(); }
    int lowerRoot() const { return lower.top(); }
    int upperRoot() const { return upper.top(); }

    // Inserts a wait into the correct heap then rebalances so max heap has the extra element, O(log n)
    void addWait(int x) {
        if (lower.empty() || x <= lower.top()) lower.push(x);
        else upper.push(x);

        if (lower.getSize() > upper.getSize() + 1) upper.push(lower.pop());
        else if (upper.getSize() > lower.getSize()) lower.push(upper.pop());
    }

    // Returns the median from the two roots, caller must check hasData first, O(1)
    double getMedian() const {
        if (lower.getSize() > upper.getSize()) return lower.top();
        return (lower.top() + upper.top()) / 2.0;
    }
};

int main() {
    const double THRESHOLD = 35.0;
    int n;
    cout << "Enter number of patients: ";
    if (!(cin >> n) || n < 0) {
        cout << "Invalid count\n";
        return 1;
    }

    WaitMonitor monitor;
    cout << "Enter " << n << " waits in minutes: ";
    cout << "\n\nArrival\tWait\tMaxRoot\tMinRoot\tMedian\tStatus\n";
    for (int i = 1; i <= n; i++) {
        int w;
        if (!(cin >> w)) {
            cout << "Invalid or missing value\n";
            return 1;
        }
        monitor.addWait(w);
        cout << i << "\t" << w << "\t" << monitor.lowerRoot() << "\t";
        if (monitor.upperEmpty()) cout << "none";
        else cout << monitor.upperRoot();
        double med = monitor.getMedian();
        cout << "\t" << med << "\t";
        cout << (med > THRESHOLD ? "ALERT: call extra staff" : "Normal") << "\n";
    }

    // Q1: the mean is dragged by one extreme value while the median only follows the middle.
    //     At arrival 6 the wait of 90 lifts the mean from 34 to 43.33 (a jump of 9.33) but the
    //     median only goes from 30 to 37.5, and if 90 were 900 the mean would be 193 while the
    //     median would still be 37.5, so the median is the safer alert measure
    // Q2: if the sizes differ by two or more the roots are no longer the middle of the sorted data,
    //     so the median would be read from the wrong elements and give a wrong value
    // Q3: re-sorting after every arrival costs O(k log k) for the kth patient, so about
    //     O(n^2 log n) total, while the heap method costs O(log n) per arrival, O(n log n) total
    return 0;
}