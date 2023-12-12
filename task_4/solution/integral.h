struct args_struct
{
    double start;
    double end;
    double (*func)(double);
    int n_points;
};

double calculate_time(int n_threads);