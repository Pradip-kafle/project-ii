#include <iostream>
#include <string>
#include <ctime>
#include <fstream>
#include <cstring>
#include <cctype>
#include <conio.h> // For _getch() to hide password
#include <iomanip>

using namespace std;

// Constants
const int MAX_BUSES = 50;
const int MAX_TICKETS = 200;
const int MAX_SEATS = 50;

// Structure to store bus details
struct Bus {
    int busId;
    char busNumber[20];
    char source[50];
    char destination[50];
    char departureTime[10];
    char arrivalTime[10];
    char travelDate[11];
    int totalSeats;
    double ticketPrice;
    bool seatAvailability[MAX_SEATS];
    bool isActive;
};

// Structure to store passenger details
struct Passenger {
    char name[50];
    char contactNumber[15];
    int age;
    char gender[2];
};

// Structure to store ticket details
struct Ticket {
    int ticketId;
    int busId;
    Passenger passenger;
    int seatNumber;
    char bookingDate[30];
    char travelDate[11];
    char source[50];
    char destination[50];
    double fare;
    bool isBooked;
};

// Structure to store bus bill history
struct BusBill {
    int billId;
    int busId;
    char busNumber[20];
    char source[50];
    char destination[50];
    char travelDate[11];
    char departureTime[10];
    char arrivalTime[10];
    int totalSeats;
    double totalRevenue;
    char generatedDate[30];
    bool isActive;
    int passengerCount;
    int passengerIds[MAX_SEATS]; // Store ticket IDs of passengers
};

class BusReservationSystem {
private:
    Bus buses[MAX_BUSES];
    Ticket tickets[MAX_TICKETS];
    BusBill busBills[MAX_BUSES]; // Array to store bus bills
    int busCount;
    int ticketCount;
    int billCount;
    int nextTicketId;
    int nextBusId;
    int nextBillId;
    const char* USERNAME = "admin";
    const char* PASSWORD = "********";
    
    // Get current date and time as string
    void getCurrentDateTime(char* dateTime) {
        time_t now = time(nullptr);
        strcpy(dateTime, ctime(&now));
        // Remove newline
        int len = strlen(dateTime);
        if (len > 0 && dateTime[len-1] == '\n') {
            dateTime[len-1] = '\0';
        }
    }

    // Function to check if a date is valid and not in the past
    bool isValidFutureDate(const char* dateStr) {
        // Check format (DD/MM/YYYY)
        if (strlen(dateStr) != 10 || dateStr[2] != '/' || dateStr[5] != '/') {
            return false;
        }
        
        // Extract day, month, year
        int day = (dateStr[0] - '0') * 10 + (dateStr[1] - '0');
        int month = (dateStr[3] - '0') * 10 + (dateStr[4] - '0');
        int year = (dateStr[6] - '0') * 1000 + (dateStr[7] - '0') * 100 + 
                   (dateStr[8] - '0') * 10 + (dateStr[9] - '0');
        
        // Basic date validation
        if (day < 1 || day > 31 || month < 1 || month > 12 || year < 2023) {
            return false;
        }
        
        // Get current date
        time_t now = time(nullptr);
        struct tm* currentTime = localtime(&now);
        int currentDay = currentTime->tm_mday;
        int currentMonth = currentTime->tm_mon + 1; // tm_mon is 0-based
        int currentYear = currentTime->tm_year + 1900;
        
        // Compare with current date
        if (year < currentYear) {
            return false;
        } else if (year == currentYear) {
            if (month < currentMonth) {
                return false;
            } else if (month == currentMonth) {
                if (day < currentDay) {
                    return false;
                }
            }
        }
        
        return true;
    }

    // String copy function
    void copyString(char* dest, const char* src) {
        strcpy(dest, src);
    }

    // String compare function
    bool compareString(const char* str1, const char* str2) {
        return strcmp(str1, str2) == 0;
    }

    // Count available seats
    int countAvailableSeats(const Bus& bus) {
        int count = 0;
        for (int i = 0; i < bus.totalSeats; i++) {
            if (bus.seatAvailability[i]) {
                count++;
            }
        }
        return count;
    }

    // Find bus by ID
    int findBusById(int busId) {
        for (int i = 0; i < busCount; i++) {
            if (buses[i].busId == busId && buses[i].isActive) {
                return i;
            }
        }
        return -1;
    }

    // Find ticket by ID
    int findTicketById(int ticketId) {
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].ticketId == ticketId && tickets[i].isBooked) {
                return i;
            }
        }
        return -1;
    }

    // Check if bus has active bookings
    bool hasActiveBookings(int busId) {
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].busId == busId && tickets[i].isBooked) {
                return true;
            }
        }
        return false;
    }

    // Clear input buffer
    void clearInputBuffer() {
        cin.clear();
        while (cin.get() != '\n');
    }

    // Check if bus is fully booked
    bool isBusFullyBooked(int busIndex) {
        for (int i = 0; i < buses[busIndex].totalSeats; i++) {
            if (buses[busIndex].seatAvailability[i]) {
                return false;
            }
        }
        return true;
    }

    // Generate bus bill
    void generateBusBill(int busIndex) {
        if (billCount >= MAX_BUSES) {
            cout << "Maximum bill limit reached!\n";
            return;
        }

        Bus& bus = buses[busIndex];
        
        // Calculate total revenue
        double totalRevenue = 0;
        int passengerCount = 0;
        int passengerIds[MAX_SEATS];
        
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].isBooked && tickets[i].busId == bus.busId) {
                totalRevenue += tickets[i].fare;
                passengerIds[passengerCount] = tickets[i].ticketId;
                passengerCount++;
            }
        }
        
        // Create bill
        BusBill newBill;
        newBill.billId = nextBillId++;
        newBill.busId = bus.busId;
        copyString(newBill.busNumber, bus.busNumber);
        copyString(newBill.source, bus.source);
        copyString(newBill.destination, bus.destination);
        copyString(newBill.travelDate, bus.travelDate);
        copyString(newBill.departureTime, bus.departureTime);
        copyString(newBill.arrivalTime, bus.arrivalTime);
        newBill.totalSeats = bus.totalSeats;
        newBill.totalRevenue = totalRevenue;
        getCurrentDateTime(newBill.generatedDate);
        newBill.isActive = true;
        newBill.passengerCount = passengerCount;
        
        // Copy passenger IDs
        for (int i = 0; i < passengerCount; i++) {
            newBill.passengerIds[i] = passengerIds[i];
        }
        
        // Add bill to array
        busBills[billCount++] = newBill;
        
        // Print bill
        cout << "\n========== BUS BILL ==========\n";
        cout << "Bill ID: " << newBill.billId << endl;
        cout << "Bus Number: " << newBill.busNumber << endl;
        cout << "Route: " << newBill.source << " to " << newBill.destination << endl;
        cout << "Travel Date: " << newBill.travelDate << endl;
        cout << "Departure Time: " << newBill.departureTime << endl;
        cout << "Arrival Time: " << newBill.arrivalTime << endl;
        cout << "Total Seats: " << newBill.totalSeats << endl;
        cout << "Total Passengers: " << passengerCount << endl;
        cout << "Total Revenue: " << newBill.totalRevenue << endl;
        cout << "Generated On: " << newBill.generatedDate << endl;
        cout << "================================\n";
        
        // No longer marking bus as inactive
        // bus.isActive = false;
        
        cout << "\nBus has been fully booked.\n";
        cout << "A bill has been generated and stored in history.\n";
    }

public:
    // Clear screen function
    void clearScreen() {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
    }

    // Display decorated header
    void displayHeader(const char* title) {
        int titleLength = strlen(title);
        int totalWidth = 60;
        int padding = (totalWidth - titleLength) / 2;
        
        cout << "\n";
        cout << "+" << string(totalWidth-2, '=') << "+\n";
        cout << "|" << string(padding-1, ' ') << title << string(totalWidth-padding-titleLength-1, ' ') << "|\n";
        cout << "+" << string(totalWidth-2, '=') << "+\n";
    }

    BusReservationSystem() {
        busCount = 0;
        ticketCount = 0;
        billCount = 0;
        nextTicketId = 1001;
        nextBusId = 101;
        nextBillId = 501;
        
        // Initialize all buses as inactive
        for (int i = 0; i < MAX_BUSES; i++) {
            buses[i].isActive = false;
            busBills[i].isActive = false;
        }
        
        loadData(); // Load data from file
    }

    ~BusReservationSystem() {
        saveData(); // Save data when program closes
    }

    // Login function
    bool login() {
        char username[50];
        char password[50];
        char ch;
        int attempts = 0;
        const int MAX_ATTEMPTS = 3;

        while (attempts < MAX_ATTEMPTS) {
            clearScreen();
            displayHeader("LOGIN");
            
            cout << "\nUsername: ";
            cin >> username;
            
            cout << "Password: ";
            // Hide password with asterisks
            int i = 0;
            while (true) {
                ch = _getch();
                if (ch == 13) { // Enter key
                    password[i] = '\0';
                    break;
                } else if (ch == 8) { // Backspace
                    if (i > 0) {
                        i--;
                        cout << "\b \b"; // Move back, erase, move back again
                    }
                } else {
                    password[i++] = ch;
                    cout << "*";
                }
            }
            
            if (compareString(username, USERNAME) && compareString(password, PASSWORD)) {
                clearScreen();
                displayHeader("LOGIN SUCCESSFUL");
                cout << "\nWelcome to Bus Ticket Reservation System!\n";
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
                return true;
            } else {
                attempts++;
                clearScreen();
                displayHeader("LOGIN FAILED");
                cout << "\nIncorrect username or password! " << (MAX_ATTEMPTS - attempts) << " attempts remaining.\n";
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
            }
        }

        clearScreen();
        displayHeader("ACCESS DENIED");
        cout << "\nToo many failed attempts. Program is closing.\n";
        return false;
    }

    // Show menu function
    void showMenu() {
        int choice;
        do {
            clearScreen();
            displayHeader("BUS TICKET RESERVATION SYSTEM");
            
            cout << "\n";
            cout << "============================================\n";
            cout << "[]  1. Add New Bus                         []\n";
            cout << "[]  2. View All Buses                      []\n";
            cout << "[]  3. Search Bus                          []\n";
            cout << "[]  4. Book Ticket                         []\n";
            cout << "[]  5. View Ticket                         []\n";
            cout << "[]  6. Cancel Ticket                       []\n";
            cout << "[]  7. View All Bookings                   []\n";
            cout << "[]  8. Delete Bus Record                   []\n";
            cout << "[]  9. View Bus Bill History               []\n";
            cout << "[]  10. Exit                               []\n";
            cout << "============================================\n";
            cout << "\nYour choice: ";
            
            // Invalid input handling
            if (!(cin >> choice)) {
                clearInputBuffer();
                clearScreen();
                displayHeader("INVALID INPUT");
                cout << "\nPlease enter a number between 1 and 10.\n";
                cout << "Press Enter to continue...";
                cin.ignore();
                cin.get();
                continue;
            }

            switch (choice) {
                case 1:
                    clearScreen();
                    addBus();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 2:
                    clearScreen();
                    viewAllBuses();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 3:
                    clearScreen();
                    searchBus();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 4:
                    clearScreen();
                    bookTicket();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 5:
                    clearScreen();
                    viewTicket();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 6:
                    clearScreen();
                    cancelTicket();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 7:
                    clearScreen();
                    viewAllBookings();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 8:
                    clearScreen();
                    deleteBus();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 9:
                    clearScreen();
                    viewBusBillHistory();
                    cout << "\nPress Enter to continue...";
                    cin.ignore();
                    cin.get();
                    break;
                case 10:
                    clearScreen();
                    displayHeader("THANK YOU");
                    cout << "\nExiting program... Thank you for using our service!\n";
                    break;
                default:
                    clearScreen();
                    displayHeader("INVALID CHOICE");
                    cout << "\nPlease enter a number between 1 and 10.\n";
                    cout << "Press Enter to continue...";
                    cin.ignore();
                    cin.get();
            }
        } while (choice != 0);
    }

    // Add new bus function
    void addBus() {
        if (busCount >= MAX_BUSES) {
            cout << "Maximum bus limit reached!\n";
            return;
        }
        
        Bus newBus;
        clearInputBuffer();
        
        cout << "\n========== ADD NEW BUS ==========\n";
        newBus.busId = nextBusId++;
        
        cout << "Bus Number: ";
        cin.getline(newBus.busNumber, 20);
        
        // Check if bus number already exists
        for (int i = 0; i < busCount; i++) {
            if (compareString(buses[i].busNumber, newBus.busNumber) && buses[i].isActive) {
                cout << "This bus number already exists!\n";
                return;
            }
        }
        
        cout << "Source: ";
        cin.getline(newBus.source, 50);
        
        cout << "Destination: ";
        cin.getline(newBus.destination, 50);

        // Travel date with validation
        bool validDate = false;
        while (!validDate) {
            cout << "Travel Date (DD/MM/YYYY): ";
            cin.getline(newBus.travelDate, 11);
            
            if (isValidFutureDate(newBus.travelDate)) {
                validDate = true;
            } else {
                cout << "Error: Please enter a valid date in DD/MM/YYYY format. The date must be today or a future date.\n";
            }
        }
        
        // Departure time with AM/PM
        char tempTime[10];
        char ampm[3];
        cout << "Departure Time (HH:MM): ";
        cin.getline(tempTime, 10);
        cout << "AM/PM: ";
        cin.getline(ampm, 3);
        
        // Combine time with AM/PM
        snprintf(newBus.departureTime, sizeof(newBus.departureTime), "%s %s", tempTime, ampm);
        
        // Arrival time with AM/PM
        cout << "Arrival Time (HH:MM): ";
        cin.getline(tempTime, 10);
        cout << "AM/PM: ";
        cin.getline(ampm, 3);
        
        // Combine time with AM/PM
        snprintf(newBus.arrivalTime, sizeof(newBus.arrivalTime), "%s %s", tempTime, ampm);
        
        cout << "Total Seats: ";
        cin >> newBus.totalSeats;
        
        if (newBus.totalSeats > MAX_SEATS) {
            cout << "Maximum seat limit is " << MAX_SEATS << "!\n";
            newBus.totalSeats = MAX_SEATS;
        }
        
        cout << "Ticket Price: ";
        cin >> newBus.ticketPrice;
        
        // Initialize all seats as available
        for (int i = 0; i < newBus.totalSeats; i++) {
            newBus.seatAvailability[i] = true;
        }
        
        newBus.isActive = true;
        buses[busCount++] = newBus;
        cout << "\nBus added successfully with ID: " << newBus.busId << "\n";
    }

    // View all buses function
    void viewAllBuses() {
        cout << "\n========== ALL BUSES ==========\n";
        
        bool found = false;
        for (int i = 0; i < busCount; i++) {
            if (buses[i].isActive) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "No buses available.\n";
            return;
        }
        
        cout << "ID    Bus Number    Source          Destination     Travel Date    Departure    Arrival      Total Seats  Available    Price\n";
        cout << "----------------------------------------------------------------------------------------------------------------\n";
        
        for (int i = 0; i < busCount; i++) {
            if (buses[i].isActive) {
                int availableSeats = countAvailableSeats(buses[i]);
                
                printf("%-5d %-13s %-15s %-15s %-14s %-12s %-12s %-12d %-12d %.2f\n", 
                       buses[i].busId, buses[i].busNumber, buses[i].source, buses[i].destination,
                       buses[i].travelDate, buses[i].departureTime, buses[i].arrivalTime, 
                       buses[i].totalSeats, availableSeats, buses[i].ticketPrice);
            }
        }
    }

    // Search bus function
    void searchBus() {
        int choice;
        clearInputBuffer();
        
        cout << "\n========== SEARCH BUS ==========\n";
        cout << "Search by:\n";
        cout << "1. Source and Destination\n";
        cout << "2. Bus Number\n";
        cout << "Your choice: ";
        cin >> choice;
        
        clearInputBuffer();
        
        if (choice == 1) {
            char source[50], destination[50];
            cout << "Enter Source: ";
            cin.getline(source, 50);
            cout << "Enter Destination: ";
            cin.getline(destination, 50);
            
            bool found = false;
            cout << "\n----- Buses from " << source << " to " << destination << " -----\n";
            
            cout << "ID    Bus Number    Departure      Arrival        Available    Price\n";
            cout << "----------------------------------------------------------------\n";
            
            for (int i = 0; i < busCount; i++) {
                if (buses[i].isActive && compareString(buses[i].source, source) && compareString(buses[i].destination, destination)) {
                    found = true;
                    int availableSeats = countAvailableSeats(buses[i]);
                    
                    printf("%-5d %-13s %-15s %-15s %-12d %.2f\n", 
                           buses[i].busId, buses[i].busNumber, buses[i].departureTime, 
                           buses[i].arrivalTime, availableSeats, buses[i].ticketPrice);
                }
            }
            
            if (!found) {
                cout << "No buses found for the specified route.\n";
            }
        } else if (choice == 2) {
            char busNumber[20];
            cout << "Enter Bus Number: ";
            cin.getline(busNumber, 20);
            
            bool found = false;
            
            for (int i = 0; i < busCount; i++) {
                if (buses[i].isActive && compareString(buses[i].busNumber, busNumber)) {
                    found = true;
                    int availableSeats = countAvailableSeats(buses[i]);
                    
                    cout << "\n----- Bus Details -----\n";
                    cout << "Bus ID: " << buses[i].busId << endl;
                    cout << "Bus Number: " << buses[i].busNumber << endl;
                    cout << "Route: " << buses[i].source << " to " << buses[i].destination << endl;
                    cout << "Departure Time: " << buses[i].departureTime << endl;
                    cout << "Arrival Time: " << buses[i].arrivalTime << endl;
                    cout << "Total Seats: " << buses[i].totalSeats << endl;
                    cout << "Available Seats: " << availableSeats << endl;
                    cout << "Ticket Price: " << buses[i].ticketPrice << endl;
                    break;
                }
            }
            
            if (!found) {
                cout << "Bus with number " << busNumber << " not found.\n";
            }
        } else {
            cout << "Invalid choice!\n";
        }
    }

    // Book ticket function
    void bookTicket() {
        clearScreen();
        displayHeader("BOOK TICKET");
        
        int busId;
        
        // Show all buses
        viewAllBuses();
        
        bool found = false;
        for (int i = 0; i < busCount; i++) {
            if (buses[i].isActive) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "\nNo buses available. Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        // Get travel details first
        clearInputBuffer();
        cout << "\n----- Travel Details -----\n";
        
        // Travel date with validation
        bool validDate = false;
        char requestedDate[11];
        
        while (!validDate) {
            cout << "Travel Date (DD/MM/YYYY): ";
            cin.getline(requestedDate, 11);
            
            if (isValidFutureDate(requestedDate)) {
                validDate = true;
            } else {
                cout << "Error: Please enter a valid date in DD/MM/YYYY format. The date must be today or a future date.\n";
            }
        }
        
        cout << "Source: ";
        char requestedSource[50];
        cin.getline(requestedSource, 50);
        
        cout << "Destination: ";
        char requestedDestination[50];
        cin.getline(requestedDestination, 50);
        
        // Show available buses for the requested date and route
        cout << "\nAvailable Buses for " << requestedDate << " from " << requestedSource << " to " << requestedDestination << ":\n";
        
        cout << "+------+-------------+-----------+-----------+-----------+--------+\n";
        cout << "| ID   | Bus Number  | Departure | Arrival   | Available | Price  |\n";
        cout << "+------+-------------+-----------+-----------+-----------+--------+\n";
        
        bool busesFound = false;
        for (int i = 0; i < busCount; i++) {
            if (buses[i].isActive && 
                compareString(buses[i].travelDate, requestedDate) &&
                compareString(buses[i].source, requestedSource) &&
                compareString(buses[i].destination, requestedDestination)) {
                
                busesFound = true;
                int availableSeats = countAvailableSeats(buses[i]);
                
                printf("| %-4d | %-11s | %-9s | %-9s | %-9d | %-6.2f |\n", 
                       buses[i].busId, buses[i].busNumber, buses[i].departureTime, 
                       buses[i].arrivalTime, availableSeats, buses[i].ticketPrice);
                
                cout << "+------+-------------+-----------+-----------+-----------+--------+\n";
            }
        }
        
        if (!busesFound) {
            cout << "No buses available for the specified date and route.\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        // Now ask for bus ID
        cout << "\nEnter Bus ID: ";
        cin >> busId;
        
        // Find the bus
        int busIndex = findBusById(busId);
        
        if (busIndex == -1) {
            cout << "Bus with ID " << busId << " not found.\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        // Verify that the bus matches the requested details
        if (!compareString(buses[busIndex].travelDate, requestedDate) ||
            !compareString(buses[busIndex].source, requestedSource) ||
            !compareString(buses[busIndex].destination, requestedDestination)) {
            cout << "Selected bus does not match the requested travel details.\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        Bus& selectedBus = buses[busIndex];
        
        // Check if seats are available
        int availableSeats = countAvailableSeats(selectedBus);
        if (availableSeats == 0) {
            cout << "Sorry, no seats available for this bus.\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        // Show seat layout
        cout << "\n----- Seat Layout -----\n";
        cout << "Available: O | Booked: X\n\n";
        
        for (int i = 0; i < selectedBus.totalSeats; i++) {
            printf("%3d%s  ", (i + 1), (selectedBus.seatAvailability[i] ? " O" : " X"));
            if ((i + 1) % 4 == 0) cout << endl;
        }
        cout << endl;
        
        // Get seat number
        int seatNumber;
        cout << "Enter Seat Number (1-" << selectedBus.totalSeats << "): ";
        cin >> seatNumber;
        
        if (seatNumber < 1 || seatNumber > selectedBus.totalSeats) {
            cout << "Invalid seat number!\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        // Check if seat is available
        if (!selectedBus.seatAvailability[seatNumber - 1]) {
            cout << "Seat " << seatNumber << " is already booked!\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        // Get passenger details
        Passenger passenger;
        clearInputBuffer();
        
        cout << "\n----- Passenger Details -----\n";
        cout << "Name: ";
        cin.getline(passenger.name, 50);
        
        cout << "Contact Number: ";
        cin.getline(passenger.contactNumber, 15);
        
        cout << "Age: ";
        cin >> passenger.age;
        
        clearInputBuffer();
        cout << "Gender (M/F): ";
        cin.getline(passenger.gender, 2);
        
        // Create ticket
        if (ticketCount >= MAX_TICKETS) {
            cout << "Maximum ticket limit reached!\n";
            cout << "Press Enter to return to main menu...";
            cin.ignore();
            cin.get();
            return;
        }
        
        Ticket newTicket;
        newTicket.ticketId = nextTicketId++;
        newTicket.busId = busId;
        newTicket.passenger = passenger;
        newTicket.seatNumber = seatNumber;
        getCurrentDateTime(newTicket.bookingDate);
        newTicket.fare = selectedBus.ticketPrice;
        newTicket.isBooked = true;
        
        // Store travel details in ticket
        copyString(newTicket.travelDate, requestedDate);
        copyString(newTicket.source, requestedSource);
        copyString(newTicket.destination, requestedDestination);
        
        // Mark seat as booked
        selectedBus.seatAvailability[seatNumber - 1] = false;
        
        // Add ticket to array
        tickets[ticketCount++] = newTicket;
        
        // Print ticket in a nice format
        clearScreen();
        displayHeader("TICKET BOOKED SUCCESSFULLY");
        
        cout << "\n+------------------------------------------+\n";
        cout << "|           TICKET DETAILS                 |\n";
        cout << "+------------------------------------------+\n";
        cout << "| Ticket ID      : " << setw(23) << left << newTicket.ticketId << "|\n";
        cout << "| Bus Number     : " << setw(23) << left << selectedBus.busNumber << "|\n";
        cout << "| Travel Date    : " << setw(23) << left << newTicket.travelDate << "|\n";
        cout << "| From           : " << setw(23) << left << newTicket.source << "|\n";
        cout << "| To             : " << setw(23) << left << newTicket.destination << "|\n";
        cout << "| Departure Time : " << setw(23) << left << selectedBus.departureTime << "|\n";
        cout << "+------------------------------------------+\n";
        cout << "|           PASSENGER DETAILS              |\n";
        cout << "+------------------------------------------+\n";
        cout << "| Name           : " << setw(23) << left << passenger.name << "|\n";
        cout << "| Seat Number    : " << setw(23) << left << seatNumber << "|\n";
        cout << "| Fare           : Rs. " << setw(20) << left << newTicket.fare << "|\n";
        cout << "| Booking Date   : " << setw(23) << left << newTicket.bookingDate << "|\n";
        cout << "+------------------------------------------+\n";
        
        cout << "\nPlease note down your Ticket ID for future reference: " << newTicket.ticketId << "\n";
        
        // Check if bus is fully booked
        if (isBusFullyBooked(busIndex)) {
            generateBusBill(busIndex);
        }
    }

    // View ticket function
    void viewTicket() {
        clearScreen();
        displayHeader("VIEW TICKET DETAILS");
        
        int ticketId;
        
        cout << "\nEnter Ticket ID: ";
        cin >> ticketId;
        
        int ticketIndex = findTicketById(ticketId);
        
        if (ticketIndex == -1) {
            cout << "\nTicket with ID " << ticketId << " not found or has been cancelled.\n";
            return;
        }
        
        const Ticket& ticket = tickets[ticketIndex];
        
        // Find the bus
        int busIndex = findBusById(ticket.busId);
        
        if (busIndex == -1) {
            cout << "\nBus information not found for this ticket.\n";
            return;
        }
        
        const Bus& bus = buses[busIndex];
        
        // Print ticket details
        cout << "\n+------------------------------------------+\n";
        cout << "|           TICKET DETAILS                 |\n";
        cout << "+------------------------------------------+\n";
        cout << "| Ticket ID      : " << setw(23) << left << ticket.ticketId << "|\n";
        cout << "| Bus Number     : " << setw(23) << left << bus.busNumber << "|\n";
        cout << "| Travel Date    : " << setw(23) << left << ticket.travelDate << "|\n";
        cout << "| From           : " << setw(23) << left << ticket.source << "|\n";
        cout << "| To             : " << setw(23) << left << ticket.destination << "|\n";
        cout << "| Departure Time : " << setw(23) << left << bus.departureTime << "|\n";
        cout << "+------------------------------------------+\n";
        cout << "|           PASSENGER DETAILS              |\n";
        cout << "+------------------------------------------+\n";
        cout << "| Name           : " << setw(23) << left << ticket.passenger.name << "|\n";
        cout << "| Contact        : " << setw(23) << left << ticket.passenger.contactNumber << "|\n";
        cout << "| Age            : " << setw(23) << left << ticket.passenger.age << "|\n";
        cout << "| Gender         : " << setw(23) << left << ticket.passenger.gender << "|\n";
        cout << "+------------------------------------------+\n";
        cout << "|           BOOKING DETAILS                |\n";
        cout << "+------------------------------------------+\n";
        cout << "| Seat Number    : " << setw(23) << left << ticket.seatNumber << "|\n";
        cout << "| Fare           : Rs. " << setw(20) << left << ticket.fare << "|\n";
        cout << "| Booking Date   : " << setw(23) << left << ticket.bookingDate << "|\n";
        cout << "| Status         : " << setw(23) << left << (ticket.isBooked ? "Active" : "Cancelled") << "|\n";
        cout << "+------------------------------------------+\n";
    }

    // Cancel ticket function
    void cancelTicket() {
        int ticketId;
        
        cout << "\n========== CANCEL TICKET ==========\n";
        cout << "Enter Ticket ID: ";
        cin >> ticketId;
        
        int ticketIndex = findTicketById(ticketId);
        
        if (ticketIndex == -1) {
            cout << "Ticket with ID " << ticketId << " not found or has already been cancelled.\n";
            return;
        }
        
        Ticket& ticket = tickets[ticketIndex];
        
        // Find the bus
        int busIndex = findBusById(ticket.busId);
        
        if (busIndex == -1) {
            cout << "Bus information not found for this ticket.\n";
            return;
        }
        
        Bus& bus = buses[busIndex];
        
        // Mark seat as available
        bus.seatAvailability[ticket.seatNumber - 1] = true;
        
        // Mark ticket as cancelled
        ticket.isBooked = false;
        
        cout << "\nTicket with ID " << ticketId << " has been cancelled successfully.\n";
        cout << "Refund amount: " << ticket.fare << endl;
    }

    // View all bookings function
    void viewAllBookings() {
        clearScreen();
        displayHeader("ALL BOOKINGS");
        
        bool found = false;
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].isBooked) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "\nNo bookings found.\n";
            return;
        }
        
        cout << "\n+----------+----------+--------------------+--------------+---------------+---------------+---------+----------+\n";
        cout << "| Ticket ID|  Bus ID  |  Passenger Name    | Travel Date  |    Source     |  Destination  | Seat No.|  Status  |\n";
        cout << "+----------+----------+--------------------+--------------+---------------+---------------+---------+----------+\n";
        
        for (int i = 0; i < ticketCount; i++) {
            if (tickets[i].isBooked || !tickets[i].isBooked) { // Show both active and cancelled tickets
                const Ticket& ticket = tickets[i];
                const char* status = ticket.isBooked ? "Active" : "Cancelled";
                
                printf("| %-8d | %-8d | %-18s | %-12s | %-13s | %-13s | %-7d | %-8s |\n", 
                       ticket.ticketId, ticket.busId, ticket.passenger.name, 
                       ticket.travelDate, ticket.source, ticket.destination, 
                       ticket.seatNumber, status);
                
                cout << "+----------+----------+--------------------+--------------+---------------+---------------+---------+----------+\n";
            }
        }
    }

    // Delete bus function
    void deleteBus() {
        int busId;
        
        cout << "\n========== DELETE BUS RECORD ==========\n";
        viewAllBuses();
        
        bool found = false;
        for (int i = 0; i < busCount; i++) {
            if (buses[i].isActive) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            return;
        }
        
        cout << "\nEnter Bus ID to delete: ";
        cin >> busId;
        
        // Find the bus
        int busIndex = findBusById(busId);
        
        if (busIndex == -1) {
            cout << "Bus with ID " << busId << " not found.\n";
            return;
        }
        
        // Check if there are active bookings for this bus
        bool hasBookings = hasActiveBookings(busId);
        bool isFullyBooked = isBusFullyBooked(busIndex);
        
        if (hasBookings && !isFullyBooked) {
            cout << "Cannot delete bus with active bookings that is not fully booked. Please cancel all tickets first.\n";
            return;
        }
        
        // Confirm deletion
        char confirm;
        cout << "Are you sure you want to delete Bus " << buses[busIndex].busNumber 
             << " (" << buses[busIndex].source << " to " << buses[busIndex].destination << ")? (y/n): ";
        cin >> confirm;
        
        if (tolower(confirm) == 'y') {
            // If bus is fully booked and not already in bill history, generate a bill
            if (isFullyBooked) {
                bool billExists = false;
                for (int i = 0; i < billCount; i++) {
                    if (busBills[i].isActive && busBills[i].busId == busId) {
                        billExists = true;
                        break;
                    }
                }
                
                if (!billExists) {
                    generateBusBill(busIndex);
                }
            }
            
            // Mark bus as inactive
            buses[busIndex].isActive = false;
            cout << "Bus record deleted successfully.\n";
        } else {
            cout << "Deletion cancelled.\n";
        }
    }

    // View bus bill history function
    void viewBusBillHistory() {
        clearScreen();
        displayHeader("BUS BILL HISTORY");
        
        bool found = false;
        for (int i = 0; i < billCount; i++) {
            if (busBills[i].isActive) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            cout << "\nNo bus bills found.\n";
            return;
        }
        
        for (int i = 0; i < billCount; i++) {
            if (busBills[i].isActive) {
                cout << "\n+---------------------------------------------------------------+\n";
                cout << "|                              BUS BILL " << setw(4) << left << busBills[i].billId << "                       |\n";
                cout << "+-------------------------------------------------------------------------+\n";
                cout << "| Bus ID         : " << setw(42) << left << busBills[i].busId << "        |\n";
                cout << "| Bus Number     : " << setw(42) << left << busBills[i].busNumber << "    |\n";
                cout << "| Route          : " << setw(20) << left << busBills[i].source << " to " << setw(19) << left << busBills[i].destination << "|\n";
                cout << "| Travel Date    : " << setw(42) << left << busBills[i].travelDate << "   |\n";
                cout << "| Departure Time : " << setw(42) << left << busBills[i].departureTime << "|\n";
                cout << "| Arrival Time   : " << setw(42) << left << busBills[i].arrivalTime << "  |\n";
                cout << "| Total Seats    : " << setw(42) << left << busBills[i].totalSeats << "   |\n";
                cout << "| Total Revenue  : Rs. " << setw(39) << left << fixed << setprecision(2) << busBills[i].totalRevenue << "|\n";
                cout << "| Generated On   : " << setw(42) << left << busBills[i].generatedDate << "|\n";
                cout << "+-------------------------------------------------------------------------+\n";
                cout << "|                     PASSENGER DETAILS                         |\n";
                cout << "+-------------------------------------------------------------------------+\n";
                cout << "| " << setw(25) << left << "Name" << setw(20) << left << "Contact" << setw(10) << left << "Seat No." << "|\n";
                cout << "+-------------------------------------------------------------------------+\n";
                
                for (int j = 0; j < busBills[i].passengerCount; j++) {
                    int ticketId = busBills[i].passengerIds[j];
                    
                    // Find the ticket
                    for (int k = 0; k < ticketCount; k++) {
                        if (tickets[k].ticketId == ticketId) {
                            cout << "| " << setw(25) << left << tickets[k].passenger.name 
                                 << setw(20) << left << tickets[k].passenger.contactNumber 
                                 << setw(10) << left << tickets[k].seatNumber << "|\n";
                            break;
                        }
                    }
                }
                
                cout << "+------------------------------------------------------------------------+\n";
                cout << "| Total Passengers: " << setw(5) << left << busBills[i].passengerCount 
                     << "            Total Revenue: Rs. " << setw(10) << left << fixed << setprecision(2) << busBills[i].totalRevenue << "|\n";
                cout << "+------------------------------------------------------------------------+\n\n";
            }
        }
    }

    // Save data to file function
    void saveData() {
        // Save buses
        ofstream busFile("buses.dat", ios::binary);
        if (busFile.is_open()) {
            busFile.write(reinterpret_cast<char*>(&busCount), sizeof(busCount));
            busFile.write(reinterpret_cast<char*>(&nextBusId), sizeof(nextBusId));
            
            for (int i = 0; i < busCount; i++) {
                busFile.write(reinterpret_cast<char*>(&buses[i]), sizeof(Bus));
            }
            busFile.close();
        }
        
        // Save tickets
        ofstream ticketFile("tickets.dat", ios::binary);
        if (ticketFile.is_open()) {
            ticketFile.write(reinterpret_cast<char*>(&ticketCount), sizeof(ticketCount));
            ticketFile.write(reinterpret_cast<char*>(&nextTicketId), sizeof(nextTicketId));
            
            for (int i = 0; i < ticketCount; i++) {
                ticketFile.write(reinterpret_cast<char*>(&tickets[i]), sizeof(Ticket));
            }
            ticketFile.close();
        }
        
        // Save bus bills
        ofstream billFile("busbills.dat", ios::binary);
        if (billFile.is_open()) {
            billFile.write(reinterpret_cast<char*>(&billCount), sizeof(billCount));
            billFile.write(reinterpret_cast<char*>(&nextBillId), sizeof(nextBillId));
            
            for (int i = 0; i < billCount; i++) {
                billFile.write(reinterpret_cast<char*>(&busBills[i]), sizeof(BusBill));
            }
            billFile.close();
        }
    }

    // Load data from file function
    void loadData() {
        // Load buses
        ifstream busFile("buses.dat", ios::binary);
        if (busFile.is_open()) {
            busFile.read(reinterpret_cast<char*>(&busCount), sizeof(busCount));
            busFile.read(reinterpret_cast<char*>(&nextBusId), sizeof(nextBusId));
            
            for (int i = 0; i < busCount; i++) {
                busFile.read(reinterpret_cast<char*>(&buses[i]), sizeof(Bus));
            }
            busFile.close();
        }
        
        // Load tickets
        ifstream ticketFile("tickets.dat", ios::binary);
        if (ticketFile.is_open()) {
            ticketFile.read(reinterpret_cast<char*>(&ticketCount), sizeof(ticketCount));
            ticketFile.read(reinterpret_cast<char*>(&nextTicketId), sizeof(nextTicketId));
            
            for (int i = 0; i < ticketCount; i++) {
                ticketFile.read(reinterpret_cast<char*>(&tickets[i]), sizeof(Ticket));
            }
            ticketFile.close();
        }
        
        // Load bus bills
        ifstream billFile("busbills.dat", ios::binary);
        if (billFile.is_open()) {
            billFile.read(reinterpret_cast<char*>(&billCount), sizeof(billCount));
            billFile.read(reinterpret_cast<char*>(&nextBillId), sizeof(nextBillId));
            
            for (int i = 0; i < billCount; i++) {
                billFile.read(reinterpret_cast<char*>(&busBills[i]), sizeof(BusBill));
            }
            billFile.close();
        }
    }
};

int main() {
    BusReservationSystem busSystem;
    
    busSystem.clearScreen();
    busSystem.displayHeader("BUS TICKET RESERVATION SYSTEM");
    cout << "\nPress Enter to continue...";
    cin.get();
    
    if (busSystem.login()) {
        busSystem.showMenu();
    }
    
    return 0;
} 