#include <blz.c>

int main(int argc, char**argv) {
    if (argc < 2) {
        printf("usage: %s filename", argv[0]);
        return 1;
    }
    FILE* inf = fopen(argv[1], "rb");
    fseek(inf, 0, 2);

    int buflen = ftell(inf);
    fseek(inf, 0, 0);

    unsigned char buf[buflen];
    fread(buf, buflen, buflen, inf);
    fclose(inf);

    int newlen = 0;
    unsigned char *newbuf = BLZ_Code(buf, buflen, &newlen, 0);

    char outfn[strlen(argv[1]) + 6];
    sprintf(outfn, "%s.lzss", argv[1]);

    FILE *outf = fopen(outfn, "wb");
    fwrite(newbuf, 1, newlen, outf);
}
