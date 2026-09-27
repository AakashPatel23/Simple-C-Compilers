long r;
void main()
{
    long a;
    a = 1;
    printf("%d\n", a ? 69 : 0);

    printf("%d\n", a > 5 ? 0 : 69);
    
    long b;
    r = a == 1 ? 69 : 0;
    printf("%d\n", r);

    b = a < 5 ? 69 : 0;
    printf("%d\n", b);
    
		long* c;
    c = malloc(800);
    c[69] = 420;
    b = c[a != 1? 0 : 69];
		c[a != 1? 0 : 69] = 69;
		printf("%d\n", b);
		printf("%d\n", c[69]);
	  printf("%d\n", (a ? 1 : 0) ? 420 : 0);
}
