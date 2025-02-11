#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>

#define NUM_SIMULATIONS 1000
#define TIME_HORIZON 40
#define BUCKET_SIZE 100000
#define MAX_BUCKETS 40  // Adjust this based on expected range of savings

double random_normal(double mean, double stddev) {
    // Box-Muller transform to generate normally distributed random numbers
    double u1 = (double)rand() / RAND_MAX;
    double u2 = (double)rand() / RAND_MAX;
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return z0 * stddev + mean;
}

double work_income_start = 100000;  // Starting annual work income
double work_income_end = 0;  // Income at retirement (0 if fully retired)

double run_simulation(double work_income_start) 
{
    double initial_savings = 1000000;
    double annual_spending_start = 85000;
    double annual_spending_end = 100000;
    double investment_mean = 0.03;
    double investment_stddev = 0.08;
    double inflation_rate = 0.03;
    double social_security_start = 34000;
    double cola_rate = 0.02;
    double max_return = 0.07;
    int retirement_age = 67;
    int current_age = 50;
    
    
    long double max_savings = -INFINITY;  // Correctly initialize max_savings
    long double min_savings = INFINITY;   // Correctly initialize min_savings

    int success_count = 0;
    double final_savings[NUM_SIMULATIONS];  // Array to store final savings
    int buckets[MAX_BUCKETS] = { 0 };  // Initialize bucket counts to zero

    // Print sample investment returns
    printf("Sample Investment Returns:\n");
    for (int i = 0; i < 20; i++) {
        double sample_return = random_normal(investment_mean, investment_stddev);
        printf("%.2f\n", sample_return);
    }

    for (int sim = 0; sim < NUM_SIMULATIONS; sim++)
    {
        double savings = initial_savings;
        double annual_spending = annual_spending_start;
        printf("Simulation %d\n", sim + 1);  // Add this line to show simulation number
        for (int year = 0; year < TIME_HORIZON; year++)
        {
            int age = current_age + year;
            double social_security = 0;
            if (age >= retirement_age)
            {
                social_security = social_security_start * pow((1 + cola_rate), (age - retirement_age));
            }

            // Calculate work income based on the year
            double work_income = work_income_start - (year * (work_income_start - work_income_end) / (retirement_age - current_age));
            if (work_income < work_income_end) 
            {
                work_income = work_income_end;
            }

            // Adjust annual spending based on life events or conditions
            if (age % 5 == 0 && age >= retirement_age) 
            { // Example: reduce spending every 5 years after retirement
                annual_spending *= 0.90;
            }
            
            if (age < retirement_age) 
            {
                annual_spending *= (inflation_rate + 1);
            }

            //double adjusted_spending = annual_spending / pow((1 + inflation_rate), year);
            double investment_return = random_normal(investment_mean, investment_stddev);
            if (investment_return > max_return)
                investment_return = max_return;

            savings = savings * (1 + investment_return) - annual_spending + social_security + work_income;

            printf("Year %d: Age %d, Savings: %.2f, Spending: %.2f, Social Security: %.2f, Investment Return: %.2f Income: %.2f\n", year + 1, age, savings, annual_spending, social_security, investment_return, work_income);  // Add this line to show yearly results

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
    printf("Probability of Success: %.2f%%\n", success_rate * 100);
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
    return median_savings;

    //Test
}




int main() 
{
    double low_income = 0;
    double high_income = 500000;  // Adjust as needed
    double target_median_savings = 1000000;  // Adjust as needed
    double tolerance = 5000;  // Adjust as needed

    double median_savings = 0;
    double optimal_income = 0;

    while (high_income - low_income > tolerance) {
        double mid_income = (low_income + high_income) / 2;
        median_savings = run_simulation(mid_income);

        if (median_savings < target_median_savings) {
            low_income = mid_income;
        }
        else {
            high_income = mid_income;
        }
    }

    optimal_income = (low_income + high_income) / 2;
    printf("Optimal Income Level: %.2f\n", optimal_income);

    return 0;
}
