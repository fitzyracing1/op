#include <cs50.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "bisect.h"

// SECTION 1: Binary Sequence Definition
const char *binaries[] = {
    "10001", "001001", "00101001", "0010010", "0100101", "01001010", "01001010",
    "10100101", "0100101001", "001001001", "0010010010", "01001010010",
    "10010100101", "0010010010010", "001001001001001", "0100101001001001",
    "10010100101001010", "0010010010010010010", "01001010010010010010",
    "100101001010010100101", "001001001001001001001", "0100101001001001001001",
    "100101001010010100101001", "0010010010010010010010010",
    "01001010010010010010010010", "100101001010010100101001010",
    "001001001001001001001001001", "0100101001001001001001001001",
    "10010100101001010010100101001", "0010010010010010010010010010",
    "01001010010010010010010010010", "100101001010010100101001010010",
    "001001001001001001001001001001", "010010100"
};
const int num_strings = 34;

// SECTION 2: Function Definitions
double f1(double x)
{
    return x * x - 2; // x^2 - 2
}

double f2(double x)
{
    return sin(x) - 0.5; // sin(x) - 0.5
}

// SECTION 3: Bit Analysis Definition
typedef struct {
    int ones_count;
    double ratio;
    int transitions;
} BitAnalysis;

void analyze_bits(const char *binary, BitAnalysis *analysis, long target_iterations)
{
    analysis->ones_count = 0;
    analysis->transitions = 0;
    int len = strlen(binary);
    for (int i = 0; i < len; i++)
    {
        if (binary[i] != '0' && binary[i] != '1')
        {
            analysis->ones_count = -1;
            analysis->ratio = 0;
            analysis->transitions = -1;
            return;
        }
        if (binary[i] == '1')
        {
            analysis->ones_count++;
        }
        if (i > 0 && binary[i] != binary[i - 1])
        {
            analysis->transitions++;
        }
    }
    analysis->ratio = len > 0 ? (double) analysis->ones_count / len : 0;
    if (target_iterations > 0)
    {
        analysis->ones_count *= (int)(target_iterations / 1000); // Scale by 1000
    }
}

// SECTION 4: Main Program
int main(void)
{
    // SECTION 5: Input and Configuration
    printf("Binary Sequence and Bisection Analysis\n");
    double tol = 0.0001;
    double a = get_double("Enter interval start (a): ");
    double b = get_double("Enter interval end (b): ");
    long target_iterations = 1000; // Target 1000 years

    // SECTION 6: Per-Bit Analysis
    printf("\n--- Per-Bit Analysis ---\n");
    BitAnalysis analyses[34];
    for (int i = 0; i < num_strings; i++)
    {
        analyze_bits(binaries[i], &analyses[i], target_iterations);
        if (analyses[i].ones_count >= 0)
        {
            printf("Binary %s: %d 1s, Ratio %.2f, Transitions %d\n",
                   binaries[i], analyses[i].ones_count, analyses[i].ratio, analyses[i].transitions);
        }
        else
        {
            printf("Binary %s: Error (invalid binary)\n", binaries[i]);
        }
    }

    // SECTION 7: Single Bisection
    printf("\n--- Single Bisection (x^2 - 2) ---\n");
    Trap trap;
    double root = bisect(f1, a, b, tol, &trap);
    if (!isnan(root))
    {
        printf("Root in [%.2f, %.2f]: %.6f\n", a, b, root);
        printf("f(%.6f) = %.6f\n", root, f1(root));
    }
    else
    {
        printf("No root found in [%.2f, %.2f]\n", a, b);
    }
    printf("Trap Status: Code %d, Message: %s, Iterations: %d\n", trap.code, trap.message, trap.iterations);

    // SECTION 8: Brink List (f(x) <= 0)
    printf("\n--- Brink List (sin(x) - 0.5 <= 0) ---\n");
    BrinkList brink_list;
    int num_brink_points = generate_brink_list(f2, a, b, 100, &brink_list);
    for (int i = 0; i < brink_list.count; i++)
    {
        printf("Point %d: x=%.6f, f(x)=%.6f\n", i + 1, brink_list.points[i].x, brink_list.points[i].fx);
    }

    // SECTION 9: Output for Plotting
    printf("\n--- Saving Results ---\n");
    FILE *fp = fopen("results.csv", "w");
    fprintf(fp, "type,index,x,fx\n");
    for (int i = 0; i < num_strings; i++)
    {
        if (analyses[i].ones_count >= 0)
        {
            fprintf(fp, "binary,%d,%.6f,%.6f\n", i, (double)i, analyses[i].ratio);
        }
    }
    for (int i = 0; i < brink_list.count; i++)
    {
        fprintf(fp, "brink,%d,%.6f,%.6f\n", i, brink_list.points[i].x, brink_list.points[i].fx);
    }
    fclose(fp);
    free(brink_list.points); // Now compiles with <stdlib.h>
    printf("Results saved to results.csv for plotting\n");

    return 0;
}