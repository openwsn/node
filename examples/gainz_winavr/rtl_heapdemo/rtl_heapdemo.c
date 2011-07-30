
int main() {
    MinHeap<int> heap(30);
    heap.insert(32);
    heap.insert(23);    
    heap.insert(9);
    heap.insert(4);
    heap.insert(21);
    heap.insert(13);
    heap.insert(15);
    heap.insert(2);
    heap.printAll();
    
    heap.deleteNode(2);
    heap.printAll();
    return 0;
}