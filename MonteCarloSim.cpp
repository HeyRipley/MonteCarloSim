#include <vector>
#include <utility> // For std::pair
#include <stdexcept> // For error handling
#include <iostream>  // For printing results
#include <cmath>     // For fmax, pow
#include <numeric>   // For std::iota (optional)
#include <algorithm> // For std::find_if

// Structure to hold year and its return (alternative to std::pair)
// struct YearReturn {
//     int year;
//     double annual_return;
// };

// Define the data structure (using std::pair)
// Data: Year, Annual Return (as decimal)
const std::vector<std::pair<int, double>> historical_sp500_returns = {
    {1928, 0.3788}, {1929, -0.1191},
    {1930, -0.2848}, {1931, -0.4707}, {1932, -0.1515}, {1933, 0.4659},
    {1934, -5.94}, {1935, 41.37}, {1936, 27.92}, {1937, -38.59},
    {1938, 25.21}, {1939, -5.35}, {1940, -15.29}, {1941, -17.88},
    {1942, 12.47}, {1943, 19.08}, {1944, 13.29}, {1945, 30.72},
    {1946, -11.97}, {1947, 0.00}, {1948, -0.65}, {1949, 10.26},
    {1950, 21.75}, {1951, 16.46}, {1952, 11.78}, {1953, -6.62},
    {1954, 45.02}, {1955, 26.39}, {1956, 2.61}, {1957, -14.31},
    {1958, 38.06}, {1959, 8.48}, {1960, -3.00}, {1961, 23.13},
    {1962, -11.81}, {1963, 18.89}, {1964, 12.96}, {1965, 9.06},
    {1966, -13.09}, {1967, 19.99}, {1968, 7.66}, {1969, -11.36},
    {1970, -0.09}, {1971, 10.79}, {1972, 15.63}, {1973, -17.37},
    {1974, -29.72}, {1975, 31.55}, {1976, 19.15}, {1977, -11.50},
    {1978, 1.06}, {1979, 12.31}, {1980, 25.77}, {1981, -9.73},
    {1982, 14.76}, {1983, 17.27}, {1984, 1.40}, {1985, 26.33},
    {1986, 14.62}, {1987, 2.03}, {1988, 12.40}, {1989, 27.25},
    {1990, -6.56}, {1991, 26.31}, {1992, 4.46}, {1993, 7.06},
    {1994, -1.54}, {1995, 34.11}, {1996, 20.26}, {1997, 31.01},
    {1998, 26.67}, {1999, 19.53}, {2000, -10.14}, {2001, -13.04},
    {2002, -23.37}, {2003, 26.38}, {2004, 8.99}, {2005, 3.00},
    {2006, 13.62}, {2007, 3.53}, {2008, -38.49}, {2009, 23.45},
    {2010, 12.78}, {2011, 0.00}, {2012, 13.41}, {2013, 29.60},
    {2014, 11.39}, {2015, -0.73}, {2016, 9.54}, {2017, 19.42},
    {2018, -6.24}, {2019, 28.88}, {2020, 16.26}, {2021, 26.89},
    {2022, -19.44}, {2023, 0.2423}, {2024, 0.1124}, {2025, -13.93}
    // Note: Data manually transcribed from image. Double-check if critical.
    // Some values were unclear/potentially needed correction (e.g., 1933, 1934, 1937, 1970). Used values directly from table.
};

// Helper function to find the starting index for a given year
int find_start_index(int start_year, const std::vector<std::pair<int, double>>& data) {
    for (int i = 0; i < data.size(); ++i) {
        if (data[i].first == start_year) {
            return i;
        }
    }
    return -1; // Not found
}


#define NUM_SIMULATIONS 1000
#define TIME_HORIZON 40
#define BUCKET_SIZE 100000
#define MAX_BUCKETS 40  // Adjust this based on expected range of savings

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double random_normal(double mean, double stddev) {
    // Box-Muller transform to generate normally distributed random numbers
    double u1 = (double)rand() / RAND_MAX;
    double u2 = (double)rand() / RAND_MAX;
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return z0 * stddev + mean;
}

// Example backtesting function (adapt parameters as needed from your original simulation)
void run_backtest(
    int start_simulation_year,
    int simulation_length_years, // e.g., TIME_HORIZON
    const std::vector<std::pair<int, double>>& historical_data,
    // --- Add your other simulation parameters here ---
    double initial_savings,
    double work_income_start_param, // Using the improved parameter name
    double work_income_end,
    int current_age,
    int retirement_age,
    double annual_spending_start,
    double inflation_rate,
    int social_security_age,
    double social_security_start,
    double cola_rate
    // ... other parameters ...
) {
    int data_start_index = find_start_index(start_simulation_year, historical_data);

    if (data_start_index == -1) {
        std::cerr << "Error: Historical data for starting year " << start_simulation_year << " not found." << std::endl;
        return;
    }

    std::cout << "\n--- Running Backtest ---" << std::endl;
    std::cout << "Starting Year: " << start_simulation_year << std::endl;
    std::cout << "Simulation Length: " << simulation_length_years << " years" << std::endl;
    printf("Initial Savings: %.2f\n", initial_savings);
//    std::cout << "Initial Savings: $" << initial_savings << std::endl;
    std::cout << "------------------------" << std::endl;

    double savings = initial_savings;
    double annual_spending = annual_spending_start; // Reset for the backtest run

    for (int year_offset = 0; year_offset < simulation_length_years; ++year_offset) {
        int current_data_index = data_start_index + year_offset;
        int age = current_age + year_offset;
        int historical_year = start_simulation_year + year_offset; // The actual historical year

        // --- Check if we have enough historical data ---
        if (current_data_index >= historical_data.size()) {
            std::cout << "Warning: Ran out of historical data at year " << historical_year
                << ". Stopping backtest." << std::endl;
            break;
        }

        // --- Get the historical investment return ---
        double investment_return = historical_data[current_data_index].second;

        // --- Apply your existing logic using the historical return ---
        // (Using the improved linear interpolation income calculation from previous suggestion)
        double current_work_income = 0.0;
        double retirement_income_target = work_income_end; // Assuming end = target at retirement
        if (age >= retirement_age) {
            current_work_income = retirement_income_target;
        }
        else if (age < current_age) {
            current_work_income = work_income_start_param;
        }
        else {
            int total_working_years = retirement_age - current_age;
            if (total_working_years <= 0) {
                current_work_income = retirement_income_target;
            }
            else {
                double working_progress = (double)(age - current_age) / total_working_years;
                current_work_income = work_income_start_param + working_progress * (retirement_income_target - work_income_start_param);
                current_work_income = fmax(retirement_income_target, current_work_income);
            }
        }


        // Calculate Social Security
        double social_security = 0;
        if (age >= social_security_age) {
            // Corrected COLA calculation (base year is social_security_age)
            social_security = social_security_start * pow((1 + cola_rate), (age - social_security_age));
        }

        // Adjust spending (example logic from your original code)
        if (age % 5 == 0 && age >= social_security_age) {
            annual_spending *= 0.95;
        }
        if (age < retirement_age) { // Apply inflation pre-retirement
            annual_spending *= (1 + inflation_rate);
        }
        // NOTE: Consider if inflation should apply post-retirement too? Your original code didn't.

       // Update Savings for the year
        savings = savings * (1 + (investment_return/100)) - annual_spending + social_security + current_work_income;

        // Print yearly results for the backtest
        printf("Year %d (%d): Age %d, Savings: %.2f, Spending: %.2f, SS: %.2f, Hist. Return: %.2f%%, Income: %.2f\n",
            year_offset + 1, historical_year, age, savings, annual_spending, social_security, investment_return, current_work_income);

        if (savings < 0 && year_offset < simulation_length_years - 1) {
            printf("Note: Savings depleted before end of simulation period.\n");
            // Optionally break here if you only care about survival
            // break;
        }

    } // End yearly loop

    std::cout << "--- Backtest End ---" << std::endl;
    printf("Final Savings after %d years: %.2f\n", simulation_length_years, savings);
    std::cout << "--------------------\n" << std::endl;

}

double run_simulation(double work_income_start) 
{
	int work_income_end = 0;  // Adjust as needed
    double initial_savings = 1300000;
    double annual_spending_start = 100000;
    double annual_spending_end = 200000;
    double investment_mean = 0.08;
    double investment_stddev = 0.15;
    double inflation_rate = 0.04;
    double social_security_start = 34000;
	int social_security_age = 67;  // Age at which social security starts
    double cola_rate = 0.02;
    double max_return = 0.5;
    int retirement_age = 67;
    int current_age = 50;
    double current_work_income = work_income_start; // Initialize
    double retirement_income_target = 0.0; // Or whatever income should be at retirement
    
    long double max_savings = -INFINITY;  // Correctly initialize max_savings
    long double min_savings = INFINITY;   // Correctly initialize min_savings

    int success_count = 0;
    double final_savings[NUM_SIMULATIONS];  // Array to store final savings
    int buckets[MAX_BUCKETS] = { 0 };  // Initialize bucket counts to zero

    for (int sim = 0; sim < NUM_SIMULATIONS; sim++)
    {
        double savings = initial_savings;
        double annual_spending = annual_spending_start;
        printf("Simulation %d\n", sim + 1);  // Add this line to show simulation number
        for (int year = 0; year < TIME_HORIZON; year++)
        {
            int age = current_age + year;
            double social_security = 0;
            if (age >= social_security_age)
            {
                social_security = social_security_start * pow((1 + cola_rate), (age - social_security_age));
            }


            // Adjust annual spending based on life events or conditions.
			// Assume that spending decreases at social security age
            if (age % 5 == 0 && age >= social_security_age) 
            { // Example: reduce spending every 5 years after retirement
                annual_spending *= 0.98;
            }
            
            if (age < social_security_age)
            {
                annual_spending *= (inflation_rate + 1);
            }

            //double adjusted_spending = annual_spending / pow((1 + inflation_rate), year);
            double investment_return = random_normal(investment_mean, investment_stddev);
            if (investment_return > max_return)
                investment_return = max_return;

            

            if (age >= retirement_age) {
                current_work_income = retirement_income_target; // Income at or after retirement
            }
            else if (age < current_age) {
                current_work_income = work_income_start; // Should not happen in sim, but safe default
            }
            else {
                // Calculate the total number of years from start age to retirement age
                int total_working_years = retirement_age - current_age;

                if (total_working_years <= 0) {
                    // Handle edge case: already at or past retirement start age
                    current_work_income = retirement_income_target;
                }
                else {
                    // Calculate the fraction of the way through the working period (0.0 to 1.0)
                    // Ensure floating-point division
                    double working_progress = (double)(age - current_age) / total_working_years;

                    // Apply a curve (exponent > 1 tapers slower at start, faster at end)
                    // (exponent < 1 tapers faster at start, slower at end)
                    double curve_exponent = .25; // Example: Slight acceleration of taper towards end
                    double curved_progress = pow(working_progress, curve_exponent);

                    // Interpolate using the curved progress
                    current_work_income = work_income_start + curved_progress * (retirement_income_target - work_income_start);
                    current_work_income = fmax(retirement_income_target, current_work_income); // Maintain floor
                    
                    // Linearly interpolate between the start income and the target retirement income
                    //current_work_income = work_income_start + working_progress * (retirement_income_target - work_income_start);

                    // As we are tapering down, ensure it doesn't go below the target
                    // (Mainly needed if start < target, but good practice)
                    current_work_income = fmax(retirement_income_target, current_work_income);
                }
            }


            savings = savings * (1 + investment_return) - annual_spending + social_security + current_work_income;

            printf("Year %d: Age %d, Savings: %.2f, Spending: %.2f, Social Security: %.2f, Investment Return: %.2f Income: %.2f\n", year + 1, age, savings, annual_spending, social_security, investment_return, current_work_income);  // Add this line to show yearly results

            //annual_spending = fmax(annual_spending_end, annual_spending - ((annual_spending_start - annual_spending_end) / (TIME_HORIZON - 1)));
           
        }
        if (savings > max_savings)
            max_savings = savings;

        if (savings < min_savings)
            min_savings = savings;

        final_savings[sim] = savings;  // Store final savings

        int bucket_index = (int)(savings / BUCKET_SIZE);
        if (bucket_index >= 0 && bucket_index < MAX_BUCKETS) {
            buckets[bucket_index]++;
        }

        if (savings >= 0) {
            success_count++;
        }
       
        
    }
    // Sort the final savings array to find the median
    std::sort(final_savings, final_savings + NUM_SIMULATIONS);
    long double median_savings = final_savings[NUM_SIMULATIONS / 2];
       

    double success_rate = (double)success_count / NUM_SIMULATIONS;
    printf("Probability of Success (savings > $0): %.2f%%\n", success_rate * 100);
    printf("Maximum Savings: %.2f\n", max_savings);
    printf("Minimum Savings: %.2f\n", min_savings);


    // Print ASCII histogram
    printf("\nSavings Distribution (in buckets of $100,000):\n");
    for (int i = 0; i < MAX_BUCKETS; i++) {
        printf("$%d - $%d: ", i * BUCKET_SIZE, (i + 1) * BUCKET_SIZE - 1);
        for (int j = 0; j < buckets[i]; j += 10) {
            printf("#");
        }
        printf(" (%d)\n", buckets[i]);
    }


    int simulation_duration = 40; // How many years each backtest runs
    // Example: Starting just before the 2008 crash
    run_backtest(2007, simulation_duration, historical_sp500_returns,
        initial_savings, work_income_start, work_income_end, current_age, retirement_age,
        annual_spending_start, inflation_rate, social_security_age, social_security_start, cola_rate);

    // Example: Starting during a strong bull run period (e.g., early 80s)
    run_backtest(1982, simulation_duration, historical_sp500_returns,
        initial_savings, work_income_start, work_income_end, current_age, retirement_age,
        annual_spending_start, inflation_rate, social_security_age, social_security_start, cola_rate);

    // Example: Starting just before the Dot-com bust
    run_backtest(2000, simulation_duration, historical_sp500_returns,
        initial_savings, work_income_start, work_income_end, current_age, retirement_age,
        annual_spending_start, inflation_rate, social_security_age, social_security_start, cola_rate);
    

    int starting_year_adjusted = retirement_age - current_age;
    // --- Scenario 1: Starting right before the 1973-74 Crash / Stagflation ---
    std::cout << "\n>>> Scenario 1: Starting Year 1973 <<<" << std::endl;
    std::cout << "   Testing resilience against severe crash combined with high inflation." << std::endl;
    run_backtest(
        1973 - starting_year_adjusted,                           // Start Year
        simulation_duration,
        historical_sp500_returns,
        // --- Pass your simulation parameters ---
        initial_savings, work_income_start, work_income_end, current_age, retirement_age,
        annual_spending_start, inflation_rate, social_security_age, social_security_start, cola_rate
        // Add any other parameters your run_backtest function needs
    );

    return median_savings;

}




int main() 
{
    double low_income = 0;
	double starting_income = 100000;  // Adjust as needed
    double high_income = 200000;  // Adjust as needed
    double target_median_savings = 1000000;  // Adjust as needed
    double tolerance = 5000;  // Adjust as needed

    double median_savings = 0;
    double optimal_income = 0;

    srand(time(NULL));

    
    median_savings = run_simulation(starting_income);

    
    //optimal_income = (low_income + high_income) / 2;
    //printf("Optimal Income Level: %.2f\n", optimal_income);

    return 0;
}
