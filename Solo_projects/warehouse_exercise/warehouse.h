/**
 * @file warehouse.h
 * @author Jesper Röyrvik (Jesper.royrvik@yaelev.se)
 * @brief
 * @version 1.0
 * @date 2025-04-24
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <array>

#ifndef MAX_CAPACITY
#define MAX_CAPACITY 1
#error "MAX_CAPACITY is not defined. Please define it using -DMAX_CAPACITY=."
#elif MAX_CAPACITY <= 7
#error "MAX_CAPACITY must be greater than 7. Please define it using -DMAX_CAPACITY=."
#endif // Ensure MAX_CAPACITY is greater than 7

static int id_counter = 1000; // Global variable for unique vehicle IDs

class vehicle // Base class
{
public:
    int id;
    std::string Model;
    std::string Type;
    std::string Property;

    vehicle() : id(id_counter++) {}
    virtual void print() const = 0; // Pure virtual function
    virtual ~vehicle() = default;   // Virtual destructor for cleanup
};

class car : public vehicle // Inherits from vehicle
{
public:
    car()
    {
        Model = "Audi";
        Type = "Car";
        Property = "Max 5 Passengers";
    }

    void print() const override // Prints car details
    {
        std::cout << "Car id: " << id << "\nModel: " << Model
                  << "\nType: " << Type << "\nProperty: " << Property << std::endl;
    }
};

class truck : public vehicle // Inherits from vehicle
{
public:
    truck()
    {
        Model = "Scania";
        Type = "Truck";
        Property = "Max 4000kg";
    }

    void print() const override // Prints truck details
    {
        std::cout << "Truck id: " << id << "\nModel: " << Model
                  << "\nType: " << Type << "\nProperty: " << Property << std::endl;
    }
};

class warehouse // Circular buffer for vehicles
{
private:
    std::array<std::shared_ptr<vehicle>, MAX_CAPACITY> warehouse_arr;
    int front;
    int rear;
    std::mutex mtx;
    std::condition_variable cv;

public:
    warehouse() : front(0), rear(0) {}

    void push(std::shared_ptr<vehicle> &value) // Add vehicle to warehouse
    {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this]
                { return (rear + 1) % MAX_CAPACITY != front; }); // Wait if warehouse is full
        warehouse_arr[rear] = value;
        rear = (rear + 1) % MAX_CAPACITY;
        cv.notify_all();
    }

    std::shared_ptr<vehicle> pop()
    {
        std::unique_lock<std::mutex> lck(mtx);
        cv.wait(lck, [this]
                { return front != rear; });
        std::shared_ptr<vehicle> value = warehouse_arr[front]; // Get the oldest vehicle
        front = (front + 1) % MAX_CAPACITY;
        cv.notify_all();
        return value;
    }

    warehouse(const warehouse &) = delete;            // Prevent copying
    warehouse &operator=(const warehouse &) = delete; // Prevent copying
};

// Producer and Consumer functions
void producer(warehouse &w);         // Function to produce vehicles
void consumer(warehouse &w, int id); // Function to consume vehicles

#endif // WAREHOUSE_H
