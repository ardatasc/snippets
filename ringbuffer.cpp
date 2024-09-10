#include <iostream>
#include <vector>
#include <ctime>

struct Data {
    std::time_t stamp;
    std::string value;

    Data(std::time_t s, const std::string& v) : stamp(s), value(v) {}
};

class RingBuffer {
public:
    RingBuffer(size_t size) 
        : data(size, nullptr), size(size), lastInsert(-1), nextRead(0) {}

    ~RingBuffer() {
        for (auto item : data) {
            delete item;
        }
    }

    void insert(const Data& input) {
        lastInsert = (lastInsert + 1) % size;
        delete data[lastInsert]; // Free the old data if it exists
        data[lastInsert] = new Data(input); // Copy the input data into the buffer

        if (nextRead == lastInsert) {
            nextRead = (nextRead + 1) % size;
        }
    }

    std::vector<Data*> emit() {
        std::vector<Data*> output;
        while (true) {
            if (data[nextRead] != nullptr) {
                output.push_back(data[nextRead]);
                data[nextRead] = nullptr; // Remove the data from the buffer
            }
            if (nextRead == lastInsert || lastInsert == -1) {
                break;
            }
            nextRead = (nextRead + 1) % size;
        }
        return output;
    }

private:
    std::vector<Data*> data;
    size_t size;
    int lastInsert;
    int nextRead;
};

int main() {
    RingBuffer rb(5);
    char currentRune = 'a' - 1;

    std::cout << "EMPTY TEST:" << std::endl;
    auto emptyEmit = rb.emit();
    for (auto item : emptyEmit) {
        std::cout << "Stamp: " << item->stamp << ", Value: " << item->value << std::endl;
    }

    std::cout << "FULL TEST:" << std::endl;
    for (int i = 0; i < 10; ++i) {
        currentRune++;
        rb.insert(Data(std::time(nullptr), std::string(1, currentRune)));
    }

    auto fullEmit = rb.emit();
    for (auto item : fullEmit) {
        std::cout << "Stamp: " << item->stamp << ", Value: " << item->value << std::endl;
        delete item; // Free the memory allocated in emit()
    }

    return 0;
}
