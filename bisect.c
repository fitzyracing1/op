#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "bisect.h"

double bisect(Function f, double a, double b, double tol, Trap *trap)
{
    trap->iterations = 0;
    trap->code = 0;
    strcpy(trap->message, "Success");
    if (f(a) * f(b) >= 0)
    {
        trap->code = 1;
        strcpy(trap->message, "Invalid interval: f(a) * f(b) >= 0");
        return NAN;
    }
    int max_iterations = 1000;
    double m;
    while ((b - a) / 2 > tol && trap->iterations < max_iterations)
    {
        m = (a + b) / 2;
        trap->iterations++;
        if (fabs(f(m)) < 1e-10)
        {
            trap->code = 3;
            strcpy(trap->message, "Precision issue: f(m) near zero");
            return m;
        }
        if (f(a) * f(m) < 0)
        {
            b = m;
        }
        else
        {
            a = m;
        }
    }
    if (trap->iterations >= max_iterations)
    {
        trap->code = 2;
        strcpy(trap->message, "Iteration limit exceeded");
        return NAN;
    }
    return (a + b) / 2;
}

int bisect_multiple(Function f, double a, double b, double tol, double *roots, int max_roots, Trap *traps)
{
    int num_roots = 0;
    double step = (b - a) / max_roots;
    for (int i = 0; i < max_roots && num_roots < max_roots; i++)
    {
        double sub_a = a + i * step;
        double sub_b = sub_a + step;
        if (sub_b > b) sub_b = b;
        if (f(sub_a) * f(sub_b) < 0)
        {
            Trap trap;
            double root = bisect(f, sub_a, sub_b, tol, &trap);
            traps[num_roots] = trap;
            if (!isnan(root))
            {
                roots[num_roots++] = root;
            }
        }
    }
    return num_roots;
}

int generate_path(Function f, double a, double b, int num_points, Point *path)
{
    if (num_points <= 0) return 0;
    double step = (b - a) / (num_points - 1);
    for (int i = 0; i < num_points; i++)
    {
        path[i].x = a + i * step;
        path[i].fx = f(path[i].x);
    }
    return num_points;
}

int link_bisections(Function f, double *roots, int num_roots, Point *path, int num_points, Point *linked, int max_linked)
{
    int num_linked = 0;
    double tol = 0.0001;
    for (int i = 0; i < num_roots && num_linked < max_linked; i++)
    {
        linked[num_linked].x = roots[i];
        linked[num_linked].fx = f(roots[i]);
        num_linked++;
    }
    for (int i = 0; i < num_points && num_linked < max_linked; i++)
    {
        int is_duplicate = 0;
        for (int j = 0; j < num_roots; j++)
        {
            if (fabs(path[i].x - roots[j]) < tol)
            {
                is_duplicate = 1;
                break;
            }
        }
        if (!is_duplicate)
        {
            linked[num_linked].x = path[i].x;
            linked[num_linked].fx = path[i].fx;
            num_linked++;
        }
    }
    return num_linked;
}

int generate_brink_list(Function f, double a, double b, int num_points, BrinkList *brink_list)
{
    if (num_points <= 0)
    {
        brink_list->points = NULL;
        brink_list->count = 0;
        return 0;
    }
    Point *temp = malloc(num_points * sizeof(Point));
    int count = 0;
    double step = (b - a) / (num_points - 1);
    for (int i = 0; i < num_points; i++)
    {
        double x = a + i * step;
        double fx = f(x);
        if (fx <= 0) // Link past of n with<=0
        {
            temp[count].x = x;
            temp[count].fx = fx;
            count++;
        }
    }
    brink_list->points = malloc(count * sizeof(Point));
    for (int i = 0; i < count; i++)
    {
        brink_list->points[i] = temp[i];
    }
    brink_list->count = count;
    free(temp);
    return count;
}