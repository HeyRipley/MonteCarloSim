#include <vector>
#include <utility>    // For std::pair
#include <stdexcept>  // For error handling (optional)
#include <iostream>   // For std::cout, std::cerr
#include <cmath>      // For fmax, pow, sqrt, log, cos
#include <numeric>    // For std::iota (optional)
#include <algorithm>  // For std::find_if (optional, using manual loop)
#include <cstdio>     // For printf
#include <cstdlib>    // For std::rand, std::srand
#include <ctime>      // For std::time

// --- Control Flags for Printing ---
const bool PRINT_PER_RUN_SUMMARY = true;  // <<< Set true to see results for each start year
const bool PRINT_YEARLY_DETAILS = true; // <<< Set true for EXTREME detail (every year of every run)

// --- Historical Data (from your provided code, potentially corrected) ---
const std::vector<std::pair<int, double>> historical_sp500_price_returns = { // Renamed for clarity
    {1928, 0.3788}, {1929, -0.1191}, {1930, -0.2848}, {1931, -0.4707},
    {1932, -0.1515}, {1933, 0.4659}, {1934, -0.0594}, {1935, 0.4137},
    {1936, 0.2792}, {1937, -0.3859}, {1938, 0.2521}, {1939, -0.0535},
    {1940, -0.1529}, {1941, -0.1788}, {1942, 0.1247}, {1943, 0.1908},
    {1944, 0.1329}, {1945, 0.3072}, {1946, -0.1197}, {1947, 0.00},
    {1948, -0.0065}, {1949, 0.1026}, {1950, 0.2175}, {1951, 0.1646},
    {1952, 0.1178}, {1953, -0.0662}, {1954, 0.4502}, {1955, 0.2639},
    {1956, 0.0261}, {1957, -0.1431}, {1958, 0.3806}, {1959, 0.0848},
    {1960, -0.0300}, {1961, 0.2313}, {1962, -0.1181}, {1963, 0.1889},
    {1964, 0.1296}, {1965, 0.0906}, {1966, -0.1309}, {1967, 0.1999},
    {1968, 0.0766}, {1969, -0.1136}, {1970, -0.0009}, {1971, 0.1079},
    {1972, 0.1563}, {1973, -0.1737}, {1974, -0.2972}, {1975, 0.3155},
    {1976, 0.1915}, {1977, -0.1150}, {1978, 0.0106}, {1979, 0.1231},
    {1980, 0.2577}, {1981, -0.0973}, {1982, 0.1476}, {1983, 0.1727},
    {1984, 0.0140}, {1985, 0.2633}, {1986, 0.1462}, {1987, 0.0203},
    {1988, 0.1240}, {1989, 0.2725}, {1990, -0.0656}, {1991, 0.2631},
    {1992, 0.0446}, {1993, 0.0706}, {1994, -0.0154}, {1995, 0.3411},
    {1996, 0.2026}, {1997, 0.3101}, {1998, 0.2667}, {1999, 0.1953},
    {2000, -0.1014}, {2001, -0.1304}, {2002, -0.2337}, {2003, 0.2638},
    {2004, 0.0899}, {2005, 0.0300}, {2006, 0.1362}, {2007, 0.0353},
    {2008, -0.3849}, {2009, 0.2345}, {2010, 0.1278}, {2011, 0.00},
    {2012, 0.1341}, {2013, 0.2960}, {2014, 0.1139}, {2015, -0.0073},
    {2016, 0.0954}, {2017, 0.1942}, {2018, -0.0624}, {2019, 0.2888},
    {2020, 0.1626}, {2021, 0.2689}, {2022, -0.1944}, {2023, 0.2423},
    {2024, 0.1124}, {2025, -0.1393} // Note: 2025 data is speculative if added manually
};

// --- Constants and Helper Functions ---
#define TIME_HORIZON 40 // Default simulation duration unless overridden in main

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int find_start_index(int start_year, const std::vector<std::pair<int, double>>& data) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i].first == start_year) {
            return i;
        }
    }
    return -1; // Not found
}

// --- Modified Backtesting Function ---
double run_backtest(
    int start_simulation_year,
    int simulation_length_years,
    const std::vector<std::pair<int, double>>& historical_price_data,
    // --- Simulation Parameters ---
    double initial_savings,
    double work_income_start_param,
    double work_income_end,
    int current_age,
    int retirement_age,
    double annual_spending_start,
    double inflation_rate,
    int social_security_age,
    double social_security_start,
    double cola_rate
) {
    int data_start_index = find_start_index(start_simulation_year, historical_price_data);

    if (data_start_index == -1) { return -1e18; }

    double savings = initial_savings;
    double annual_spending = annual_spending_start;
    const double FLAT_DIVIDEND_YIELD = 0.03;

    if (PRINT_YEARLY_DETAILS) { // Print header if printing details
        printf("--- Backtest Start Year: %d ---\n", start_simulation_year);
        printf("Year | Hist Yr | Age | Start Sav | Price Rtn | Div Inc | Spend | SS | Work Inc | End Sav\n");
        printf("-------------------------------------------------------------------------------------------\n");
    }


    for (int year_offset = 0; year_offset < simulation_length_years; ++year_offset) {
        int current_data_index = data_start_index + year_offset;
        int age = current_age + year_offset;
        int historical_year = start_simulation_year + year_offset;

        if (current_data_index >= historical_price_data.size()) {
            if (PRINT_YEARLY_DETAILS) printf("Error: Ran out of data at historical year %d\n", historical_year);
            return -1e17;
        }

        double start_of_year_savings = savings; // Store for printing if needed
        double price_return = historical_price_data[current_data_index].second;
        double dividend_income = savings * FLAT_DIVIDEND_YIELD;
        double savings_after_price_change = savings * (1.0 + price_return);

        // --- Calculate Income (Work + SS) ---
        double current_work_income = 0.0;
        double retirement_income_target = work_income_end;
        if (age >= retirement_age) { current_work_income = retirement_income_target; }
        else if (age >= current_age) {
            int total_working_years = retirement_age - current_age;
            if (total_working_years > 0) {
                double working_progress = (double)(age - current_age) / total_working_years;
                current_work_income = work_income_start_param + working_progress * (retirement_income_target - work_income_start_param);
                current_work_income = fmax(retirement_income_target, current_work_income);
            }
            else { current_work_income = retirement_income_target; }
        }
        else { current_work_income = work_income_start_param; }

        double social_security = 0.0;
        if (age >= social_security_age) {
            social_security = social_security_start * pow((1.0 + cola_rate), (age - social_security_age));
        }

        // --- Adjust Spending ---
        double previous_spending = annual_spending; // Store before modification
        if (age % 5 == 0 && age >= social_security_age) { annual_spending *= 0.98; }
        if (age < social_security_age) { annual_spending *= (1.0 + inflation_rate); }
        double current_annual_spending = annual_spending;

        // --- Update Savings ---
        savings = savings_after_price_change + dividend_income - current_annual_spending + social_security + current_work_income;

        // --- Optional Detailed Yearly Print ---
        if (PRINT_YEARLY_DETAILS) {
            printf("%4d | %7d | %3d | %10.0f | %9.2f%% | %8.0f | %7.0f | %4.0f | %9.0f | %10.0f\n",
                year_offset + 1, historical_year, age, start_of_year_savings, price_return * 100.0,
                dividend_income, current_annual_spending, social_security, current_work_income, savings);
        }

        // --- Optional Early Exit ---
        // if (savings < 0.0 && year_offset < simulation_length_years - 1) {
        //     if (PRINT_YEARLY_DETAILS) printf("Note: Savings depleted early.\n");
        //     return savings;
        // }

    } // End yearly loop

    if (PRINT_YEARLY_DETAILS) { // Print footer if printing details
        printf("-------------------------------------------------------------------------------------------\n");
        printf("End of Backtest for Start Year %d. Final Savings: %.2f\n\n", start_simulation_year, savings);
    }

    return savings;
}


// --- Main Function for Comprehensive Backtesting ---
int main() {
    std::srand(std::time(nullptr));

    // --- Define Simulation Parameters ---
    // !!! Use your actual plan numbers !!!
    double initial_savings = 25000;
    double work_income_start = 125000;
    double work_income_end = 0;
    int current_age = 27;
    int retirement_age = 55;
    double annual_spending_start = 75000;
    double inflation_rate = 0.04;
    int social_security_age = 67;
    double social_security_start = 34000;
    double cola_rate = 0.02;
    int simulation_duration = TIME_HORIZON;

    // --- Data Checks ---
    if (historical_sp500_price_returns.empty()) {
        std::cerr << "Error: Historical price data vector is empty." << std::endl; return 1;
    }
    if (historical_sp500_price_returns.size() < simulation_duration) {
        std::cerr << "Error: Not enough historical data points (" << historical_sp500_price_returns.size()
            << ") for the simulation duration (" << simulation_duration << ")." << std::endl; return 1;
    }

    // --- Comprehensive Backtesting Loop ---
    int success_count = 0;
    int total_runs = 0;
    int data_error_runs = 0;
    int start_year_error_runs = 0; // Should be 0 with this loop structure

    int first_possible_start_year = historical_sp500_price_returns[0].first;
    int last_possible_start_year_index = historical_sp500_price_returns.size() - simulation_duration;
    int last_possible_start_year = historical_sp500_price_returns[last_possible_start_year_index].first;

    std::cout << "--- Running Comprehensive Backtest ---" << std::endl;
    std::cout << "Data Range Available: " << historical_sp500_price_returns[0].first << " - " << historical_sp500_price_returns.back().first << std::endl;
    std::cout << "Simulation Duration: " << simulation_duration << " years" << std::endl;
    std::cout << "Testing All Possible Start Years: " << first_possible_start_year << " to " << last_possible_start_year << std::endl;
    std::cout << "Dividend Yield Assumption: Flat 3.00%" << std::endl;
    printf("Parameters: InitSav $%.0f, StartSpend $%.0f, RetAge %d, SSStart $%.0f@%d, Infl %.2f%%\n",
        initial_savings, annual_spending_start, retirement_age, social_security_start, social_security_age, inflation_rate * 100.0);
    std::cout << "------------------------------------" << std::endl;
    if (!PRINT_PER_RUN_SUMMARY && !PRINT_YEARLY_DETAILS) { // Only print if not showing details later
        std::cout << "Running..." << std::endl;
    }


    for (int start_year = first_possible_start_year; start_year <= last_possible_start_year; ++start_year) {

        double final_savings = run_backtest(
            start_year, simulation_duration, historical_sp500_price_returns,
            initial_savings, work_income_start, work_income_end, current_age, retirement_age,
            annual_spending_start, inflation_rate, social_security_age, social_security_start, cola_rate
        );

        if (final_savings <= -1e18) {
            start_year_error_runs++; // Should not happen if loop is correct
        }
        else if (final_savings <= -1e17) {
            data_error_runs++;
            if (PRINT_PER_RUN_SUMMARY) { // Print error if showing per-run results
                printf("Start Year: %d | Error: Insufficient data length for full run.\n", start_year);
            }
        }
        else {
            total_runs++; // Only count runs that completed fully
            bool success = (final_savings >= 0.0);
            if (success) {
                success_count++;
            }
            // --- Print Per-Run Summary ---
            if (PRINT_PER_RUN_SUMMARY) {
                printf("Start Year: %d | Final Savings: %12.0f | Success: %s\n", // Formatted output
                    start_year, final_savings, (success ? "Yes" : " No"));
            }
        }

    } // End loop through start years

    // --- Calculate and Print Final Summary ---
    if (total_runs > 0) {
        double success_rate = static_cast<double>(success_count) / total_runs * 100.0;
        std::cout << "\n--- Overall Backtesting Summary ---" << std::endl;
        std::cout << "Total Historical Periods Tested (" << simulation_duration << " years each): " << total_runs << std::endl;
        std::cout << "Successful Periods (Final Savings >= 0): " << success_count << std::endl;
        printf("Overall Success Rate: %.2f%%\n", success_rate);
        if (data_error_runs > 0 || start_year_error_runs > 0) {
            printf("Note: %d run(s) stopped early due to insufficient data length, %d failed on start year lookup.\n",
                data_error_runs, start_year_error_runs);
        }
        std::cout << "----------------------------------" << std::endl;
    }
    else {
        std::cout << "\nNo valid backtest runs were completed." << std::endl;
        if (data_error_runs > 0 || start_year_error_runs > 0) {
            printf("Note: %d run(s) stopped early due to insufficient data length, %d failed on start year lookup.\n",
                data_error_runs, start_year_error_runs);
        }
    }

    return 0;
}