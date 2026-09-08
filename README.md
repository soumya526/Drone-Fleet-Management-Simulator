# Drone-Fleet-Management-Simulator

# 🚁 Drone Fleet Management and Mission Simulation System

A C++ Object-Oriented Programming project that simulates the management of a fleet of different types of drones and their missions.

The system allows users to register and manage drones, create missions, assign suitable drones based on their capabilities, simulate mission completion, monitor battery levels, and maintain mission records.

This project is primarily developed to demonstrate the practical implementation of **Object-Oriented Programming concepts in C++**.

---

## 📌 Project Overview

With the increasing use of drones in areas such as delivery, surveillance, mapping, and emergency response, managing multiple drones efficiently can become challenging.

The **Drone Fleet Management and Mission Simulation System** provides a software-based simulation where a fleet of drones can be monitored and assigned different missions.

Each drone has its own capabilities, such as:

- Battery level
- Maximum range
- Maximum speed
- Payload capacity
- Current status
- Drone type

When a mission is created, the system checks the available drones and determines whether a suitable drone can perform the mission based on factors such as battery, range, payload capacity, and availability.

> **Note:** This project is a software simulation and does not directly control physical drones or flight hardware.

---

## 🎯 Objectives

The main objectives of this project are:

- To implement real-world concepts using C++ OOP.
- To manage multiple types of drones using inheritance.
- To demonstrate runtime polymorphism through different drone behaviors.
- To simulate drone mission assignment and completion.
- To monitor drone battery and operational status.
- To handle invalid mission assignments using exception handling.
- To use STL containers for efficient fleet and mission management.
- To store and retrieve data using file handling.

---

## ✨ Features

### 🚁 Drone Management

- Add/Register new drones
- Remove drones
- Display all drones
- Search for a drone
- Track drone status
- Monitor battery level
- Track payload capacity
- Track maximum operational range
- Track drone type

### 📋 Mission Management

- Create new missions
- View pending missions
- View completed missions
- Assign missions to suitable drones
- Complete missions
- Track mission status
- Set mission priority
- Validate mission requirements

### 🔋 Battery Simulation

The system simulates battery consumption based on mission distance.

Example:

```text
Initial Battery = 80%
Mission Distance = 10 km
Consumption Rate = 3% per km

Battery Consumed = 10 × 3 = 30%

Remaining Battery = 80 - 30 = 50%
