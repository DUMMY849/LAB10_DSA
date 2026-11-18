#include <iostream>
using namespace std;
// Array based max heap with sift-up insertion and heap sort
class MaxHeap {
private:
    int* arr;
    int size;
    int capacity;
    // Doubles the array when it is full, O(n) but amortized O(1) per insert
    void grow() {
        int newCap = capacity * 2;
        int* bigger = new int[newCap];
        for (int i = 0; i < size; i++) bigger[i] = arr[i];
        delete[] arr;
        arr = bigger;
        capacity = newCap;
    }
    // Pushes the element at index i down inside the first n slots of a, O(log n)
    static void siftDown(int* a, int n, int i) {
        while (true) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int largest = i;
            if (left < n && a[left] > a[largest]) largest = left;
            if (right < n && a[right] > a[largest]) largest = right;
            if (largest == i) break;
            int temp = a[i];
            a[i] = a[largest];
            a[largest] = temp;
            i = largest;
        }
    }

public:
    MaxHeap(int cap = 4) {
        if (cap < 1) cap = 1;
        arr = new int[cap];
        size = 0;
        capacity = cap;
    }

    ~MaxHeap() { delete[] arr; }

    MaxHeap(const MaxHeap&) = delete;
    MaxHeap& operator=(const MaxHeap&) = delete;

    int getSize() const { return size; }
    // Inserts value at the end and sifts it up, returns the swap count, O(log n)
    int insert(int value) {
        if (size == capacity) grow();
        int i = size;
        arr[size++] = value;
        int swaps = 0;
        while (i > 0) {
            int parent = (i - 1) / 2;
            if (arr[i] <= arr[parent]) break;
            int temp = arr[i];
            arr[i] = arr[parent];
            arr[parent] = temp;
            i = parent;
            swaps++;
        }
        return swaps;
    }

    // Prints the heap array in level order, O(n)
    void print() const {
        for (int i = 0; i < size; i++) {
            cout << arr[i];
            if (i < size - 1) cout << " ";
        }
        cout << "\n";
    }
    // Sorts a copy ascending using repeated max extraction, caller must delete[] it, O(n log n)
    int* heapSort() const {
        int* temp = new int[size > 0 ? size : 1];
        for (int i = 0; i < size; i++) temp[i] = arr[i];
        for (int end = size - 1; end > 0; end--) {
            int t = temp[0];
            temp[0] = temp[end];
            temp[end] = t;
            siftDown(temp, end, 0);
        }
        return temp;
    }
};
int main() {
    int n;
    cout << "Enter number of values: ";
    if (!(cin >> n) || n < 0) {
        cout << "Invalid count\n";
        return 1;
    }
    MaxHeap heap;
    cout << "Enter " << n << " values: ";
    cout << "\n\nInsert\tSwaps\tHeap after insertion\n";
    for (int i = 0; i < n; i++) {
        int v;
        if (!(cin >> v)) {
            cout << "Invalid or missing value\n";
            return 1;
        }
        int swaps = heap.insert(v);
        cout << v << "\t" << swaps << "\t";
        heap.print();
    }
    cout << "\nFinal max heap: ";
    heap.print();
    int* sorted = heap.heapSort();
    cout << "Sorted output: ";
    for (int i = 0; i < heap.getSize(); i++) {
        cout << sorted[i];
        if (i < heap.getSize() - 1) cout << " ";
    }
    cout << "\n";
    delete[] sorted;

    // Q1: worst case swaps for one insertion into n elements is floor(log2(n)) since the
    //     new leaf can climb at most the height of the tree to the root, so O(log n)
    // Q2: n insertions cost O(n log n) in the worst case (each insert is O(log n)),
    //     heap sort costs O(n log n) for all cases (n extractions, each sift-down O(log n))
    return 0;
}