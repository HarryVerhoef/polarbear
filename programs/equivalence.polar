/* f(x) = x^2 + 3x + 2 */
int f(int x) {
    return (x ** 2) + (3 * x) + 2;
}

/* g(x) = x(x - 1) */
int g(int x) {
    return x * (x - 1);
}

/* h(x) = x + 2 */
int h(int x) {
    return x + 2;
}

int main() {
    out("f is equivalent to g.h: {f <==> g.h}");
    return 1
}