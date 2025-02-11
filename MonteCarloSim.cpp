#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>

#define NUM_SIMULATIONS 10000
#define TIME_HORIZON 40

double random_normal(double mean, double stddev) {
    // Box-Muller transform to generate normally distributed random numbers
    double u1 = (double)rand() / RAND_MAX;
    double u2 = (double)rand() / RAND_MAX;
    double z0 = sqrt(-2.0 * log(u1)) * cos(2.0 * M_PI * u2);
    return z0 * stddev + mean;
}



int main() {
    double initial_savings = 100000;
    double annual_spending_start = 80000;
    double annual_spending_end = 100000;
    double investment_mean = 0.05;
    double investment_stddev = 0.05;
    double inflation_rate = 0.03;
    double social_security_start = 34000;
    double cola_rate = 0.02;
    int retirement_age = 67;
    int current_age = 50;
    double work_income_start = 100000;  // Starting annual work income
    double work_income_end = 0;  // Income at retirement (0 if fully retired)
    long double max_savings = -INFINITY;  // Correctly initialize max_savings
    long double min_savings = INFINITY;   // Correctly initialize min_savings

    int success_count = 0;

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
            savings = savings * (1 + investment_return) - annual_spending + social_security + work_income;

            printf("Year %d: Age %d, Savings: %.2f, Spending: %.2f, Social Security: %.2f, Investment Return: %.2f Income: %.2f\n", year + 1, age, savings, annual_spending, social_security, investment_return, work_income);  // Add this line to show yearly results

            //annual_spending = fmax(annual_spending_end, annual_spending - ((annual_spending_start - annual_spending_end) / (TIME_HORIZON - 1)));
           
        }
        if (savings > max_savings)
            max_savings = savings;

        if (savings < min_savings)
            min_savings = savings;

        if (savings >= 0)
        {
            success_count++;
        }
    }

       

    double success_rate = (double)success_count / NUM_SIMULATIONS;
    printf("Probability of Success: %.2f%%\n", success_rate * 100);
    printf("Maximum Savings: %.2f\n", max_savings);
    printf("Minimum Savings: %.2f\n", min_savings);

    return 0;

    //Test
}
