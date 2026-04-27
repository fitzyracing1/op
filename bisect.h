#ifndef BISECT_H
#define BISECT_H

typedef double (*Function)(double);

typedef struct {
    double x;
    double fx;
} Point;

typedef struct {
    int code; // 0: success, 1: invalid interval, 2: iteration limit, 3: precision issue, 4: invalid binary
    char message[100];
    int iterations;
} Trap;

typedef struct {
    Point *points;
    int count;
} BrinkList;

double bisect(Function f, double a, double b, double tol, Trap *trap);
int bisect_multiple(Function f, double a, double b, double tol, double *roots, int max_roots, Trap *traps);
int generate_path(Function f, double a, double b, int num_points, Point *path);
int link_bisections(Function f, double *roots, int num_roots, Point *path, int num_points, Point *linked, int max_linked);
int generate_brink_list(Function f, double a, double b, int num_points, BrinkList *brink_list);

#endif