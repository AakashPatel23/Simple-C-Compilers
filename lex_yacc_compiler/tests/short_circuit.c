void main () {
	long* v;
	v = malloc(800);
	v[0] = 0;
	v[1] = 1;
  long* kota;
  kota = 0;
  printf("SHORT CIRCUIT AND: %d\nSHORT CIRCUIT OR: %d\n", v[0] && kota[-10], v[1] || kota[24]);
}
