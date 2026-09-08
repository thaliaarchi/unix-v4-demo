define NODES 220
define BINDS 32
define NAMELEN 12
define NUMBER 1
define SYMBOL 2
define PAIR 3

struct node {
    int type;
    int value;
    char name[NAMELEN];
    struct node *a;
    struct node *d;
};

struct binding {
    char name[NAMELEN];
    struct node *value;
};

struct node space[NODES];
struct binding env[BINDS];
char input[200];
int used;
int nenv;
int pos;

struct node *alloc();
struct node *mknum();
struct node *mksym();
struct node *cons();
struct node *parse();
struct node *eval();
struct node *apply();
struct node *lookup();
struct node *nth();

int same(a, b)
char *a, *b;
{
    while (*a == *b) {
        if (*a == 0)
            return(1);
        a++;
        b++;
    }
    return(0);
}

copy(a, b)
char *a, *b;
{
    int i;
    i = 0;
    while (b[i] && i < NAMELEN-1) {
        a[i] = b[i];
        i++;
    }
    a[i] = 0;
}

struct node *alloc()
{
    struct node *p;
    if (used >= NODES) {
        printf("out of nodes\n");
        return(0);
    }
    p = &space[used++];
    p->type = 0;
    p->value = 0;
    p->name[0] = 0;
    p->a = 0;
    p->d = 0;
    return(p);
}

struct node *mknum(v)
int v;
{
    struct node *p;
    p = alloc();
    if (p) {
        p->type = NUMBER;
        p->value = v;
    }
    return(p);
}

struct node *mksym(s)
char *s;
{
    struct node *p;
    p = alloc();
    if (p) {
        p->type = SYMBOL;
        copy(p->name, s);
    }
    return(p);
}

struct node *cons(a, d)
struct node *a, *d;
{
    struct node *p;
    p = alloc();
    if (p) {
        p->type = PAIR;
        p->a = a;
        p->d = d;
    }
    return(p);
}

skip()
{
    while (input[pos] == ' ' || input[pos] == '\t')
        pos++;
}

isdigit(c)
int c;
{
    return(c >= '0' && c <= '9');
}
struct node *parse()
{
    int c, i, sign, value;
    char word[NAMELEN];
    struct node *head, *tail, *x, *q;
    skip();
    c = input[pos];
    if (c == 0)
        return(0);
    if (c == '(') {
        pos++;
        head = 0;
        tail = 0;
        skip();
        while (input[pos] && input[pos] != ')') {
            x = parse();
            q = cons(x, 0);
            if (head == 0)
                head = q;
            else
                tail->d = q;
            tail = q;
            skip();
        }
        if (input[pos] == ')')
            pos++;
        return(head);
    }
    if (c == ')') {
        pos++;
        return(0);
    }
    if (isdigit(c) || (c == '-' && isdigit(input[pos+1]))) {
        sign = 1;
        if (c == '-') {
            sign = -1;
            pos++;
        }
        value = 0;
        while (isdigit(input[pos])) {
            value = value * 10 + input[pos] - '0';
            pos++;
        }
        return(mknum(sign * value));
    }
    i = 0;
    while ((c = input[pos]) && c != ' ' && c != '\t'
        && c != '(' && c != ')') {
        if (i < NAMELEN-1)
            word[i++] = c;
        pos++;
    }
    word[i] = 0;
    return(mksym(word));
}

struct node *lookup(s)
char *s;
{
    int i;
    i = nenv - 1;
    while (i >= 0) {
        if (same(env[i].name, s))
            return(env[i].value);
        i--;
    }
    printf("unbound symbol: %s\n", s);
    return(0);
}

bind(s, v)
char *s;
struct node *v;
{
    int i;
    i = 0;
    while (i < nenv) {
        if (same(env[i].name, s)) {
            env[i].value = v;
            return;
        }
        i++;
    }
    if (nenv >= BINDS) {
        printf("environment full\n");
        return;
    }
    copy(env[nenv].name, s);
    env[nenv].value = v;
    nenv++;
}

struct node *nth(p, n)
struct node *p;
int n;
{
    while (p && n > 0) {
        p = p->d;
        n--;
    }
    if (p && p->type == PAIR)
        return(p->a);
    return(0);
}

struct node *boolean(v)
int v;
{
    if (v)
        return(mksym("t"));
    return(0);
}

isnum(p)
struct node *p;
{
    if (p && p->type == NUMBER)
        return(1);
    printf("number required\n");
    return(0);
}

struct node *eval(x)
struct node *x;
{
    struct node *op, *args, *v, *name;
    if (x == 0)
        return(0);
    if (x->type == NUMBER)
        return(x);
    if (x->type == SYMBOL)
        return(lookup(x->name));
    op = x->a;
    args = x->d;
    if (op == 0 || op->type != SYMBOL) {
        printf("bad function\n");
        return(0);
    }
    if (same(op->name, "quote"))
        return(nth(args, 0));
    if (same(op->name, "define")) {
        name = nth(args, 0);
        if (name == 0 || name->type != SYMBOL) {
            printf("define needs a name\n");
            return(0);
        }
        v = eval(nth(args, 1));
        bind(name->name, v);
        return(v);
    }
    if (same(op->name, "if")) {
        v = eval(nth(args, 0));
        if (v)
            return(eval(nth(args, 1)));
        return(eval(nth(args, 2)));
    }
    return(apply(op, args));
}

struct node *apply(op, args)
struct node *op, *args;
{
    struct node *p, *x, *y;
    int v, first;
    if (same(op->name, "cons")) {
        x = eval(nth(args, 0));
        y = eval(nth(args, 1));
        return(cons(x, y));
    }
    if (same(op->name, "car")) {
        x = eval(nth(args, 0));
        if (x && x->type == PAIR)
            return(x->a);
        printf("car needs a list\n");
        return(0);
    }
    if (same(op->name, "cdr")) {
        x = eval(nth(args, 0));
        if (x && x->type == PAIR)
            return(x->d);
        printf("cdr needs a list\n");
        return(0);
    }
    if (same(op->name, "atom")) {
        x = eval(nth(args, 0));
        return(boolean(x == 0 || x->type != PAIR));
    }
    if (same(op->name, "+") || same(op->name, "*")) {
        if (same(op->name, "+"))
            v = 0;
        else
            v = 1;
        p = args;
        while (p) {
            x = eval(p->a);
            if (!isnum(x))
                return(0);
            if (same(op->name, "+"))
                v = v + x->value;
            else
                v = v * x->value;
            p = p->d;
        }
        return(mknum(v));
    }
    if (same(op->name, "-") || same(op->name, "/")) {
        p = args;
        if (p == 0) {
            printf("argument required\n");
            return(0);
        }
        x = eval(p->a);
        if (!isnum(x))
            return(0);
        v = x->value;
        p = p->d;
        if (p == 0 && same(op->name, "-"))
            return(mknum(-v));
        while (p) {
            x = eval(p->a);
            if (!isnum(x))
                return(0);
            if (same(op->name, "-"))
                v = v - x->value;
            else {
                if (x->value == 0) {
                    printf("division by zero\n");
                    return(0);
                }
                v = v / x->value;
            }
            p = p->d;
        }
        return(mknum(v));
    }
    if (same(op->name, "=") || same(op->name, "<")) {
        x = eval(nth(args, 0));
        y = eval(nth(args, 1));
        if (!isnum(x) || !isnum(y))
            return(0);
        if (same(op->name, "="))
            return(boolean(x->value == y->value));
        return(boolean(x->value < y->value));
    }
    printf("unknown function: %s\n", op->name);
    return(0);
}

pr(x)
struct node *x;
{
    struct node *p;
    if (x == 0) {
        printf("()");
        return;
    }
    if (x->type == NUMBER) {
        printf("%d", x->value);
        return;
    }
    if (x->type == SYMBOL) {
        printf("%s", x->name);
        return;
    }
    printf("(");
    p = x;
    while (p && p->type == PAIR) {
        pr(p->a);
        p = p->d;
        if (p)
            printf(" ");
    }
    if (p) {
        printf(". ");
        pr(p);
    }
    printf(")");
}

main()
{
    struct node *x, *v;
    printf("tiny lisp for unix v4\n");
    printf("type quit to leave\n");
    while (1) {
        printf("> ");
        if (gets(input) == 0)
            break;
        if (same(input, "quit"))
            break;
        pos = 0;
        x = parse();
        skip();
        if (input[pos])
            printf("extra input ignored\n");
        v = eval(x);
        pr(v);
        printf("\n");
    }
    printf("bye\n");
}
