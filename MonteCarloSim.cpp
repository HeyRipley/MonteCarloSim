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
#include <iomanip> // For std::setw, std::setprecision
#include <sstream>
#include <locale>

// --- Control Flags for Printing ---
const bool PRINT_PER_RUN_SUMMARY = true;  // <<< Set true to see results for each start year
const bool PRINT_YEARLY_DETAILS = true  ; // <<< Set true for EXTREME detail (every year of every run)

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
    {2024, 0.1124}
};

// --- Conservative speculative data for years after 2024 (2025-2054) ---
// Based on historical patterns with deeper bear market periods for realism
const std::vector<std::pair<int, double>> speculative_sp500_returns = {
    // 2025-2029: Post-inflation stabilization period (moderate growth, like 1980s recovery)
    {2025, 0.0850},  // AI boost, tech recovery
    {2026, 0.0950},  // Continued momentum
    {2027, 0.0650},  // Normalizing growth
    {2028, 0.0750},  // Steady expansion
    {2029, 0.0550},  // Slight caution, valuation concerns

    // 2030-2042: Mature growth cycle with mid-cycle correction
    {2030, 0.0900},  // Recovery year
    {2031, 0.1050},  // Strong growth
    {2032, 0.0700},  // Moderate growth
    {2033, -0.1800}, // DEEPER CORRECTION: Geopolitical tension or credit event
    {2034, -0.0950}, // Bear market continues
    {2035, 0.2200},  // Strong recovery
    {2036, 0.1100},  // Sustained recovery
    {2037, 0.0850},  // Continued growth
    {2038, 0.0750},  // Sustained expansion
    {2039, 0.0950},  // Acceleration
    {2040, 0.1100},  // Strong growth
    {2041, 0.0850},  // Continued growth
    {2042, 0.0700},  // Normalizing

    // 2043-2054: Mature cycle with late-cycle weakness
    {2043, 0.0900},  // Strong growth
    {2044, 0.0600},  // Moderation
    {2045, 0.0750},  // Moderate growth
    {2046, 0.0850},  // Solid growth
    {2047, -0.2400}, // DEEPER BEAR MARKET: Major correction
    {2048, -0.1200}, // Bear market deepens
    {2049, 0.2800},  // Strong recovery
    {2050, 0.1550},  // Continued recovery momentum
    {2051, 0.0750},  // Growth continues
    {2052, 0.0650},  // Normalizing
    {2053, 0.0800},  // Sustained
    {2054, 0.0550},   // Cautious outlook
    // 2055-2064: Final decade - normalized maturity with cyclical patterns
       {2055, 0.0750 },  // Steady growth
       {2056, 0.0900},  // Acceleration
       {2057, 0.0650},  // Moderation
       {2058, -0.1100}, // Minor correction
       {2059, 0.1850},  // Recovery
       {2060, 0.0800},  // Sustained growth
       {2061, 0.0950},  // Strong performance
       {2062, 0.0700},  // Normalizing
       {2063, 0.0600},  // Moderation
       {2064, 0.0550}   // Cautious outlook
};
const std::vector<std::pair<int, const char*>> sp500_return_reasons = {
    {1928, "Roaring 20s bull market"}, {1929, "Stock market crash"}, {1930, "Great Depression"}, {1931, "Great Depression worsens"},
    {1932, "Depression bottom"}, {1933, "New Deal recovery"}, {1934, "Recovery stalls"}, {1935, "Recovery continues"},
    {1936, "Pre-war growth"}, {1937, "Recession"}, {1938, "Recovery"}, {1939, "Pre-WWII uncertainty"},
    {1940, "WWII begins"}, {1941, "US enters WWII"}, {1942, "WWII spending"}, {1943, "WWII boom"},
    {1944, "WWII boom"}, {1945, "War ends"}, {1946, "Post-war adjustment"}, {1947, "Post-war inflation"},
    {1948, "Mild recession"}, {1949, "Recovery"}, {1950, "Korean War"}, {1951, "Korean War spending"},
    {1952, "Eisenhower elected"}, {1953, "Mild recession"}, {1954, "Recovery"}, {1955, "Boom years"},
    {1956, "Suez crisis"}, {1957, "Recession"}, {1958, "Recovery"}, {1959, "Growth"},
    {1960, "Mild recession"}, {1961, "Kennedy optimism"}, {1962, "Cuban missile crisis"}, {1963, "Growth"},
    {1964, "Growth"}, {1965, "Vietnam escalation"}, {1966, "Tight money"}, {1967, "Recovery"},
    {1968, "Vietnam, inflation"}, {1969, "Tight money"}, {1970, "Mild recession"}, {1971, "Nixon shock"},
    {1972, "Pre-Watergate"}, {1973, "Oil crisis"}, {1974, "Stagflation"}, {1975, "Recovery"},
    {1976, "Growth"}, {1977, "Stagflation"}, {1978, "Stagflation"}, {1979, "Oil shock"},
    {1980, "Recession, Volcker"}, {1981, "Recession"}, {1982, "Recovery begins"}, {1983, "Reagan boom"},
    {1984, "Growth"}, {1985, "Bull market"}, {1986, "Bull market"}, {1987, "Black Monday"},
    {1988, "Recovery"}, {1989, "Bull market"}, {1990, "Gulf War recession"}, {1991, "Recovery"},
    {1992, "Growth"}, {1993, "Growth"}, {1994, "Bond market crash"}, {1995, "Tech boom"},
    {1996, "Tech boom"}, {1997, "Tech boom"}, {1998, "LTCM crisis"}, {1999, "Tech bubble"},
    {2000, "Dot-com bust"}, {2001, "9/11, recession"}, {2002, "Bear market"}, {2003, "Recovery"},
    {2004, "Growth"}, {2005, "Growth"}, {2006, "Housing bubble"}, {2007, "Pre-crisis"},
    {2008, "Financial crisis"}, {2009, "Recovery"}, {2010, "Recovery"}, {2011, "Euro crisis"},
    {2012, "Recovery"}, {2013, "Bull market"}, {2014, "Bull market"}, {2015, "China slowdown"},
    {2016, "Trump election"}, {2017, "Bull market"}, {2018, "Trade war"}, {2019, "Bull market"},
    {2020, "COVID crash/recovery"}, {2021, "COVID stimulus"}, {2022, "Inflation, rate hikes"}, {2023, "Recovery, AI rally"},
    {2024, "Market growth"}, {2025, "Growth + AI rally"}, {2026, "Tech momentum"}, {2027, "Stable growth"},
    {2028, "Expansion"}, {2029, "Moderation"}, {2030, "Recovery"}, {2031, "Strong growth"},
    {2032, "Sustained growth"}, {2033, "Market correction"}, {2034, "Bear market"}, {2035, "Recovery begins"},
    {2036, "Strong recovery"}, {2037, "Expansion"}, {2038, "Growth"}, {2039, "Acceleration"},
    {2040, "Bull market"}, {2041, "Growth continues"}, {2042, "Normalization"}, {2043, "Strong growth"},
    {2044, "Moderation"}, {2045, "Steady growth"}, {2046, "Solid growth"}, {2047, "Market correction"},
    {2048, "Bear market"}, {2049, "Recovery rally"}, {2050, "Recovery momentum"}, {2051, "Growth"},
    {2052, "Normalization"}, {2053, "Expansion"}, {2054, "Cautious outlook"},{2055, "Steady growth"},
    {2056, "Acceleration"}, {2057, "Moderation"}, {2058, "Minor correction"}, {2059, "Recovery"},
    {2060, "Sustained growth"}, {2061, "Strong performance"}, {2062, "Normalizing"}, {2063, "Moderation"},
    {2064, "Cautious outlook"}
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

double get_stock_return(int year, const std::vector<std::pair<int, double>>& historical_data, const std::vector<std::pair<int, double>>& speculative_data) {
    // Try to find in historical data first
    for (const auto& p : historical_data) {
        if (p.first == year) {
            return p.second;
        }
    }
    // Fall back to speculative data for years after 2024
    for (const auto& p : speculative_data) {
        if (p.first == year) {
            return p.second;
        }
    }
    return 0.0; // Return 0 if year not found
}

const char* get_return_reason(int year) {
    for (const auto& p : sp500_return_reasons) {
        if (p.first == year) return p.second;
    }
    return "";
}

std::string format_usd(double value) {
    std::stringstream ss;
    ss.imbue(std::locale("en_US.UTF-8")); // Use US locale for commas
    ss << std::fixed << std::setprecision(0) << value;
    std::string s = ss.str();
    // Add $ sign
    return "$" + s;
}

std::string get_retirement_phase(int age) {
    if (age < 67) {
        return "Pre-Ret";
    }
    else if (age >= 67 && age < 75) {
        return "Go-Go";
    }
    else if (age >= 75 && age < 85) {
        return "Slow-Go";
    }
    else {
        return "No-Go";
    }
}

// --- Struct to hold yearly simulation data ---
struct YearlyData {
    int year;
    int age;
    double start_savings;
    double blended_return;
    double dividend_income;
    double required_spending;
    double actual_spending;
    double portfolio_percent_spent;
    double social_security;
    double work_income;
    double available_after_reserve;
    double end_savings;
    double net_change;
    bool is_failure_year;
};

// --- Backtesting Function with Target End Savings ---
struct BacktestResult {
    double final_savings;
    int failure_year_count;
    std::vector<YearlyData> yearly_data;
};

BacktestResult run_backtest_with_target(
    int start_simulation_year,
    int simulation_length_years,
    const std::vector<std::pair<int, double>>& historical_price_data,
    const std::vector<std::pair<int, double>>& speculative_price_data,
    double initial_savings,
    double work_income_start_param,
    double work_income_end,
    int current_age,
    int retirement_age,
    double annual_spending_start,
    double inflation_rate,
    int social_security_age,
    double social_security_start,
    double cola_rate,
    double stock_allocation,
    double bond_allocation,
    double cash_allocation,
    double bond_annual_return,
    double cash_annual_return,
    double target_end_savings
) {
    BacktestResult result;
    result.failure_year_count = 0;

    int data_start_index = find_start_index(start_simulation_year, historical_price_data);

    if (data_start_index == -1) {
        result.final_savings = -1e18;
        return result;
    }

    const double FLAT_DIVIDEND_YIELD = 0.02;
    const double FAILURE_THRESHOLD = 0.75;  // Spending is 25% less than required

    // --- Spending Phase Parameters (applied after age 67) ---
    // Phase 1: Ages 67-75 (Early retirement - "Go-go years")
    const int PHASE1_AGE_START = 67;
    const int PHASE1_AGE_END = 75;
    const double PHASE1_INFLATION_RATE = inflation_rate;  // Full inflation + COLA
    
    // Phase 2: Ages 75-85 (Mid retirement - Reduced spending)
    const int PHASE2_AGE_START = 75;
    const int PHASE2_AGE_END = 85;
    const double PHASE2_INFLATION_RATE = inflation_rate * 0.5;  // Half inflation, no COLA
    
    // Phase 3: Ages 85+ (Late retirement - Minimal growth)
    const int PHASE3_AGE_START = 85;
    const double PHASE3_INFLATION_RATE = 0.0;  // Flat spending

    // --- SINGLE PASS: Run simulation year by year ---
    double savings = initial_savings;
    double annual_spending = annual_spending_start;

    if (PRINT_YEARLY_DETAILS) {
        printf("--- Backtest Start Year: %d | Target End Savings: $%.0f ---\n", 
               start_simulation_year, target_end_savings);
        printf("Year | Age | Start Sav | Return | Dividend | Required | Available | Actual | %% Port | Fail | End Sav\n");
        printf("-----------------------------------------------------------------------------------------------------\n");
    }

    for (int year_offset = 0; year_offset < simulation_length_years; ++year_offset) {
        int age = current_age + year_offset;
        int historical_year = start_simulation_year + year_offset;

        double start_of_year_savings = savings;
        
        double stock_return = get_stock_return(historical_year, historical_price_data, speculative_price_data);
        double blended_return = (stock_allocation * stock_return) + 
                                (bond_allocation * bond_annual_return) + 
                                (cash_allocation * cash_annual_return);
        
        double dividend_income = savings * FLAT_DIVIDEND_YIELD;
        double savings_after_price_change = savings * (1.0 + blended_return);

        double current_work_income = 0.0;
        if (age >= retirement_age) { current_work_income = work_income_end; }
        else if (age >= current_age) {
            int total_working_years = retirement_age - current_age;
            if (total_working_years > 0) {
                double working_progress = (double)(age - current_age) / total_working_years;
                current_work_income = work_income_start_param + working_progress * (work_income_end - work_income_start_param);
                current_work_income = fmax(work_income_end, current_work_income);
            }
            else { current_work_income = work_income_end; }
        }
        else { current_work_income = work_income_start_param; }

        double social_security = 0.0;
        if (age >= social_security_age) {
            social_security = social_security_start * pow((1.0 + cola_rate), (age - social_security_age));
        }

        // --- Apply spending adjustments based on life phase ---
        double phase_inflation_rate = inflation_rate;  // Default (pre-67)
        
        if (age >= PHASE3_AGE_START) {
            // Phase 3: Ages 85+ - Flat spending
            phase_inflation_rate = PHASE3_INFLATION_RATE;
        }
        else if (age >= PHASE2_AGE_START) {
            // Phase 2: Ages 75-85 - Reduced growth
            phase_inflation_rate = PHASE2_INFLATION_RATE;
        }
        else if (age >= PHASE1_AGE_START) {
            // Phase 1: Ages 67-75 - Full inflation
            phase_inflation_rate = PHASE1_INFLATION_RATE;
        }

        // Apply inflation/growth adjustment
        annual_spending *= (1.0 + phase_inflation_rate);

        double required_spending = annual_spending;
        
        // Calculate available funds this year (after growth but before spending)
        double available_for_spending = savings_after_price_change + dividend_income + social_security + current_work_income;
        
        // Determine actual spending based on what's available and what's required
        // Also ensure we maintain target end savings by year end
        double remaining_years = simulation_length_years - year_offset;
        double required_end_balance = target_end_savings;
        
        // Conservative estimate: reserve enough to reach target assuming 2% annual growth
        double minimum_to_reserve = required_end_balance / pow(1.02, remaining_years);
        double available_after_reserve = available_for_spending - minimum_to_reserve;
        
        double actual_spending = std::min(required_spending, available_after_reserve);
        actual_spending = std::max(0.0, actual_spending);  // Can't spend negative
        
        // Check for failure year: actual spending is 25% less than required
        bool is_failure_year = (actual_spending < required_spending * FAILURE_THRESHOLD);
        if (is_failure_year) {
            result.failure_year_count++;
        }

        double portfolio_percent_spent = (start_of_year_savings > 0) ? (actual_spending / start_of_year_savings) : 0.0;

        // Update savings with actual spending
        savings = savings_after_price_change + dividend_income - actual_spending + social_security + current_work_income;
        
        double net_change = savings - start_of_year_savings;

        YearlyData yd;
        yd.year = historical_year;
        yd.age = age;
        yd.start_savings = start_of_year_savings;
        yd.blended_return = blended_return;
        yd.dividend_income = dividend_income;
        yd.required_spending = required_spending;
        yd.actual_spending = actual_spending;
        yd.portfolio_percent_spent = portfolio_percent_spent;
        yd.social_security = social_security;
        yd.work_income = current_work_income;
        yd.available_after_reserve = available_after_reserve;
        yd.end_savings = savings;
        yd.net_change = net_change;
        yd.is_failure_year = is_failure_year;
        result.yearly_data.push_back(yd);

        if (PRINT_YEARLY_DETAILS) {
            if (year_offset == 0) {
                // Print header once at the start
                printf("\n");
                printf("%-6s %-4s %-8s %-14s %-10s %-12s %-12s %-12s %-12s %-12s %-12s %-14s %-14s %-12s %-8s\n",
                    "Year", "Age", "Phase", "Begin Balance", "Blend Ret", "Dividend", "Work Inc", "SS Income", "Required Sp", "Actual Spend", "% of Port", "Avail Resrv", "End Balance", "Net Change", "Status");
                printf("%-6s %-4s %-8s %-14s %-10s %-12s %-12s %-12s %-12s %-12s %-12s %-14s %-14s %-12s %-8s\n",
                    "------", "---", "--------", "--------------", "----------", "=========", "========", "--------", "-----------", "-----------", "---------", "-----------", "--------------", "----------", "--------");
            }
            
            // Print data row
            std::string phase = get_retirement_phase(age);
            std::string status = is_failure_year ? "FAIL" : "OK";
            
            printf("%-6d %-4d %-8s %13.0f %9.2f%% %11.0f %11.0f %11.0f %11.0f %11.0f %11.1f%% %13.0f %13.0f %+11.0f %-8s\n",
                historical_year,
                age,
                phase.c_str(),
                start_of_year_savings,
                blended_return * 100.0,
                dividend_income,
                current_work_income,
                social_security,
                required_spending,
                actual_spending,
                portfolio_percent_spent * 100.0,
                available_after_reserve,
                savings,
                net_change,
                status.c_str());
        }
    }

    if (PRINT_YEARLY_DETAILS) {
        printf("%-6s %-4s %-8s %-14s %-10s %-12s %-12s %-12s %-12s %-12s %-12s %-14s %-14s %-12s %-8s\n",
            "======", "===", "========", "==============", "==========", "=========", "========", "========", "===========", "===========", "=========", "===========", "==============", "==========", "========");
        printf("\nSummary: Start Year %d | Final Balance: $%.0f | Target: $%.0f | Failure Years: %d\n\n", 
               start_simulation_year, savings, target_end_savings, result.failure_year_count);
    }

    result.final_savings = savings;
    return result;
}

// --- Main Function for Comprehensive Backtesting ---
int main() {
    std::srand(std::time(nullptr));

    // --- Define Simulation Parameters ---
    double initial_savings = 1500000;
    double work_income_start = 130000;
    double work_income_end = 0;
    int current_age = 50;
    int retirement_age = 55;
    double annual_spending_start = 85000;
    double inflation_rate = 0.0324;
    int social_security_age = 67;
    double social_security_start = 34000;
    double cola_rate = 0.02;
    int simulation_duration = TIME_HORIZON;
    double target_end_savings = 0;

    // --- Asset Allocation Parameters ---
    double stock_allocation = 0.60;
    double bond_allocation = 0.20;
    double cash_allocation = 0.20;
    double bond_annual_return = 0.04;
    double cash_annual_return = 0.03;

    // --- Data Checks ---
    if (historical_sp500_price_returns.empty()) {
        std::cerr << "Error: Historical price data vector is empty." << std::endl; return 1;
    }

    // --- Comprehensive Backtesting Loop ---
    int success_count = 0;
    int total_runs = 0;
    int failure_years_total = 0;

    int first_possible_start_year = historical_sp500_price_returns[0].first;
    int last_possible_start_year_index = historical_sp500_price_returns.size() - 1;
    int last_possible_start_year = historical_sp500_price_returns[last_possible_start_year_index].first;

    std::cout << "--- Running Comprehensive Backtest with Target End Savings ---" << std::endl;
    std::cout << "Data Range: " << historical_sp500_price_returns[0].first << " - " << historical_sp500_price_returns.back().first << std::endl;
    std::cout << "Speculative Data: " << speculative_sp500_returns[0].first << " - " << speculative_sp500_returns.back().first << std::endl;
    std::cout << "Simulation Duration: " << simulation_duration << " years" << std::endl;
    std::cout << "Asset Allocation: " << (stock_allocation * 100.0) << "% Stocks / " 
              << (bond_allocation * 100.0) << "% Bonds / " << (cash_allocation * 100.0) << "% Cash" << std::endl;
    printf("Target End Savings: $%.0f (Minimum Required Balance)\n", target_end_savings);
    printf("Parameters: InitSav $%.0f, StartSpend $%.0f, RetAge %d, SSStart $%.0f@%d, Infl %.2f%%\n",
        initial_savings, annual_spending_start, retirement_age, social_security_start, social_security_age, inflation_rate * 100.0);
    printf("Failure Threshold: Spending < 75%% of Required\n");
    std::cout << "------------------------------------" << std::endl;

    // --- Data storage for summary chart ---
    std::vector<std::pair<int, BacktestResult>> summary_results;

    for (int start_year = first_possible_start_year; start_year <= last_possible_start_year; ++start_year) {
        BacktestResult result = run_backtest_with_target(
            start_year, simulation_duration, historical_sp500_price_returns, speculative_sp500_returns,
            initial_savings, work_income_start, work_income_end, current_age, retirement_age,
            annual_spending_start, inflation_rate, social_security_age, social_security_start, cola_rate,
            stock_allocation, bond_allocation, cash_allocation, bond_annual_return, cash_annual_return,
            target_end_savings
        );

        if (result.final_savings > -1e17) {
            total_runs++;
            bool success = (result.final_savings >= target_end_savings && result.failure_year_count == 0);
            if (success) {
                success_count++;
            }
            if (PRINT_PER_RUN_SUMMARY) {
                printf("Start Year: %d | Final: $%12.0f | Target: $%12.0f | Failure Years: %2d | Success: %s\n",
                    start_year, result.final_savings, target_end_savings, result.failure_year_count,
                    (success ? "Yes" : " No"));
            }
            summary_results.push_back({start_year, result});
            failure_years_total += result.failure_year_count;
        }
    }

    // --- Calculate and Print Final Summary ---
    if (total_runs > 0) {
        double success_rate = static_cast<double>(success_count) / total_runs * 100.0;
        std::cout << "\n--- Overall Backtesting Summary ---" << std::endl;
            std::cout << "Total Historical Periods Tested: " << total_runs << std::endl;
        std::cout << "Successful Periods (Target Met + No Failure Years): " << success_count << std::endl;
        printf("Overall Success Rate: %.2f%%\n", success_rate);
        printf("Total Failure Years Across All Periods: %d\n", failure_years_total);
        std::cout << "----------------------------------" << std::endl;
    }

    // --- Print Summary Chart ---
    std::cout << "\n--- Summary Chart ---\n";
    std::cout << std::setw(8) << "Year" << " | "
          << std::setw(14) << "End Balance" << " | "
          << std::setw(14) << "vs Target" << " | "
          << std::setw(8) << "Fail Yrs" << " | "
          << "Reason\n";
    std::cout << "--------------------------------------------------------------------------\n";
    for (const auto& pair : summary_results) {
        int year = pair.first;
        const BacktestResult& result = pair.second;
        double difference = result.final_savings - target_end_savings;
        bool success = (result.final_savings >= target_end_savings && result.failure_year_count == 0);

        std::cout << std::setw(8) << year << " | "
              << std::setw(14) << format_usd(result.final_savings) << " | "
              << std::setw(14) << format_usd(difference) << " | "
              << std::setw(8) << result.failure_year_count << " | "
              << (success ? "✓ " : "✗ ")
              << get_return_reason(year) << "\n";
    }
    std::cout << "--------------------------------------------------------------------------\n";

    return 0;
}