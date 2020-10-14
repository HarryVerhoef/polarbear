int fib(int n) {
    if (1 <= n <= 2) {
        return 1;
    }
    return fib(n - 1) + fib(n - 2);
};
int main(array<string> args) {
    int n = int(args[0]);
    out("The {n}th fibonacci number is: {fib(n)}");
    return 1;
};