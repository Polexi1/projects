#include "warehouse.h"
#include <cstdlib> // for rand, srand
#include <ctime>   // for time

std::mutex print_mutex;
void producer(warehouse &w) // Function to produce vehicles
{
    while (true)
    {
        std::shared_ptr<vehicle> new_vehicle;
        if (rand() % 2 == 0)
            new_vehicle = std::make_shared<car>();
        else
            new_vehicle = std::make_shared<truck>();

        w.push(new_vehicle);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void consumer(warehouse &w, int id) // Function to consume vehicles
{
    while (true)
    {
        std::shared_ptr<vehicle> consumer = w.pop();
        print_mutex.lock();
        std::cout << "======= Consumer " << id << " =======\n";
        consumer->print();
        std::cout << std::endl;
        print_mutex.unlock();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        throw std::invalid_argument("please provide a value for the number of consumers");
    }

    int num_consumers = std::atoi(argv[1]);
    if (num_consumers < 2)
    {
        throw std::invalid_argument("There must be at least 2 consumers.");
    }

    std::srand(static_cast<unsigned int>(std::time(0)));

    warehouse w;
    std::thread producer_thread(producer, std::ref(w));

    for (int i = 0; i < num_consumers; ++i)
    {
        std::thread(consumer, std::ref(w), i + 1).detach();
    }

    producer_thread.join();
    return 0;
}
