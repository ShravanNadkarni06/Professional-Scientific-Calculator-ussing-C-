#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <cmath>
#include <limits>
#include <vector>

using namespace std;

const string GREEN_COLOR = "\033[32m";
const string RED_COLOR = "\033[31m";
const string YELLOW_COLOR = "\033[33m";
const string CYAN_COLOR = "\033[36m";
const string BLUE_COLOR = "\033[34m";
const string RESET_COLOR = "\033[0m";

struct HistoryEntry {
    string type, input, result;
    HistoryEntry(string t, string i, string r) : type(t), input(i), result(r) {}
};

class Converter {
protected:
    double value;
public:
    Converter(double val = 0.0) : value(val) {}
    virtual ~Converter() = default;
    virtual double convert() const = 0;
    virtual string getType() const = 0;
    void displayResult(const string& input, const string& unitInfo) const {
        const int COL_WIDTH = 15;
        double result = convert();
        cout << fixed << setprecision(2);
        ostringstream oss;
        oss << fixed << setprecision(2) << result;
        string resultStr = oss.str();

        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        cout << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + "Input" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + unitInfo + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << GREEN_COLOR + resultStr + RESET_COLOR << "|\n";
        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
    }
};

class TemperatureConverter : public Converter {
private:
    char fromUnit, toUnit;
public:
    TemperatureConverter(double temp, char from, char to)
        : Converter(temp), fromUnit(toupper(from)), toUnit(toupper(to)) {}

    double convert() const override {
        // Step-by-step temperature conversion process
        // Step 1: Check if converting from Celsius to Fahrenheit
        if (fromUnit == 'C' && toUnit == 'F') {
            // Step 2: Apply the formula: °F = (°C * 9/5) + 32
            //         - Multiply the Celsius value by 9/5
            //         - Add 32 to the result
            return value * 9.0 / 5.0 + 32.0;
        }
        // Step 3: Check if converting from Fahrenheit to Celsius
        if (fromUnit == 'F' && toUnit == 'C') {
            // Step 4: Apply the formula: °C = (°F - 32) * 5/9
            //         - Subtract 32 from the Fahrenheit value
            //         - Multiply the result by 5/9
            return (value - 32.0) * 5.0 / 9.0;
        }
        // Step 5: If units are the same, return the original value
        if (fromUnit == toUnit) {
            return value;
        }
        // Step 6: If units are invalid, throw an error
        throw runtime_error("Invalid temperature units (use C or F)");
    }
    string getType() const override { return "Temperature"; }
};

class NumberBaseConverter : public Converter {
private:
    string inputValue;
    char fromBase, toBase;

    string convertToBase(long long num, int base) const {
        // Step-by-step process to convert a decimal number to the target base
        // Step 1: Handle the special case where the number is 0
        if (num == 0) return "0";
        // Step 2: Define the digits for the target base (0-9, A-F for hex)
        string digits = "0123456789ABCDEF";
        string result;
        // Step 3: Convert the number to the target base using division
        //         - Repeatedly divide the number by the base
        //         - Prepend the remainder (as a digit) to the result
        //         - Continue until the number becomes 0
        while (num > 0) {
            result = digits[num % base] + result;
            num /= base;
        }
        // Step 4: Return the converted string
        return result;
    }

public:
    NumberBaseConverter(string val, char from, char to)
        : Converter(0), inputValue(val), fromBase(toupper(from)), toBase(toupper(to)) {
        try {
            // Step-by-step process to parse the input string
            // Step 1: Check the source base and convert the string to a decimal number
            if (fromBase == 'B') {
                // Step 2: For binary, use base 2 to parse the string
                value = static_cast<double>(stoi(val, nullptr, 2));
            }
            else if (fromBase == 'D') {
                // Step 3: For decimal, parse directly as a double
                value = stod(val);
            }
            else if (fromBase == 'O') {
                // Step 4: For octal, use base 8 to parse the string
                value = static_cast<double>(stoi(val, nullptr, 8));
            }
            else if (fromBase == 'H') {
                // Step 5: For hexadecimal, use base 16 to parse the string
                value = static_cast<double>(stoi(val, nullptr, 16));
            }
            else {
                // Step 6: If the source base is invalid, throw an error
                throw runtime_error("Invalid source base (use B, D, O, H)");
            }
        } catch (const invalid_argument& e) {
            throw runtime_error("Invalid number format for the specified base");
        } catch (const out_of_range& e) {
            throw runtime_error("Number out of range for conversion");
        }
    }

    double convert() const override {
        // Step-by-step process to convert the decimal value to the target base
        // Step 1: Convert the double value to a long long integer (ignoring decimal part)
        long long intValue = static_cast<long long>(value);
        // Step 2: Check the target base and convert accordingly
        if (toBase == 'B') {
            // Step 3: For binary, convert to base 2 and parse the result as a double
            return stod(convertToBase(intValue, 2));
        }
        if (toBase == 'D') {
            // Step 4: For decimal, return the value as is
            return value;
        }
        if (toBase == 'O') {
            // Step 5: For octal, convert to base 8 and parse the result as a double
            return stod(convertToBase(intValue, 8));
        }
        if (toBase == 'H') {
            // Step 6: For hexadecimal, convert to base 16 and parse the result as a double
            return stod(convertToBase(intValue, 16));
        }
        // Step 7: If the target base is invalid, throw an error
        throw runtime_error("Invalid target base (use B, D, O, H)");
    }

    string getResultString() const {
        // Step-by-step process to get the result as a string
        // Step 1: Convert the double value to a long long integer
        long long intValue = static_cast<long long>(value);
        // Step 2: Check the target base and convert to a string
        if (toBase == 'B') {
            // Step 3: For binary, convert to base 2
            return convertToBase(intValue, 2);
        }
        if (toBase == 'D') {
            // Step 4: For decimal, convert to string directly
            return to_string(intValue);
        }
        if (toBase == 'O') {
            // Step 5: For octal, convert to base 8
            return convertToBase(intValue, 8);
        }
        if (toBase == 'H') {
            // Step 6: For hexadecimal, convert to base 16
            return convertToBase(intValue, 16);
        }
        // Step 7: If the target base is invalid, throw an error
        throw runtime_error("Invalid target base");
    }

    void displayResult(const string& input, const string& unitInfo) const {
        const int COL_WIDTH = 15;
        string result = getResultString();

        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        cout << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + "Input" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + unitInfo + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << GREEN_COLOR + result + RESET_COLOR << "|\n";
        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
    }

    string getType() const override { return "Number Base"; }
};

class LogarithmicCalculator : public Converter {
private:
    char logType;
public:
    LogarithmicCalculator(double val, char type) : Converter(val), logType(toupper(type)) {}

    double convert() const override {
        // Step-by-step logarithmic calculation process
        // Step 1: Check if the input value is positive (log is undefined for non-positive numbers)
        if (value <= 0) {
            throw runtime_error("Logarithm undefined for non-positive numbers");
        }
        // Step 2: Check the type of logarithm to compute
        if (logType == 'L') {
            // Step 3: For base 10 logarithm, use log10() function
            return log10(value);
        }
        if (logType == 'N') {
            // Step 4: For natural logarithm (base e), use log() function
            return log(value);
        }
        if (logType == 'B') {
            // Step 5: For base 2 logarithm, use log2() function
            return log2(value);
        }
        // Step 6: If the log type is invalid, throw an error
        throw runtime_error("Invalid log type (use L, N, B)");
    }
    string getType() const override { return "Logarithm"; }
};

class CurrencyConverter : public Converter {
private:
    char fromCurrency, toCurrency;
    static constexpr double INR_TO_USD = 0.012;
    static constexpr double USD_TO_INR = 83.33;
    static constexpr double USD_TO_EUR = 0.92;
    static constexpr double USD_TO_GBP = 0.79;
    static constexpr double EUR_TO_USD = 1.09;
    static constexpr double GBP_TO_USD = 1.27;
public:
    CurrencyConverter(double amount, char from, char to)
        : Converter(amount), fromCurrency(toupper(from)), toCurrency(toupper(to)) {}

    double convert() const override {
        // Step-by-step currency conversion process
        // Step 1: Check if converting from INR to USD
        if (fromCurrency == 'I' && toCurrency == 'U') {
            // Step 2: Multiply the amount by the INR to USD rate
            return value * INR_TO_USD;
        }
        // Step 3: Check if converting from USD to INR
        if (fromCurrency == 'U' && toCurrency == 'I') {
            // Step 4: Multiply the amount by the USD to INR rate
            return value * USD_TO_INR;
        }
        // Step 5: Check if converting from USD to EUR
        if (fromCurrency == 'U' && toCurrency == 'E') {
            // Step 6: Multiply the amount by the USD to EUR rate
            return value * USD_TO_EUR;
        }
        // Step 7: Check if converting from USD to GBP
        if (fromCurrency == 'U' && toCurrency == 'G') {
            // Step 8: Multiply the amount by the USD to GBP rate
            return value * USD_TO_GBP;
        }
        // Step 9: Check if converting from EUR to USD
        if (fromCurrency == 'E' && toCurrency == 'U') {
            // Step 10: Multiply the amount by the EUR to USD rate
            return value * EUR_TO_USD;
        }
        // Step 11: Check if converting from GBP to USD
        if (fromCurrency == 'G' && toCurrency == 'U') {
            // Step 12: Multiply the amount by the GBP to USD rate
            return value * GBP_TO_USD;
        }
        // Step 13: If the currencies are the same, return the original value
        if (fromCurrency == toCurrency) {
            return value;
        }
        // Step 14: If the currency pair is invalid, throw an error
        throw runtime_error("Invalid or unsupported currency (use I, U, E, G)");
    }
    string getType() const override { return "Currency"; }
};

class LengthConverter : public Converter {
private:
    char fromUnit, toUnit;
    static constexpr double M_TO_FT = 3.28084;
    static constexpr double FT_TO_M = 0.3048;
public:
    LengthConverter(double length, char from, char to)
        : Converter(length), fromUnit(toupper(from)), toUnit(toupper(to)) {}

    double convert() const override {
        // Step-by-step length conversion process
        // Step 1: Check if converting from meters to feet
        if (fromUnit == 'M' && toUnit == 'F') {
            // Step 2: Multiply the length by the meters to feet conversion factor
            return value * M_TO_FT;
        }
        // Step 3: Check if converting from feet to meters
        if (fromUnit == 'F' && toUnit == 'M') {
            // Step 4: Multiply the length by the feet to meters conversion factor
            return value * FT_TO_M;
        }
        // Step 5: If units are the same, return the original value
        if (fromUnit == toUnit) {
            return value;
        }
        // Step 6: If units are invalid, throw an error
        throw runtime_error("Invalid length units (use M or F)");
    }
    string getType() const override { return "Length"; }
};

class Calculator {
private:
    double num1, num2;
    char operation;
public:
    Calculator(double n1, char op, double n2 = 0.0) : num1(n1), num2(n2), operation(toupper(op)) {}

    double calculate() const {
        // Step-by-step calculation process based on the operation
        switch (operation) {
            case '+':
                // Step 1: For addition, add num1 and num2
                return num1 + num2;
            case '-':
                // Step 2: For subtraction, subtract num2 from num1
                return num1 - num2;
            case '*':
                // Step 3: For multiplication, multiply num1 by num2
                return num1 * num2;
            case '/':
                // Step 4: For division, check if num2 is zero
                if (num2 == 0) {
                    // Step 5: If num2 is zero, throw an error
                    throw runtime_error("Division by zero");
                }
                // Step 6: Divide num1 by num2
                return num1 / num2;
            case '^':
                // Step 7: For exponentiation, raise num1 to the power of num2
                return pow(num1, num2);
            case 'S':
                // Step 8: For sine, convert num1 from degrees to radians
                //         - Multiply num1 by π/180
                // Step 9: Compute the sine of the angle
                return sin(num1 * M_PI / 180.0);
            case 'C':
                // Step 10: For cosine, convert num1 from degrees to radians
                //          - Multiply num1 by π/180
                // Step 11: Compute the cosine of the angle
                return cos(num1 * M_PI / 180.0);
            case 'T':
                // Step 12: For tangent, convert num1 from degrees to radians
                //          - Multiply num1 by π/180
                // Step 13: Check if cosine is zero (tan is undefined at 90°, 270°, etc.)
                if (cos(num1 * M_PI / 180.0) == 0) {
                    // Step 14: If cosine is zero, throw an error
                    throw runtime_error("Tan undefined");
                }
                // Step 15: Compute the tangent of the angle
                return tan(num1 * M_PI / 180.0);
            default:
                // Step 16: If the operation is invalid, throw an error
                throw runtime_error("Invalid operation (use +, -, *, /, ^, S, C, T)");
        }
    }

    void displayResult() const {
        try {
            const int COL_WIDTH = 15;
            double result = calculate();
            cout << fixed << setprecision(2);
            ostringstream oss;
            oss << fixed << setprecision(2) << num1 << " " << operation << (operation == 'S' || operation == 'C' || operation == 'T' ? "" : " " + to_string(num2));
            string inputStr = oss.str();
            oss.str(""); oss << fixed << setprecision(2) << result;
            string resultStr = oss.str();

            cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
            cout << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + "Input" + RESET_COLOR
                 << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + inputStr + RESET_COLOR
                 << "|" << setw(COL_WIDTH) << left << GREEN_COLOR + resultStr + RESET_COLOR << "|\n";
            cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        } catch (const runtime_error& e) {
            const int COL_WIDTH = 45;
            cout << "+" << string(COL_WIDTH, '-') << "+\n";
            cout << "|" << setw(COL_WIDTH) << left << RED_COLOR + string("Error: ") + e.what() + RESET_COLOR << "|\n";
            cout << "+" << string(COL_WIDTH, '-') << "+\n";
        }
    }
};

class Program {
private:
    vector<HistoryEntry> history;

    void showWelcome() const {
        const int COL_WIDTH = 40;
        cout << "+" << string(COL_WIDTH, '-') << "+\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Welcome to the Professional Converter!" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Advanced conversion and calculation tool" + RESET_COLOR << "|\n";
        cout << "+" << string(COL_WIDTH, '-') << "+\n";
    }

    void clearInputBuffer() const {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    double getDoubleInput(const string& prompt) const {
        double input;
        cout << YELLOW_COLOR << prompt << RESET_COLOR;
        while (!(cin >> input)) {
            cout << RED_COLOR << "Invalid input. " << RESET_COLOR << YELLOW_COLOR << prompt << RESET_COLOR;
            clearInputBuffer();
        }
        return input;
    }

    char getCharInput(const string& prompt) const {
        char input;
        cout << YELLOW_COLOR << prompt << RESET_COLOR;
        cin >> input;
        clearInputBuffer();
        return input;
    }

    string getStringInput(const string& prompt) const {
        string input;
        cout << YELLOW_COLOR << prompt << RESET_COLOR;
        cin >> input;
        clearInputBuffer();
        return input;
    }

    void displayMenu() const {
        const int COL_WIDTH = 25;
        cout << "\n+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Option" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Description" + RESET_COLOR << "|\n";
        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "1" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Calculator" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "2" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Temperature (C/F)" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "3" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Number Base (B/D/O/H)" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "4" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Logarithm (L/N/B)" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "5" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Currency (I/U/E/G)" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "6" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Length (M/F)" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "7" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "View History" + RESET_COLOR << "|\n";
        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "8" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Quit" + RESET_COLOR << "|\n";
        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
    }

    void displayHistory() const {
        const int COL_WIDTH = 15;
        if (history.empty()) {
            cout << YELLOW_COLOR << "No history available." << RESET_COLOR << endl;
            return;
        }
        cout << "\n+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        cout << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + "Type" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + "Input" + RESET_COLOR
             << "|" << setw(COL_WIDTH) << left << BLUE_COLOR + "Result" + RESET_COLOR << "|\n";
        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
        for (const auto& entry : history) {
            cout << "|" << setw(COL_WIDTH) << left << entry.type
                 << "|" << setw(COL_WIDTH) << left << entry.input
                 << "|" << setw(COL_WIDTH) << left << GREEN_COLOR + entry.result + RESET_COLOR << "|\n";
        }
        cout << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+" << string(COL_WIDTH, '-') << "+\n";
    }

public:
    void run() {
        showWelcome();

        while (true) {
            displayMenu();
            int choice = static_cast<int>(getDoubleInput("Enter choice (1-8): "));
            clearInputBuffer();

            try {
                switch (choice) {
                    case 1: {
                        double num1 = getDoubleInput("Enter first number: ");
                        char op = getCharInput("Enter operation (+, -, *, /, ^, S(sin), C(cos), T(tan)): ");
                        if (op != 'S' && op != 'C' && op != 'T') {
                            double num2 = getDoubleInput("Enter second number: ");
                            Calculator calc(num1, op, num2);
                            calc.displayResult();
                            ostringstream oss;
                            oss << fixed << setprecision(2) << num1 << " " << op << " " << num2;
                            string inputStr = oss.str();
                            oss.str(""); oss << fixed << setprecision(2) << calc.calculate();
                            history.emplace_back("Calculator", inputStr, oss.str());
                        } else {
                            Calculator calc(num1, op);
                            calc.displayResult();
                            ostringstream oss;
                            oss << fixed << setprecision(2) << num1 << " " << op;
                            string inputStr = oss.str();
                            oss.str(""); oss << fixed << setprecision(2) << calc.calculate();
                            history.emplace_back("Calculator", inputStr, oss.str());
                        }
                        break;
                    }
                    case 2: {
                        double temp = getDoubleInput("Enter temperature: ");
                        char from = getCharInput("Enter from unit (C or F): ");
                        char to = getCharInput("Enter to unit (C or F): ");
                        TemperatureConverter tempConv(temp, from, to);
                        ostringstream oss;
                        oss << fixed << setprecision(2) << temp;
                        string inputStr = oss.str();
                        tempConv.displayResult(inputStr, string(1, toupper(from)) + " to " + string(1, toupper(to)));
                        oss.str(""); oss << fixed << setprecision(2) << tempConv.convert();
                        history.emplace_back(tempConv.getType(), inputStr + " " + string(1, toupper(from)) + " to " + string(1, toupper(to)), oss.str());
                        break;
                    }
                    case 3: {
                        string number = getStringInput("Enter number: ");
                        char from = getCharInput("Enter from base (B(binary), D(decimal), O(octal), H(hex)): ");
                        char to = getCharInput("Enter to base (B, D, O, H): ");
                        NumberBaseConverter baseConv(number, from, to);
                        baseConv.displayResult(number, string(1, toupper(from)) + " to " + string(1, toupper(to)));
                        history.emplace_back(baseConv.getType(), number + " " + string(1, toupper(from)) + " to " + string(1, toupper(to)), baseConv.getResultString());
                        break;
                    }
                    case 4: {
                        double num = getDoubleInput("Enter number: ");
                        char type = getCharInput("Enter log type (L=log10, N=ln, B=log2): ");
                        LogarithmicCalculator logCalc(num, type);
                        ostringstream oss;
                        oss << fixed << setprecision(2) << num;
                        string inputStr = oss.str();
                        string logStr = (type == 'L' ? "log10" : type == 'N' ? "ln" : "log2");
                        logCalc.displayResult(inputStr, logStr);
                        oss.str(""); oss << fixed << setprecision(2) << logCalc.convert();
                        history.emplace_back(logCalc.getType(), logStr + "(" + inputStr + ")", oss.str());
                        break;
                    }
                    case 5: {
                        double amount = getDoubleInput("Enter amount: ");
                        char from = getCharInput("Enter from currency (I, U, E, or G): ");
                        char to = getCharInput("Enter to currency (I, U, E, or G): ");
                        CurrencyConverter currConv(amount, from, to);
                        ostringstream oss;
                        oss << fixed << setprecision(2) << amount;
                        string inputStr = oss.str();
                        currConv.displayResult(inputStr, string(1, toupper(from)) + " to " + string(1, toupper(to)));
                        oss.str(""); oss << fixed << setprecision(2) << currConv.convert();
                        history.emplace_back(currConv.getType(), inputStr + " " + string(1, toupper(from)) + " to " + string(1, toupper(to)), oss.str());
                        break;
                    }
                    case 6: {
                        double length = getDoubleInput("Enter length: ");
                        char from = getCharInput("Enter from unit (M or F): ");
                        char to = getCharInput("Enter to unit (M or F): ");
                        LengthConverter lenConv(length, from, to);
                        ostringstream oss;
                        oss << fixed << setprecision(2) << length;
                        string inputStr = oss.str();
                        lenConv.displayResult(inputStr, string(1, toupper(from)) + " to " + string(1, toupper(to)));
                        oss.str(""); oss << fixed << setprecision(2) << lenConv.convert();
                        history.emplace_back(lenConv.getType(), inputStr + " " + string(1, toupper(from)) + " to " + string(1, toupper(to)), oss.str());
                        break;
                    }
                    case 7:
                        displayHistory();
                        break;
                    case 8: {
                        const int COL_WIDTH = 40;
                        cout << "+" << string(COL_WIDTH, '-') << "+\n";
                        cout << "|" << setw(COL_WIDTH) << left << CYAN_COLOR + "Thank you for using Professional Converter!" + RESET_COLOR << "|\n";
                        cout << "+" << string(COL_WIDTH, '-') << "+\n";
                        return;
                    }
                    default:
                        const int COL_WIDTH = 40;
                        cout << "+" << string(COL_WIDTH, '-') << "+\n";
                        cout << "|" << setw(COL_WIDTH) << left << RED_COLOR + "Invalid choice. Please select 1-8." + RESET_COLOR << "|\n";
                        cout << "+" << string(COL_WIDTH, '-') << "+\n";
                }
            } catch (const runtime_error& e) {
                const int COL_WIDTH = 45;
                cout << "+" << string(COL_WIDTH, '-') << "+\n";
                cout << "|" << setw(COL_WIDTH) << left << RED_COLOR + string("Error: ") + e.what() + RESET_COLOR << "|\n";
                cout << "+" << string(COL_WIDTH, '-') << "+\n";
            }
        }
    }
};

int main() {
    Program program;
    program.run();
    return 0;
}
