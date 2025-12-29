#include <vector>
#include <utility>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <locale>
#include <fstream>

const bool PRINT_PER_RUN_SUMMARY = false;
const bool PRINT_YEARLY_DETAILS = false;
const bool PRINT_HTML_OUTPUT = true;

const std::vector<std::pair<int, double>> historical_sp500_price_returns = {
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

const std::vector<std::pair<int, double>> speculative_sp500_returns = {
    {2025, 0.0850}, {2026, 0.0950}, {2027, 0.0650}, {2028, 0.0750}, {2029, 0.0550},
    {2030, 0.0900}, {2031, 0.1050}, {2032, 0.0700}, {2033, -0.1800}, {2034, -0.0950},
    {2035, 0.2200}, {2036, 0.1100}, {2037, 0.0850}, {2038, 0.0750}, {2039, 0.0950},
    {2040, 0.1100}, {2041, 0.0850}, {2042, 0.0700}, {2043, 0.0900}, {2044, 0.0600},
    {2045, 0.0750}, {2046, 0.0850}, {2047, -0.2400}, {2048, -0.1200}, {2049, 0.2800},
    {2050, 0.1550}, {2051, 0.0750}, {2052, 0.0650}, {2053, 0.0800}, {2054, 0.0550},
    {2055, 0.0750}, {2056, 0.0900}, {2057, 0.0650}, {2058, -0.1100}, {2059, 0.1850},
    {2060, 0.0800}, {2061, 0.0950}, {2062, 0.0700}, {2063, 0.0600}, {2064, 0.0550}
};

#define TIME_HORIZON 40

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

int find_start_index(int start_year, const std::vector<std::pair<int, double>>& data) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i].first == start_year) {
            return i;
        }
    }
    return -1;
}

double get_stock_return(int year, const std::vector<std::pair<int, double>>& historical_data, const std::vector<std::pair<int, double>>& speculative_data) {
    for (const auto& p : historical_data) {
        if (p.first == year) {
            return p.second;
        }
    }
    for (const auto& p : speculative_data) {
        if (p.first == year) {
            return p.second;
        }
    }
    return 0.0;
}

std::string format_usd(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(0) << value;
    return ss.str();
}

std::string format_percent(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << (value * 100.0);
    return ss.str();
}

struct YearlyData {
    int year;
    int age;
    double start_balance;
    double end_savings;
    double annual_spending;
    double spend_percent;
    double market_return;
    double market_value_change;
    double work_income;
    double social_security;
    double dividend_income;
    double balance_change;
    bool is_failure_year;
};

struct BacktestResult {
    double final_savings;
    int failure_year_count;
    int start_year;
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
    result.start_year = start_simulation_year;

    int data_start_index = find_start_index(start_simulation_year, historical_price_data);

    if (data_start_index == -1) {
        result.final_savings = -1e18;
        return result;
    }

    const double FLAT_DIVIDEND_YIELD = 0.02;
    const double FAILURE_THRESHOLD = 0.75;

    const int PHASE1_AGE_START = 67;
    const int PHASE2_AGE_START = 75;
    const int PHASE3_AGE_START = 85;
    const double PHASE1_INFLATION_RATE = inflation_rate;
    const double PHASE2_INFLATION_RATE = inflation_rate * 0.5;
    const double PHASE3_INFLATION_RATE = 0.0;

    double savings = initial_savings;
    double annual_spending = annual_spending_start;

    for (int year_offset = 0; year_offset < simulation_length_years; ++year_offset) {
        int age = current_age + year_offset;
        int historical_year = start_simulation_year + year_offset;

        double start_of_year_savings = savings;
        
        double stock_return = get_stock_return(historical_year, historical_price_data, speculative_price_data);
        double blended_return = (stock_allocation * stock_return) + 
                                (bond_allocation * bond_annual_return) + 
                                (cash_allocation * cash_annual_return);
        
        double dividend_income = savings * FLAT_DIVIDEND_YIELD;
        double market_value_increase = start_of_year_savings * blended_return;
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

        double social_security = 0.0;
        if (age >= social_security_age) {
            social_security = social_security_start * pow((1.0 + cola_rate), (age - social_security_age));
        }

        double phase_inflation_rate = inflation_rate;
        
        if (age >= PHASE3_AGE_START) {
            phase_inflation_rate = PHASE3_INFLATION_RATE;
        }
        else if (age >= PHASE2_AGE_START) {
            phase_inflation_rate = PHASE2_INFLATION_RATE;
        }
        else if (age >= PHASE1_AGE_START) {
            phase_inflation_rate = PHASE1_INFLATION_RATE;
        }

        annual_spending *= (1.0 + phase_inflation_rate);
        double required_spending = annual_spending;
        
        double available_for_spending = savings_after_price_change + dividend_income + social_security + current_work_income;
        
        double remaining_years = simulation_length_years - year_offset;
        double required_end_balance = target_end_savings;
        double minimum_to_reserve = required_end_balance / pow(1.02, remaining_years);
        double available_after_reserve = available_for_spending - minimum_to_reserve;
        
        double actual_spending = std::min(required_spending, available_after_reserve);
        actual_spending = std::max(0.0, actual_spending);
        
        bool is_failure_year = (actual_spending < required_spending * FAILURE_THRESHOLD);
        if (is_failure_year) {
            result.failure_year_count++;
        }

        double spend_percent = (start_of_year_savings > 0) ? (actual_spending / start_of_year_savings) * 100.0 : 0.0;

        savings = savings_after_price_change + dividend_income - actual_spending + social_security + current_work_income;
        double balance_change = savings - start_of_year_savings;

        YearlyData yd;
        yd.year = historical_year;
        yd.age = age;
        yd.start_balance = start_of_year_savings;
        yd.end_savings = savings;
        yd.annual_spending = actual_spending;
        yd.spend_percent = spend_percent;
        yd.market_return = blended_return;
        yd.market_value_change = market_value_increase;
        yd.work_income = current_work_income;
        yd.social_security = social_security;
        yd.dividend_income = dividend_income;
        yd.balance_change = balance_change;
        yd.is_failure_year = is_failure_year;
        result.yearly_data.push_back(yd);
    }

    result.final_savings = savings;
    return result;
}

void write_html_output(const std::vector<BacktestResult>& all_results,
    double initial_savings, double work_income_start, double work_income_end,
    int current_age, int retirement_age, double annual_spending_start,
    double inflation_rate, int social_security_age, double social_security_start,
    double cola_rate, double stock_allocation, double bond_allocation,
    double cash_allocation, double bond_annual_return, double cash_annual_return,
    int simulation_duration) {
    std::ofstream html_file("index.html");
    
    html_file << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html_file << "    <meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html_file << "    <title>Retirement Scenario Analysis</title>\n";
    html_file << "    <script src=\"https://cdn.jsdelivr.net/npm/apexcharts@latest/dist/apexcharts.min.js\"></script>\n";
    html_file << "    <style>\n";
    html_file << "        * { margin: 0; padding: 0; box-sizing: border-box; }\n";
    html_file << "        body { font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; padding: 20px; }\n";
    html_file << "        .container { max-width: 1600px; margin: 0 auto; background: white; border-radius: 12px; box-shadow: 0 20px 60px rgba(0, 0, 0, 0.3); padding: 40px; }\n";
    html_file << "        h1 { color: #2c3e50; margin-bottom: 10px; font-size: 2.5em; }\n";
    html_file << "        h2 { color: #667eea; margin-top: 40px; margin-bottom: 20px; font-size: 1.6em; border-bottom: 2px solid #667eea; padding-bottom: 10px; }\n";
    html_file << "        .subtitle { color: #7f8c8d; margin-bottom: 30px; font-size: 1.1em; }\n";
    html_file << "        .controls { display: flex; gap: 20px; margin-bottom: 30px; flex-wrap: wrap; align-items: center; }\n";
    html_file << "        .control-group { display: flex; align-items: center; gap: 10px; }\n";
    html_file << "        label { font-weight: 600; color: #2c3e50; }\n";
    html_file << "        select, button { padding: 10px 15px; border: 2px solid #e0e0e0; border-radius: 6px; font-size: 1em; cursor: pointer; transition: all 0.3s ease; }\n";
    html_file << "        select:focus { border-color: #667eea; outline: none; }\n";
    html_file << "        button { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; border: none; font-weight: 600; }\n";
    html_file << "        button:hover { transform: translateY(-2px); box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4); }\n";
    html_file << "        table { width: 100%; border-collapse: collapse; margin-bottom: 20px; background: white; border-radius: 8px; overflow: hidden; box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1); }\n";
    html_file << "        th { background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); color: white; padding: 12px; text-align: left; font-weight: 600; font-size: 0.9em; }\n";
    html_file << "        td { padding: 12px; border-bottom: 1px solid #e0e0e0; }\n";
    html_file << "        tr:hover { background: #f8f9fa; cursor: pointer; }\n";
    html_file << "        tr:last-child td { border-bottom: none; }\n";
    html_file << "        .success { color: #10b981; font-weight: 600; }\n";
    html_file << "        .failure { color: #ef4444; font-weight: 600; }\n";
    html_file << "        .expand-btn { cursor: pointer; font-weight: bold; color: #667eea; user-select: none; padding: 0 5px; }\n";
    html_file << "        .yearly-details { display: none; background: #f8f9fa; }\n";
    html_file << "        .yearly-details.active { display: table-row; }\n";
    html_file << "        .yearly-table { width: 100%; border-collapse: collapse; margin: 10px 0; background: white; font-size: 0.8em; }\n";
    html_file << "        .yearly-table th { background: #667eea; color: white; padding: 6px; text-align: left; font-size: 0.8em; }\n";
    html_file << "        .yearly-table td { padding: 6px; border-bottom: 1px solid #ddd; }\n";
    html_file << "        .yearly-row:hover { background: #f0f0f0; }\n";
    html_file << "        .details-cell { padding: 20px; overflow-x: auto; }\n";
    html_file << "        .chart-container { background: white; border-radius: 8px; padding: 20px; box-shadow: 0 2px 8px rgba(0, 0, 0, 0.1); margin-bottom: 30px; }\n";
    html_file << "        #chart { width: 100%; height: 500px; }\n";
    html_file << "        .legend-info { display: flex; gap: 30px; flex-wrap: wrap; margin-top: 20px; padding-top: 20px; border-top: 1px solid #e0e0e0; }\n";
    html_file << "        .legend-item { display: flex; align-items: center; gap: 8px; }\n";
    html_file << "        .legend-color { width: 20px; height: 20px; border-radius: 2px; }\n";
    html_file << "        .legend-item.success .legend-color { background: #10b981; }\n";
    html_file << "        .legend-item.failure .legend-color { background: #ef4444; }\n";
    html_file << "        .apexcharts-tooltip { pointer-events: auto !important; }\n";
    html_file << "        .numeric { text-align: right; font-family: 'Courier New', monospace; }\n";
    html_file << "        .assumptions-grid { display: grid; grid-template-columns: 1fr 1fr; gap: 0; margin-bottom: 20px; }\n";
    html_file << "        .assumptions-grid tr { border: none; }\n";
    html_file << "        .assumptions-grid tr:nth-child(odd) { background: #f8f9fa; }\n";
    html_file << "        .assumptions-grid td:first-child { background: #e8f0ff; font-weight: 600; color: #667eea; }\n";
    html_file << "        @media (max-width: 1200px) { .container { max-width: 100%; } table { font-size: 0.85em; } .yearly-table { font-size: 0.75em; } }\n";
    html_file << "    </style>\n";
    html_file << "</head>\n<body>\n";
    html_file << "    <div class=\"container\">\n";
    html_file << "        <h1>💰 Retirement Scenario Analysis</h1>\n";
    html_file << "        <p class=\"subtitle\">Monte Carlo Simulation - All Historical Starting Years (1928-2024)</p>\n";
    
    // Assumptions Table
    html_file << "        <h2>⚙️ Simulation Assumptions</h2>\n";
    html_file << "        <table class=\"assumptions-grid\">\n";
    html_file << "            <tr><td>Current Age</td><td class=\"numeric\">" << current_age << "</td></tr>\n";
    html_file << "            <tr><td>Retirement Age</td><td class=\"numeric\">" << retirement_age << "</td></tr>\n";
    html_file << "            <tr><td>Simulation Duration</td><td class=\"numeric\">" << simulation_duration << " years</td></tr>\n";
    html_file << "            <tr><td>Initial Savings</td><td class=\"numeric\">$" << format_usd(initial_savings) << "</td></tr>\n";
    html_file << "            <tr><td>Starting Work Income</td><td class=\"numeric\">$" << format_usd(work_income_start) << "</td></tr>\n";
    html_file << "            <tr><td>Work Income at Retirement</td><td class=\"numeric\">$" << format_usd(work_income_end) << "</td></tr>\n";
    html_file << "            <tr><td>Annual Spending (Year 1)</td><td class=\"numeric\">$" << format_usd(annual_spending_start) << "</td></tr>\n";
    html_file << "            <tr><td>Social Security Age</td><td class=\"numeric\">" << social_security_age << "</td></tr>\n";
    html_file << "            <tr><td>Social Security Annual (Start)</td><td class=\"numeric\">$" << format_usd(social_security_start) << "</td></tr>\n";
    html_file << "            <tr><td>SS COLA Rate</td><td class=\"numeric\">" << format_percent(cola_rate) << "%</td></tr>\n";
    html_file << "            <tr><td>General Inflation Rate</td><td class=\"numeric\">" << format_percent(inflation_rate) << "%</td></tr>\n";
    html_file << "            <tr><td>Stock Allocation</td><td class=\"numeric\">" << format_percent(stock_allocation) << "%</td></tr>\n";
    html_file << "            <tr><td>Bond Allocation</td><td class=\"numeric\">" << format_percent(bond_allocation) << "%</td></tr>\n";
    html_file << "            <tr><td>Cash Allocation</td><td class=\"numeric\">" << format_percent(cash_allocation) << "%</td></tr>\n";
    html_file << "            <tr><td>Bond Annual Return</td><td class=\"numeric\">" << format_percent(bond_annual_return) << "%</td></tr>\n";
    html_file << "            <tr><td>Cash Annual Return</td><td class=\"numeric\">" << format_percent(cash_annual_return) << "%</td></tr>\n";
    html_file << "        </table>\n";
    
    html_file << "        <div class=\"controls\">\n";
    html_file << "            <div class=\"control-group\">\n";
    html_file << "                <label for=\"startYearSelect\">Filter by Start Year:</label>\n";
    html_file << "                <select id=\"startYearSelect\">\n";
    html_file << "                    <option value=\"\">All Scenarios</option>\n";
    
    for (const auto& scenario : all_results) {
        html_file << "                    <option value=\"" << scenario.start_year << "\">Year " << scenario.start_year << "</option>\n";
    }
    
    html_file << "                </select>\n";
    html_file << "            </div>\n";
    html_file << "            <button onclick=\"showAllScenarios()\">Show All</button>\n";
    html_file << "            <button onclick=\"deselectYear()\">Deselect Year</button>\n";
    html_file << "            <button onclick=\"toggleFailuresOnly()\">Toggle Failures Only</button>\n";
    html_file << "        </div>\n";
    
    // Summary Statistics Table
    html_file << "        <h2>📊 Summary Statistics</h2>\n";
    html_file << "        <table>\n";
    html_file << "            <thead><tr><th>Metric</th><th class=\"numeric\">Value</th></tr></thead>\n";
    html_file << "            <tbody>\n";
    html_file << "                <tr><td>Total Scenarios</td><td class=\"numeric\" id=\"totalScenarios\">-</td></tr>\n";
    html_file << "                <tr><td>Successful Scenarios</td><td class=\"numeric\" id=\"successCount\">-</td></tr>\n";
    html_file << "                <tr><td>Failed Scenarios</td><td class=\"numeric\" id=\"failedCount\">-</td></tr>\n";
    html_file << "                <tr><td>Success Rate</td><td class=\"numeric\" id=\"successRate\">-</td></tr>\n";
    html_file << "                <tr><td>Average End Balance</td><td class=\"numeric\" id=\"avgBalance\">-</td></tr>\n";
    html_file << "                <tr><td>Best Case (Max Balance)</td><td class=\"numeric\" id=\"maxBalance\">-</td></tr>\n";
    html_file << "                <tr><td>Worst Case (Min Balance)</td><td class=\"numeric\" id=\"minBalance\">-</td></tr>\n";
    html_file << "            </tbody>\n";
    html_file << "        </table>\n";
    
    html_file << "        <h2>📈 Interactive Chart</h2>\n";
    html_file << "        <div class=\"chart-container\"><div id=\"chart\"></div>\n";
    html_file << "            <div class=\"legend-info\">\n";
    html_file << "                <div class=\"legend-item success\"><div class=\"legend-color\"></div><span>Successful (No shortfalls)</span></div>\n";
    html_file << "                <div class=\"legend-item failure\"><div class=\"legend-color\"></div><span>Failed (Had shortfall years)</span></div>\n";
    html_file << "            </div>\n";
    html_file << "        </div>\n";
    
    // Scenario Results Table
    html_file << "        <h2>🎯 Scenario Results (Click to Expand)</h2>\n";
    html_file << "        <table id=\"scenarioTable\">\n";
    html_file << "            <thead><tr><th></th><th>Start Year</th><th class=\"numeric\">Final Balance</th><th class=\"numeric\">Failure Years</th><th>Status</th></tr></thead>\n";
    html_file << "            <tbody id=\"scenarioTableBody\"></tbody>\n";
    html_file << "        </table>\n";
    
    html_file << "    </div>\n";
    html_file << "    <script>\n";
    html_file << "        const allData={\"scenarios\":[";
    
    for (size_t i = 0; i < all_results.size(); ++i) {
        const BacktestResult& result = all_results[i];
        html_file << "{\"start_year\":" << result.start_year << ",\"final_balance\":" << format_usd(result.final_savings) << ",\"failure_years\":" << result.failure_year_count << ",\"data\":[";
        
        for (size_t j = 0; j < result.yearly_data.size(); ++j) {
            const YearlyData& yd = result.yearly_data[j];
            html_file << "{\"yo\":" << j << ",\"y\":" << yd.year << ",\"a\":" << yd.age << ",\"sb\":" << format_usd(yd.start_balance) << ",\"eb\":" << format_usd(yd.end_savings) << ",\"sp\":" << format_usd(yd.annual_spending) << ",\"pc\":" << std::fixed << std::setprecision(2) << yd.spend_percent << ",\"mr\":" << std::fixed << std::setprecision(2) << (yd.market_return * 100.0) << ",\"mvc\":" << format_usd(yd.market_value_change) << ",\"wi\":" << format_usd(yd.work_income) << ",\"ss\":" << format_usd(yd.social_security) << ",\"di\":" << format_usd(yd.dividend_income) << ",\"bc\":" << format_usd(yd.balance_change) << ",\"f\":" << (yd.is_failure_year ? "1" : "0") << "}";
            if (j < result.yearly_data.size() - 1) html_file << ",";
        }
        html_file << "]}";
        if (i < all_results.size() - 1) html_file << ",";
    }
    
    html_file << "]};\n";
    html_file << "        let chart=null,failureOnly=false,data=allData.scenarios;\n";
    html_file << "        function formatNum(n){return(n/1000000).toFixed(2)+'M';}\n";
    html_file << "        function formatUSD(n){const v=parseFloat(n);return '$'+(Math.abs(v)>=1000000?(v/1000000).toFixed(2)+'M':(v/1000).toFixed(0)+'K');}\n";
    html_file << "        function getMkt(r){return r>15?'Strong Bull':r>10?'Bull':r>5?'Moderate Gain':r>=-5?'Flat':r>=-15?'Mild Decline':'Bear Market';}\n";
    html_file << "        function toggleExpand(btn,scenarioIdx){btn.textContent=btn.textContent==='▼'?'▶':'▼';const detailsRow=document.getElementById('details-'+scenarioIdx);detailsRow.classList.toggle('active');}\n";
    html_file << "        function buildYearlyTable(scenario){let html='<table class=\"yearly-table\"><thead><tr><th>Yr</th><th>Year</th><th>Age</th><th class=\"numeric\">Start Bal</th><th class=\"numeric\">Mkt Return%</th><th class=\"numeric\">Mkt Value Chg</th><th class=\"numeric\">Dividends</th><th class=\"numeric\">Work Inc</th><th class=\"numeric\">SS Income</th><th class=\"numeric\">Total Income</th><th class=\"numeric\">Expenses</th><th class=\"numeric\">Net Chg</th><th class=\"numeric\">End Bal</th><th>Status</th></tr></thead><tbody>';scenario.data.forEach(yr=>{const totalIncome=parseFloat(yr.di)+parseFloat(yr.wi)+parseFloat(yr.ss);const status=yr.f=='1'?'<span class=\"failure\">⚠️</span>':'<span class=\"success\">✓</span>';html+=`<tr class=\"yearly-row\"><td>${yr.yo}</td><td>${yr.y}</td><td>${yr.a}</td><td class=\"numeric\">${formatUSD(yr.sb)}</td><td class=\"numeric\" style=\"color:${yr.mr>=0?'#10b981':'#ef4444'};\">${yr.mr.toFixed(2)}%</td><td class=\"numeric\" style=\"color:${parseFloat(yr.mvc)>=0?'#10b981':'#ef4444'};\">${formatUSD(yr.mvc)}</td><td class=\"numeric\">${formatUSD(yr.di)}</td><td class=\"numeric\">${formatUSD(yr.wi)}</td><td class=\"numeric\">${formatUSD(yr.ss)}</td><td class=\"numeric\"><strong>${formatUSD(totalIncome)}</strong></td><td class=\"numeric\">${formatUSD(yr.sp)}</td><td class=\"numeric\" style=\"color:${parseFloat(yr.bc)>=0?'#10b981':'#ef4444'};\">${parseFloat(yr.bc)>=0?'+':''} ${formatUSD(yr.bc)}</td><td class=\"numeric\"><strong>${formatUSD(yr.eb)}</strong></td><td>${status}</td></tr>`;});html+='</tbody></table>';return html;}\n";
    html_file << "        function updateStats(){const stats=data;let max=Math.max(...stats.map(s=>parseFloat(s.final_balance)));let min=Math.min(...stats.map(s=>parseFloat(s.final_balance)));const avg=stats.length>0?stats.reduce((s,sc)=>s+parseFloat(sc.final_balance),0)/stats.length:0;const failed=stats.filter(sc=>sc.failure_years>0).length;const successful=stats.length-failed;const rate=stats.length>0?(successful/stats.length*100).toFixed(1):0;document.getElementById('totalScenarios').textContent=stats.length;document.getElementById('successCount').textContent=successful;document.getElementById('failedCount').textContent=failed;document.getElementById('successRate').textContent=rate+'%';document.getElementById('avgBalance').textContent=formatUSD(avg);document.getElementById('maxBalance').textContent=formatUSD(max);document.getElementById('minBalance').textContent=formatUSD(min);updateScenarioTable();}\n";
    html_file << "        function updateScenarioTable(){const tbody=document.getElementById('scenarioTableBody');tbody.innerHTML='';data.forEach((sc,idx)=>{const status=sc.failure_years===0?'<span class=\"success\">✓ Success</span>':'<span class=\"failure\">⚠️ Failed</span>';const row=document.createElement('tr');row.innerHTML=`<td><span class=\"expand-btn\" onclick=\"toggleExpand(this,${idx})\">▶</span></td><td>${sc.start_year}</td><td class=\"numeric\">${formatUSD(sc.final_balance)}</td><td class=\"numeric\">${sc.failure_years}</td><td>${status}</td>`;tbody.appendChild(row);const detailsRow=document.createElement('tr');detailsRow.id='details-'+idx;detailsRow.className='yearly-details';detailsRow.innerHTML=`<td colspan=\"5\"><div class=\"details-cell\">${buildYearlyTable(sc)}</div></td>`;tbody.appendChild(detailsRow);});}\n";
    html_file << "        function render(){const s=data.map(sc=>({name:`${sc.start_year}`,data:sc.data.map(p=>({x:p.yo,y:Math.round(parseFloat(p.eb)),yo:p.yo,y_:p.y,a:p.a,sb:p.sb,eb:p.eb,sp:p.sp,pc:p.pc,mr:p.mr,mvc:p.mvc,wi:p.wi,ss:p.ss,di:p.di,bc:p.bc,f:p.f})),st:sc.start_year,fail:sc.failure_years}));const opts={chart:{type:'line',height:500,animations:{enabled:true,easing:'easeinout',speed:800,animateGradually:{enabled:true,delay:150},dynamicAnimation:{enabled:true}},toolbar:{show:true,tools:{download:true,selection:true,zoom:true,zoomin:true,zoomout:true,pan:true,reset:true}}},series:s,xaxis:{type:'numeric',title:{text:'Year Offset from Start (0-40)'},min:0,max:40},yaxis:{title:{text:'End Balance (\$)'},labels:{formatter:v=>`\$${formatNum(v)}`}},stroke:{width:2,curve:'smooth'},colors:data.map(s=>s.failure_years>0?'#ef4444':'#10b981'),tooltip:{enabled:true,shared:false,intersect:false,followMouseX:true,theme:'light',onDatasetHover:{highlightDataSeries:true},x:{show:true,format:'undefined'},y:{formatter:(val,{series,seriesIndex,dataPointIndex})=>{const pt=s[seriesIndex].data[dataPointIndex];if(!pt)return val;const m=getMkt(pt.mr);const totalIncome=parseFloat(pt.di)+parseFloat(pt.wi)+parseFloat(pt.ss);return`<div style='padding:8px;font-size:11px;width:350px;'><div style='font-weight:bold;color:#667eea;margin-bottom:6px;'>Scenario: ${s[seriesIndex].name} | Yr ${pt.yo}</div><table style='width:100%;'><tr><td style='color:#667eea;font-weight:600;width:50%;'>Calendar Year:</td><td style='text-align:right;'>${pt.y_}</td></tr><tr><td style='color:#667eea;font-weight:600;'>Age:</td><td style='text-align:right;'>${pt.a}</td></tr><tr><td style='color:#667eea;font-weight:600;'>Market Status:</td><td style='text-align:right;'>${m}</td></tr><tr><td style='color:#667eea;font-weight:600;'>Return %:</td><td style='text-align:right;color:${pt.mr>=0?'#10b981':'#ef4444'};'>${pt.mr.toFixed(2)}%</td></tr><tr style='border-top:1px solid #ddd;'><td style='color:#667eea;font-weight:600;padding-top:4px;'>Mkt Value Change:</td><td style='text-align:right;color:${parseFloat(pt.mvc)>=0?'#10b981':'#ef4444'};padding-top:4px;'>${formatUSD(pt.mvc)}</td></tr><tr><td style='color:#667eea;font-weight:600;'>Work Income:</td><td style='text-align:right;'>${formatUSD(pt.wi)}</td></tr><tr><td style='color:#667eea;font-weight:600;'>SS Income:</td><td style='text-align:right;'>${formatUSD(pt.ss)}</td></tr><tr><td style='color:#667eea;font-weight:600;'>Dividends:</td><td style='text-align:right;'>${formatUSD(pt.di)}</td></tr><tr style='border-top:1px solid #ddd;'><td style='color:#667eea;font-weight:600;padding-top:4px;'><strong>Total Income:</strong></td><td style='text-align:right;padding-top:4px;'><strong>${formatUSD(totalIncome)}</strong></td></tr><tr><td style='color:#667eea;font-weight:600;'>Expenses:</td><td style='text-align:right;'>${formatUSD(pt.sp)}</td></tr><tr style='border-top:1px solid #ddd;border-bottom:1px solid #ddd;'><td style='color:#667eea;font-weight:600;padding-top:4px;padding-bottom:4px;'><strong>Net Change:</strong></td><td style='text-align:right;color:${parseFloat(pt.bc)>=0?'#10b981':'#ef4444'};padding-top:4px;padding-bottom:4px;'><strong>${parseFloat(pt.bc)>=0?'+':''} ${formatUSD(pt.bc)}</strong></td></tr><tr><td style='color:#667eea;font-weight:600;'>End Balance:</td><td style='text-align:right;font-weight:bold;'>${formatUSD(pt.eb)}</td></tr></table><div style='margin-top:6px;font-size:10px;color:#999;'>${pt.f=='1'?'<span style=color:#ef4444;>⚠️Shortfall</span>':'<span style=color:#10b981;>✓OK</span>'}</div></div>`}}},legend:{position:'bottom',horizontalAlign:'left',fontSize:12},grid:{borderColor:'#e0e0e0',strokeDashArray:3}};if(chart)chart.destroy();chart=new ApexCharts(document.querySelector('#chart'),opts);chart.render();}\n";
    html_file << "        function showAll(){failureOnly=false;data=allData.scenarios;document.getElementById('startYearSelect').value='';render();updateStats();}\n";
    html_file << "        function deselect(){document.getElementById('startYearSelect').value='';showAll();}\n";
    html_file << "        function toggleFail(){failureOnly=!failureOnly;data=failureOnly?allData.scenarios.filter(s=>s.failure_years>0):allData.scenarios;if(data.length===0){alert('No failed scenarios');failureOnly=false;data=allData.scenarios;return;}render();updateStats();}\n";
    html_file << "        document.getElementById('startYearSelect').addEventListener('change',e=>{if(e.target.value){data=allData.scenarios.filter(s=>s.start_year===parseInt(e.target.value));render();updateStats();}else showAll();});\n";
    html_file << "        window.addEventListener('load',()=>{data=allData.scenarios;render();updateStats();});\n";
    html_file << "    </script>\n";
    html_file << "</body>\n</html>\n";
    
    html_file.close();
    
    std::cout << "HTML output written to index.html" << std::endl;
}

int main() {
    std::srand(std::time(nullptr));

    double initial_savings = 1600000;
    double work_income_start = 130000;
    double work_income_end = 0;
    int current_age = 50;
    int retirement_age = 53;
    double annual_spending_start = 88944;
    double inflation_rate = 0.025;
    int social_security_age = 67;
    double social_security_start = 34000;
    double cola_rate = 0.02;
    int simulation_duration = TIME_HORIZON;
    double target_end_savings = 0;

    double stock_allocation = 0.80;
    double bond_allocation = 0.10;
    double cash_allocation = 0.10;
    double bond_annual_return = 0.04;
    double cash_annual_return = 0.03;

    if (historical_sp500_price_returns.empty()) {
        std::cerr << "Error: Historical price data vector is empty." << std::endl;
        return 1;
    }

    int success_count = 0;
    int total_runs = 0;

    int first_possible_start_year = historical_sp500_price_returns[0].first;
    int last_possible_start_year = historical_sp500_price_returns.back().first;

    std::cout << "--- Running Comprehensive Backtest ---" << std::endl;
    std::cout << "Data Range: " << historical_sp500_price_returns[0].first << " - " << historical_sp500_price_returns.back().first << std::endl;
    std::cout << "Simulation Duration: " << simulation_duration << " years" << std::endl;
    std::cout << "Generating visualization data..." << std::endl;

    std::vector<BacktestResult> all_results;

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
            all_results.push_back(result);
        }
    }

    if (total_runs > 0) {
        double success_rate = static_cast<double>(success_count) / total_runs * 100.0;
        std::cout << "\n--- Summary ---" << std::endl;
        std::cout << "Total Scenarios: " << total_runs << std::endl;
        std::cout << "Successful Scenarios: " << success_count << std::endl;
        printf("Success Rate: %.2f%%\n", success_rate);
    }

    if (PRINT_HTML_OUTPUT) {
        write_html_output(all_results, initial_savings, work_income_start, work_income_end,
            current_age, retirement_age, annual_spending_start, inflation_rate,
            social_security_age, social_security_start, cola_rate, stock_allocation,
            bond_allocation, cash_allocation, bond_annual_return, cash_annual_return,
            simulation_duration);
    }

    return 0;
}