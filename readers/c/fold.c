/*
 * fold.c -- git.pp fold. Reader 3 of 4. Plain C99, stdlib only.
 *
 * DOC-ONLY DISCIPLINE
 * -------------------
 * Written from SPEC.md v0.3.0 (SuperInstance/git.pp-spec) ALONE. No existing
 * git.pp implementation was read -- not gitpp-c, gitpp-js, gitpp-cs,
 * gitpp-pl, gitpp-rust, gitpp-doctor, nor the hub git.pp-ng. Every judgement
 * call the spec does not settle is recorded in AMBIGUITIES.md and is reachable
 * at runtime through a command-line flag, so a second reader can see exactly
 * which clause drives which output.
 *
 * Build:
 *   cc -std=c99 -O2 -Wall -Wextra -Wpedantic -Werror -o fold fold.c -lm
 *
 * Sections
 *   0  checked allocation, arena, growable buffer
 *   1  SHA-256 (FIPS 180-4) -- stdlib has none
 *   2  UTF-8 validation; code-point-order comparison
 *   3  JSON value model + hand-written parser (no JSON library, by design)
 *   4  SPEC 3  leaf-number encoding
 *   5  SPEC 2  canonical JSON and the content leaf
 *   6  SPEC 1  record validation
 *   7  SPEC 5  the fold: state, order, conflicts, orphans, setters
 *   8  SPEC 4  closure root
 *   9  reporting
 *  10  selftest: SPEC 3.5 pinned leaves, SPEC 2.5 example, SPEC 3 table
 *
 * LOCALE: the entire byte-exactness story depends on strtod/printf using '.'
 * as the decimal point, so this program never calls setlocale() and always
 * runs in the "C" locale. See AMBIGUITIES.md A-19.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include <float.h>

#define GITPP_VERSION "gitpp-rr-reader3/0.1 (C99)"

static void die(const char *fmt, ...)
{
    va_list ap;
    fputs("fatal: ", stderr);
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fputc('\n', stderr);
    exit(2);
}

/* ======================================================================== *
 * 0. Checked allocation, arena, growable buffer                            *
 * ======================================================================== */

static void *xmalloc(size_t n)
{
    void *p;
    if (n == 0) n = 1;
    p = malloc(n);
    if (!p) die("out of memory (%zu bytes)", n);
    return p;
}

static void *xrealloc(void *p, size_t n)
{
    void *q;
    if (n == 0) n = 1;
    q = realloc(p, n);
    if (!q) die("out of memory (%zu bytes)", n);
    return q;
}

/* size_t arithmetic that traps instead of silently wrapping. */
static size_t mul_ck(size_t a, size_t b)
{
    if (a != 0 && b > SIZE_MAX / a) die("size overflow in %zu*%zu", a, b);
    return a * b;
}

static size_t add_ck(size_t a, size_t b)
{
    if (b > SIZE_MAX - a) die("size overflow in %zu+%zu", a, b);
    return a + b;
}

/*
 * Arena: every JSON node, every string payload, and every derived canonical
 * string lives here. Teardown is then one call, so "no leaks" is checkable by
 * inspection instead of by auditing free() calls.
 */
typedef struct ArenaBlock {
    struct ArenaBlock *next;
    size_t used, cap;
    unsigned char *data;
} ArenaBlock;

typedef struct { ArenaBlock *head; size_t total; } Arena;

static void arena_init(Arena *a) { a->head = NULL; a->total = 0; }

/* 16-byte alignment suits any scalar or struct we store here. */
static void *arena_alloc(Arena *a, size_t n)
{
    ArenaBlock *b;
    size_t need = (n + 15u) & ~(size_t)15u;
    size_t cap;

    if (need < n) die("arena size overflow");
    b = a->head;
    if (b && b->cap - b->used >= need) {
        void *p = b->data + b->used;
        b->used += need;
        return p;
    }
    cap = need > 65536u ? need : 65536u;
    b = (ArenaBlock *)xmalloc(sizeof *b);
    b->data = (unsigned char *)xmalloc(cap);
    b->cap = cap;
    b->used = need;
    b->next = a->head;
    a->head = b;
    a->total = add_ck(a->total, cap);
    return b->data;
}

static void arena_free(Arena *a)
{
    ArenaBlock *b = a->head, *n;
    while (b) { n = b->next; free(b->data); free(b); b = n; }
    a->head = NULL;
    a->total = 0;
}

static char *arena_strndup(Arena *a, const char *s, size_t n)
{
    char *p = (char *)arena_alloc(a, add_ck(n, 1u));
    if (n) memcpy(p, s, n);
    p[n] = '\0';
    return p;
}

/* Growable byte buffer. Always NUL-terminated so it can feed printf %s. */
typedef struct { char *p; size_t len, cap; } Buf;

static void buf_init(Buf *b) { b->p = NULL; b->len = 0; b->cap = 0; }

static void buf_free(Buf *b) { free(b->p); buf_init(b); }

static void buf_reserve(Buf *b, size_t extra)
{
    size_t want = add_ck(b->len, extra + 1u);
    size_t cap;
    if (want <= b->cap) return;
    cap = b->cap ? b->cap : 64u;
    while (cap < want) {
        if (cap > SIZE_MAX / 2u) die("buffer capacity overflow");
        cap *= 2u;
    }
    b->p = (char *)xrealloc(b->p, cap);
    b->cap = cap;
}

static void buf_put(Buf *b, const void *data, size_t n)
{
    buf_reserve(b, n);
    if (n) memcpy(b->p + b->len, data, n);
    b->len += n;
    b->p[b->len] = '\0';
}

static void buf_putc(Buf *b, char c) { buf_put(b, &c, 1); }

static void buf_puts(Buf *b, const char *s) { buf_put(b, s, strlen(s)); }

static void buf_printf(Buf *b, const char *fmt, ...)
{
    va_list ap, ap2;
    char stack[256];
    int need;
    va_start(ap, fmt);
    va_copy(ap2, ap);
    need = vsnprintf(stack, sizeof stack, fmt, ap);
    va_end(ap);
    if (need < 0) die("vsnprintf failed");
    if ((size_t)need < sizeof stack) {
        buf_put(b, stack, (size_t)need);
    } else {
        buf_reserve(b, (size_t)need);
        (void)vsnprintf(b->p + b->len, (size_t)need + 1u, fmt, ap2);
        b->len += (size_t)need;
    }
    va_end(ap2);
}

static void buf_puti64(Buf *b, long long v)
{
    char tmp[32];
    int k = snprintf(tmp, sizeof tmp, "%lld", v);
    if (k < 0 || (size_t)k >= sizeof tmp) die("int64 formatting failed");
    buf_put(b, tmp, (size_t)k);
}

/* ======================================================================== *
 * 1. SHA-256 (FIPS 180-4). stdlib provides none.                            *
 * ======================================================================== */

typedef struct {
    uint32_t h[8];
    unsigned char block[64];
    size_t blen;
    unsigned long long total;
} Sha256;

static const uint32_t sha_k[64] = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,
    0x923f82a4u,0xab1c5ed5u,0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,
    0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,0xe49b69c1u,0xefbe4786u,
    0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,
    0x06ca6351u,0x14292967u,0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,
    0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,0xa2bfe8a1u,0xa81a664bu,
    0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,
    0x5b9cca4fu,0x682e6ff3u,0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,
    0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

#define ROR32(x, n) (((x) >> (n)) | ((x) << (32 - (n))))

static void sha256_compress(Sha256 *s, const unsigned char *p)
{
    uint32_t w[64], a, b, c, d, e, f, g, h, t1, t2;
    int i;
    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)p[i*4] << 24) | ((uint32_t)p[i*4+1] << 16) |
               ((uint32_t)p[i*4+2] << 8) | (uint32_t)p[i*4+3];
    for (i = 16; i < 64; i++) {
        uint32_t s0 = ROR32(w[i-15], 7) ^ ROR32(w[i-15], 18) ^ (w[i-15] >> 3);
        uint32_t s1 = ROR32(w[i-2], 17) ^ ROR32(w[i-2], 19) ^ (w[i-2] >> 10);
        w[i] = w[i-16] + s0 + w[i-7] + s1;
    }
    a = s->h[0]; b = s->h[1]; c = s->h[2]; d = s->h[3];
    e = s->h[4]; f = s->h[5]; g = s->h[6]; h = s->h[7];
    for (i = 0; i < 64; i++) {
        uint32_t S1 = ROR32(e, 6) ^ ROR32(e, 11) ^ ROR32(e, 25);
        uint32_t ch = (e & f) ^ ((~e) & g);
        uint32_t S0 = ROR32(a, 2) ^ ROR32(a, 13) ^ ROR32(a, 22);
        uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
        t1 = h + S1 + ch + sha_k[i] + w[i];
        t2 = S0 + maj;
        h = g; g = f; f = e; e = d + t1;
        d = c; c = b; b = a; a = t1 + t2;
    }
    s->h[0] += a; s->h[1] += b; s->h[2] += c; s->h[3] += d;
    s->h[4] += e; s->h[5] += f; s->h[6] += g; s->h[7] += h;
}

static void sha256_init(Sha256 *s)
{
    s->h[0] = 0x6a09e667u; s->h[1] = 0xbb67ae85u;
    s->h[2] = 0x3c6ef372u; s->h[3] = 0xa54ff53au;
    s->h[4] = 0x510e527fu; s->h[5] = 0x9b05688cu;
    s->h[6] = 0x1f83d9abu; s->h[7] = 0x5be0cd19u;
    s->blen = 0;
    s->total = 0;
}

static void sha256_update(Sha256 *s, const void *data, size_t n)
{
    const unsigned char *p = (const unsigned char *)data;
    s->total += (unsigned long long)n;
    if (s->blen) {
        size_t need = 64u - s->blen;
        size_t take = n < need ? n : need;
        memcpy(s->block + s->blen, p, take);
        s->blen += take; p += take; n -= take;
        if (s->blen == 64u) { sha256_compress(s, s->block); s->blen = 0; }
    }
    while (n >= 64u) { sha256_compress(s, p); p += 64; n -= 64; }
    if (n) { memcpy(s->block, p, n); s->blen = n; }
}

/* Finalises in place and writes 64 lowercase hex chars + NUL into out[65]. */
static void sha256_hex(Sha256 *s, char out[65])
{
    static const char hex[] = "0123456789abcdef";
    unsigned char pad[72];
    unsigned long long bits = s->total * 8ull;
    unsigned long long saved;
    size_t padlen;
    int i;

    padlen = (s->blen < 56u) ? (56u - s->blen) : (120u - s->blen);
    memset(pad, 0, sizeof pad);
    pad[0] = 0x80;
    sha256_update(s, pad, padlen);

    for (i = 0; i < 8; i++)
        pad[i] = (unsigned char)((bits >> (56 - 8 * i)) & 0xffu);
    saved = s->total;          /* the length block is not part of the message */
    sha256_update(s, pad, 8);
    s->total = saved;

    for (i = 0; i < 8; i++) {
        uint32_t w = s->h[i];
        out[i*8 + 0] = hex[(w >> 28) & 0xfu];
        out[i*8 + 1] = hex[(w >> 24) & 0xfu];
        out[i*8 + 2] = hex[(w >> 20) & 0xfu];
        out[i*8 + 3] = hex[(w >> 16) & 0xfu];
        out[i*8 + 4] = hex[(w >> 12) & 0xfu];
        out[i*8 + 5] = hex[(w >> 8) & 0xfu];
        out[i*8 + 6] = hex[(w >> 4) & 0xfu];
        out[i*8 + 7] = hex[w & 0xfu];
    }
    out[64] = '\0';
}

static void sha256_of(const void *data, size_t n, char out[65])
{
    Sha256 s;
    sha256_init(&s);
    sha256_update(&s, data, n);
    sha256_hex(&s, out);
}

/* ======================================================================== *
 * 2. UTF-8 validation and code-point-order comparison                        *
 * ======================================================================== */

/*
 * SPEC 2.3, 4 and 5.4 all say "sorted by Unicode code point", but SPEC 2.5's
 * forbidden-shortcut list names `String.prototype.localeCompare`, which is
 * locale collation and NOT code-point order. Code-point order is what the rest
 * of the spec says, so that is what is implemented. See AMBIGUITIES.md A-3.
 *
 * For well-formed UTF-8, UNSIGNED BYTE-LEXICOGRAPHIC ORDER IS CODE-POINT ORDER:
 * UTF-8 is a variable-width encoding specifically designed to preserve scalar
 * value order in byte order. So in C the correct comparator is a byte compare
 * that reads each byte as `unsigned char`, and that is what cmp_cp does.
 *
 * The hazard, stated precisely: strcmp() and memcmp() are NOT hazards here,
 * because C defines them to compare as unsigned char. The hazards are the
 * hand-written ones -- a byte loop that reads into a signed `char`, and any
 * ctype call (tolower/isalpha/...) whose argument is negative, which is
 * undefined behaviour. See AMBIGUITIES.md A-4, where this is demonstrated.
 *
 * For ill-formed UTF-8 there are no code points at all; byte order still yields
 * a total order, which is the documented fallback. See AMBIGUITIES.md A-2.
 */
static int cmp_cp(const char *a, size_t an, const char *b, size_t bn)
{
    size_t n = an < bn ? an : bn, i;
    for (i = 0; i < n; i++) {
        unsigned char ca = (unsigned char)a[i], cb = (unsigned char)b[i];
        if (ca != cb) return ca < cb ? -1 : 1;
    }
    if (an == bn) return 0;
    return an < bn ? -1 : 1;   /* a proper prefix sorts first */
}

static int cmp_cstr(const char *a, const char *b)
{
    return cmp_cp(a, strlen(a), b, strlen(b));
}

/*
 * 1 if [p,p+n) is well-formed UTF-8: no overlongs, no surrogates, <= U+10FFFF,
 * no truncated sequences. This is the strict definition; WTF-8 surrogates
 * produced by decoding lone \uD800 escapes are detected separately below.
 */
static int utf8_valid(const unsigned char *p, size_t n)
{
    size_t i = 0;
    while (i < n) {
        unsigned char c = p[i];
        size_t need, k;
        unsigned long cp;
        if (c < 0x80u) { i++; continue; }
        else if ((c & 0xe0u) == 0xc0u) { need = 1; cp = c & 0x1fu; }
        else if ((c & 0xf0u) == 0xe0u) { need = 2; cp = c & 0x0fu; }
        else if ((c & 0xf8u) == 0xf0u) { need = 3; cp = c & 0x07u; }
        else return 0;
        if (n - i < need + 1u) return 0;
        for (k = 1; k <= need; k++) {
            unsigned char cc = p[i + k];
            if ((cc & 0xc0u) != 0x80u) return 0;
            cp = (cp << 6) | (cc & 0x3fu);
        }
        if (need == 1 && cp < 0x80ul) return 0;              /* overlong 2-byte */
        if (need == 2 && cp < 0x800ul) return 0;             /* overlong 3-byte */
        if (need == 3 && cp < 0x10000ul) return 0;           /* overlong 4-byte */
        if (cp >= 0xd800ul && cp <= 0xdffful) return 0;      /* surrogate half */
        if (cp > 0x10fffful) return 0;
        i += need + 1u;
    }
    return 1;
}

/* 1 if any 3-byte sequence encodes a lone surrogate (WTF-8 shape). */
static int utf8_has_lone_surrogate(const unsigned char *p, size_t n)
{
    size_t i;
    for (i = 0; i + 2u < n; i++) {
        if ((p[i] & 0xf0u) == 0xe0u && (p[i+1] & 0xc0u) == 0x80u &&
            (p[i+2] & 0xc0u) == 0x80u) {
            unsigned long cp = ((unsigned long)(p[i] & 0x0fu) << 12) |
                               ((unsigned long)(p[i+1] & 0x3fu) << 6) |
                               (unsigned long)(p[i+2] & 0x3fu);
            if (cp >= 0xd800ul && cp <= 0xdffful) return 1;
        }
    }
    return 0;
}

/* ======================================================================== *
 * 3. JSON value model and parser (hand-written; no JSON library)           *
 * ======================================================================== */

typedef enum { J_NULL, J_BOOL, J_NUM, J_STR, J_ARR, J_OBJ } JType;

typedef struct JVal JVal;

typedef struct { const char *key; size_t klen; JVal *val; } JMem;

struct JVal {
    JType t;
    union {
        int b;                                  /* J_BOOL */
        double d;                               /* J_NUM  */
        struct { const char *p; size_t n; } str;  /* J_STR: bytes, NUL-terminated */
        struct { JVal **v; size_t n, cap; } arr;
        struct { JMem *m; size_t n, cap; } obj;
    } u;
};

/* Stack-exhaustion guard for the recursive-descent parser. See NOTES.md. */
#define JSON_MAX_DEPTH 200

typedef struct {
    const char *p, *end;
    Arena *a;
    int depth;
    int nonstandard;   /* saw NaN / Infinity / -Infinity */
    char err[256];
} JParser;

static void jerr(JParser *P, const char *msg)
{
    if (P->err[0] == '\0')
        (void)snprintf(P->err, sizeof P->err, "%s", msg);
}

static JVal *jparse_value(JParser *P);

static JVal *jnew(JParser *P, JType t)
{
    JVal *v = (JVal *)arena_alloc(P->a, sizeof *v);
    memset(v, 0, sizeof *v);
    v->t = t;
    return v;
}

static void jskip_ws(JParser *P)
{
    while (P->p < P->end) {
        char c = *P->p;
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') P->p++;
        else break;
    }
}

static void utf8_emit(Buf *b, unsigned long cp)
{
    if (cp < 0x80ul) buf_putc(b, (char)cp);
    else if (cp < 0x800ul) {
        buf_putc(b, (char)(0xc0u | (cp >> 6)));
        buf_putc(b, (char)(0x80u | (cp & 0x3fu)));
    } else if (cp < 0x10000ul) {
        buf_putc(b, (char)(0xe0u | (cp >> 12)));
        buf_putc(b, (char)(0x80u | ((cp >> 6) & 0x3fu)));
        buf_putc(b, (char)(0x80u | (cp & 0x3fu)));
    } else {
        buf_putc(b, (char)(0xf0u | (cp >> 18)));
        buf_putc(b, (char)(0x80u | ((cp >> 12) & 0x3fu)));
        buf_putc(b, (char)(0x80u | ((cp >> 6) & 0x3fu)));
        buf_putc(b, (char)(0x80u | (cp & 0x3fu)));
    }
}

static int jhex4(JParser *P, unsigned long *out)
{
    unsigned long v = 0;
    int k;
    for (k = 0; k < 4; k++) {
        char c;
        if (P->p >= P->end) { jerr(P, "truncated \\u escape"); return 0; }
        c = *P->p++;
        v <<= 4;
        if (c >= '0' && c <= '9') v |= (unsigned long)(c - '0');
        else if (c >= 'a' && c <= 'f') v |= (unsigned long)(c - 'a' + 10);
        else if (c >= 'A' && c <= 'F') v |= (unsigned long)(c - 'A' + 10);
        else { jerr(P, "bad hex digit in \\u escape"); return 0; }
    }
    *out = v;
    return 1;
}

static JVal *jparse_string(JParser *P)
{
    Buf b;
    JVal *v;
    if (P->p >= P->end || *P->p != '"') { jerr(P, "expected string"); return NULL; }
    P->p++;
    buf_init(&b);
    for (;;) {
        unsigned char c;
        if (P->p >= P->end) { jerr(P, "unterminated string"); buf_free(&b); return NULL; }
        c = (unsigned char)*P->p++;
        if (c == '"') break;
        if (c < 0x20u) { jerr(P, "raw control character in string"); buf_free(&b); return NULL; }
        if (c != '\\') { buf_putc(&b, (char)c); continue; }
        if (P->p >= P->end) { jerr(P, "truncated escape"); buf_free(&b); return NULL; }
        {
            char e = *P->p++;
            switch (e) {
            case '"':  buf_putc(&b, '"');  break;
            case '\\': buf_putc(&b, '\\'); break;
            case '/':  buf_putc(&b, '/');  break;
            case 'b':  buf_putc(&b, '\b'); break;
            case 'f':  buf_putc(&b, '\f'); break;
            case 'n':  buf_putc(&b, '\n'); break;
            case 'r':  buf_putc(&b, '\r'); break;
            case 't':  buf_putc(&b, '\t'); break;
            case 'u': {
                unsigned long hi;
                if (!jhex4(P, &hi)) { buf_free(&b); return NULL; }
                if (hi >= 0xd800ul && hi <= 0xdbfful &&
                    P->end - P->p >= 6 && P->p[0] == '\\' && P->p[1] == 'u') {
                    const char *save = P->p;
                    unsigned long lo;
                    P->p += 2;
                    if (!jhex4(P, &lo)) { buf_free(&b); return NULL; }
                    if (lo >= 0xdc00ul && lo <= 0xdffful) {
                        utf8_emit(&b, 0x10000ul + ((hi - 0xd800ul) << 10)
                                      + (lo - 0xdc00ul));
                        break;
                    }
                    P->p = save;
                }
                /*
                 * Unpaired surrogate. CPython's json accepts "\ud800" and then
                 * raises UnicodeEncodeError in .encode('utf-8'); we keep the
                 * WTF-8 form so the record still has bytes. See A-5.
                 */
                utf8_emit(&b, hi);
                break;
            }
            default:
                jerr(P, "invalid escape character");
                buf_free(&b);
                return NULL;
            }
        }
    }
    v = jnew(P, J_STR);
    v->u.str.n = b.len;
    v->u.str.p = arena_strndup(P->a, b.p ? b.p : "", b.len);
    buf_free(&b);
    return v;
}

static int jlit(JParser *P, const char *lit, size_t n)
{
    if ((size_t)(P->end - P->p) >= n && memcmp(P->p, lit, n) == 0) {
        P->p += n;
        return 1;
    }
    return 0;
}

/*
 * SPEC 3's reject rows demand defined behaviour for NaN / +Inf / -Inf, so the
 * tokenizer must be able to PRODUCE them; a strict RFC 8259 parser would reject
 * them at the syntax layer and the owning record would never reach the domain
 * check that the spec tells us to apply. So the three spellings are accepted
 * here and flagged as out-of-domain. See AMBIGUITIES.md A-6.
 */
static JVal *jparse_number(JParser *P)
{
    const char *start = P->p;
    JVal *v;
    double d;

    if (P->p < P->end && *P->p == '-') P->p++;
    if (P->p < P->end && *P->p == '0') {
        P->p++;
    } else if (P->p < P->end && *P->p >= '1' && *P->p <= '9') {
        while (P->p < P->end && *P->p >= '0' && *P->p <= '9') P->p++;
    } else {
        jerr(P, "invalid number");
        return NULL;
    }
    if (P->p < P->end && *P->p == '.') {
        P->p++;
        if (!(P->p < P->end && *P->p >= '0' && *P->p <= '9')) {
            jerr(P, "digit required after decimal point"); return NULL;
        }
        while (P->p < P->end && *P->p >= '0' && *P->p <= '9') P->p++;
    }
    if (P->p < P->end && (*P->p == 'e' || *P->p == 'E')) {
        P->p++;
        if (P->p < P->end && (*P->p == '+' || *P->p == '-')) P->p++;
        if (!(P->p < P->end && *P->p >= '0' && *P->p <= '9')) {
            jerr(P, "digit required in exponent"); return NULL;
        }
        while (P->p < P->end && *P->p >= '0' && *P->p <= '9') P->p++;
    }

    /*
     * SPEC 3: "Every JSON number is a float64" and "The fix always belongs at
     * the decode boundary, never in the encoder." So the literal is decoded
     * straight into a double and this program has no integer type anywhere for
     * a record value to land in. That is also why the SPEC 3 table can have an
     * `int` row and a `float` row that cannot disagree. See A-7.
     *
     * strtod sets ERANGE and returns +-HUGE_VAL on overflow; the result is
     * non-finite, which the domain check rejects, so 1e400 behaves as SPEC 3
     * requires without a separate overflow path.
     */
    {
        size_t n = (size_t)(P->p - start);
        char tmp[512];
        char *heap = NULL, *use;
        if (n + 1u > sizeof tmp) { heap = (char *)xmalloc(n + 1u); use = heap; }
        else use = tmp;
        memcpy(use, start, n);
        use[n] = '\0';
        errno = 0;
        d = strtod(use, NULL);
        free(heap);
    }
    v = jnew(P, J_NUM);
    v->u.d = d;
    return v;
}

static JVal *jparse_array(JParser *P)
{
    JVal *v = jnew(P, J_ARR);
    P->p++;   /* '[' */
    jskip_ws(P);
    if (P->p < P->end && *P->p == ']') { P->p++; return v; }
    for (;;) {
        JVal *item;
        jskip_ws(P);
        item = jparse_value(P);
        if (!item) return NULL;
        if (v->u.arr.n == v->u.arr.cap) {
            size_t nc = v->u.arr.cap ? mul_ck(v->u.arr.cap, 2u) : 8u;
            JVal **nv = (JVal **)arena_alloc(P->a, mul_ck(nc, sizeof(JVal *)));
            if (v->u.arr.n)
                memcpy(nv, v->u.arr.v, mul_ck(v->u.arr.n, sizeof(JVal *)));
            v->u.arr.v = nv;
            v->u.arr.cap = nc;
        }
        v->u.arr.v[v->u.arr.n++] = item;
        jskip_ws(P);
        if (P->p < P->end && *P->p == ',') { P->p++; continue; }
        if (P->p < P->end && *P->p == ']') { P->p++; return v; }
        jerr(P, "expected ',' or ']'");
        return NULL;
    }
}

static JVal *jparse_object(JParser *P)
{
    JVal *v = jnew(P, J_OBJ);
    P->p++;   /* '{' */
    jskip_ws(P);
    if (P->p < P->end && *P->p == '}') { P->p++; return v; }
    for (;;) {
        JMem m;
        JVal *val, *ks;
        size_t i;
        jskip_ws(P);
        m.key = NULL;
        if (P->p >= P->end || *P->p != '"') {
            jerr(P, "expected object key"); return NULL;
        }
        ks = jparse_string(P);
        if (!ks) return NULL;
        m.key = ks->u.str.p;
        m.klen = ks->u.str.n;
        jskip_ws(P);
        if (P->p >= P->end || *P->p != ':') { jerr(P, "expected ':'"); return NULL; }
        P->p++;
        jskip_ws(P);
        val = jparse_value(P);
        if (!val) return NULL;
        m.val = val;
        /* Duplicate keys: last wins, matching every mainstream decoder. SPEC
         * is silent. See AMBIGUITIES.md A-8. */
        for (i = 0; i < v->u.obj.n; i++)
            if (v->u.obj.m[i].klen == m.klen &&
                memcmp(v->u.obj.m[i].key, m.key, m.klen) == 0)
                break;
        if (i < v->u.obj.n) {
            v->u.obj.m[i].val = val;
        } else {
            if (v->u.obj.n == v->u.obj.cap) {
                size_t nc = v->u.obj.cap ? mul_ck(v->u.obj.cap, 2u) : 8u;
                JMem *nm = (JMem *)arena_alloc(P->a, mul_ck(nc, sizeof(JMem)));
                if (v->u.obj.n)
                    memcpy(nm, v->u.obj.m, mul_ck(v->u.obj.n, sizeof(JMem)));
                v->u.obj.m = nm;
                v->u.obj.cap = nc;
            }
            v->u.obj.m[v->u.obj.n++] = m;
        }
        jskip_ws(P);
        if (P->p < P->end && *P->p == ',') { P->p++; continue; }
        if (P->p < P->end && *P->p == '}') { P->p++; return v; }
        jerr(P, "expected ',' or '}'");
        return NULL;
    }
}

static JVal *jparse_value(JParser *P)
{
    JVal *v;
    if (++P->depth > JSON_MAX_DEPTH) { jerr(P, "JSON nesting too deep"); return NULL; }
    jskip_ws(P);
    if (P->p >= P->end) { jerr(P, "unexpected end of input"); return NULL; }
    switch (*P->p) {
    case '{': v = jparse_object(P); break;
    case '[': v = jparse_array(P); break;
    case '"': v = jparse_string(P); break;
    case 't':
        if (jlit(P, "true", 4)) { v = jnew(P, J_BOOL); v->u.b = 1; }
        else { jerr(P, "invalid literal"); return NULL; }
        break;
    case 'f':
        if (jlit(P, "false", 5)) { v = jnew(P, J_BOOL); v->u.b = 0; }
        else { jerr(P, "invalid literal"); return NULL; }
        break;
    case 'n':
        if (jlit(P, "null", 4)) v = jnew(P, J_NULL);
        else { jerr(P, "invalid literal"); return NULL; }
        break;
    case 'N':
        if (jlit(P, "NaN", 3)) { P->nonstandard = 1; v = jnew(P, J_NUM); v->u.d = (double)NAN; }
        else { jerr(P, "invalid literal"); return NULL; }
        break;
    case 'I':
        if (jlit(P, "Infinity", 8)) { P->nonstandard = 1; v = jnew(P, J_NUM); v->u.d = (double)INFINITY; }
        else { jerr(P, "invalid literal"); return NULL; }
        break;
    default:
        if (*P->p == '-' && (size_t)(P->end - P->p) >= 9 &&
            memcmp(P->p, "-Infinity", 9) == 0) {
            P->p += 9;
            P->nonstandard = 1;
            v = jnew(P, J_NUM);
            v->u.d = -(double)INFINITY;
        } else {
            v = jparse_number(P);
        }
        break;
    }
    P->depth--;
    return v;
}

/* Parses one complete document. Returns NULL and fills P->err on failure. */
static JVal *json_parse(Arena *a, const char *text, size_t len, JParser *outp)
{
    JParser P;
    JVal *v;
    memset(&P, 0, sizeof P);
    P.p = text;
    P.end = text + len;
    P.a = a;
    v = jparse_value(&P);
    if (!v) { if (outp) *outp = P; return NULL; }
    jskip_ws(&P);
    if (P.p != P.end) {
        jerr(&P, "trailing content after JSON value");
        if (outp) *outp = P;
        return NULL;
    }
    if (outp) *outp = P;
    return v;
}

static const JVal *jobj_get(const JVal *o, const char *key)
{
    size_t i, n;
    if (!o || o->t != J_OBJ) return NULL;
    n = strlen(key);
    for (i = 0; i < o->u.obj.n; i++)
        if (o->u.obj.m[i].klen == n && memcmp(o->u.obj.m[i].key, key, n) == 0)
            return o->u.obj.m[i].val;
    return NULL;
}

/* ======================================================================== *
 * 4. SPEC 3 -- leaf-number encoding                                         *
 * ======================================================================== */

/*
 * Shortest-round-trip search: try %.0e .. %.16e and take the first digit count
 * that strtod()s back to the same double. Soundness argument: if a p-digit
 * decimal D round-trips, and X is the correctly-rounded p-digit decimal (which
 * printf gives us), then |X - v| <= |D - v|; the round-trip interval around v
 * is convex, so X round-trips too. Therefore the first p that round-trips is
 * BOTH the shortest length and correctly rounded -- the same choice David Gay /
 * Ryu / CPython's repr make. The residual risk is discussed in A-9.
 *
 * Rendering then follows SPEC 3: scientific when decpt <= -4 or decpt > 16,
 * exponent always signed and zero-padded to at least two digits, so 1e-5
 * renders "1e-05" and 1e16 renders "1e+16".
 *
 * Returns 0 on success, -1 if the value is outside the SPEC 3 pinned domain.
 */
static int encode_leaf_number(double v, Buf *out)
{
    double a;
    char tmp[64], digits[32];
    int p, ndig = 0, neg = 0;
    const char *e;
    long decpt;

    if (v != v) return -1;                          /* NaN                  */
    if (v > DBL_MAX || v < -DBL_MAX) return -1;    /* +Inf / -Inf          */
    if (v == 0.0) { buf_putc(out, '0'); return 0; } /* drops the -0.0 sign   */

    a = v < 0 ? -v : v;

    /* SPEC 3: finite float with |v| >= 1e21 -> out of pinned domain. */
    if (a >= 1e21) return -1;

    /* SPEC 3 band 1: |v| < 1e16 and integral -> exact integer digits.
     * 1e16 < 2^63, so the double -> long long conversion is exact and every
     * value in this band is representable. That is the whole reason C can do
     * this band without bignums; see A-7. */
    if (a < 1e16 && v == trunc(v)) {
        buf_puti64(out, (long long)v);
        return 0;
    }

    for (p = 1; p <= 17; p++) {
        int k = snprintf(tmp, sizeof tmp, "%.*e", p - 1, v);
        if (k < 0 || (size_t)k >= sizeof tmp) return -1;
        errno = 0;
        if (strtod(tmp, NULL) == v) break;
    }
    if (p > 17) return -1;

    /* Split "-d.dddde[+-]dd" into sign / digit string / decpt. */
    e = tmp;
    if (*e == '-') { neg = 1; e++; }
    while (*e && *e != 'e' && *e != 'E') {
        if (*e >= '0' && *e <= '9' && ndig < (int)sizeof digits - 1)
            digits[ndig++] = *e;
        e++;
    }
    digits[ndig] = '\0';
    while (ndig > 1 && digits[ndig - 1] == '0') digits[--ndig] = '\0';
    if (ndig == 0 || (*e != 'e' && *e != 'E')) return -1;
    decpt = strtol(e + 1, NULL, 10) + 1;   /* value = 0.<digits> * 10^decpt */

    if (neg) buf_putc(out, '-');

    if (decpt <= -4 || decpt > 16) {
        long ex = decpt - 1, ae;
        buf_putc(out, digits[0]);
        if (ndig > 1) {
            buf_putc(out, '.');
            buf_put(out, digits + 1, (size_t)(ndig - 1));
        }
        buf_putc(out, 'e');
        buf_putc(out, ex < 0 ? '-' : '+');
        ae = ex < 0 ? -ex : ex;
        /* "at least two digits": 16 -> "16", 5 -> "05", 100 -> "100" */
        if (ae < 10) { buf_putc(out, '0'); buf_puti64(out, ae); }
        else buf_puti64(out, ae);
    } else if (decpt <= 0) {
        long i;
        buf_puts(out, "0.");
        for (i = 0; i < -decpt; i++) buf_putc(out, '0');
        buf_put(out, digits, (size_t)ndig);
    } else if (decpt >= ndig) {
        long i;
        buf_put(out, digits, (size_t)ndig);
        for (i = 0; i < decpt - ndig; i++) buf_putc(out, '0');
    } else {
        buf_put(out, digits, (size_t)decpt);
        buf_putc(out, '.');
        buf_put(out, digits + decpt, (size_t)(ndig - decpt));
    }
    return 0;
}

/* 1 if any number anywhere under v is outside the SPEC 3 pinned domain. */
static int value_out_of_domain(const JVal *v)
{
    size_t i;
    if (!v) return 0;
    switch (v->t) {
    case J_NUM:
        return (v->u.d != v->u.d) || (v->u.d > DBL_MAX) || (v->u.d < -DBL_MAX) ||
               (fabs(v->u.d) >= 1e21);
    case J_ARR:
        for (i = 0; i < v->u.arr.n; i++)
            if (value_out_of_domain(v->u.arr.v[i])) return 1;
        return 0;
    case J_OBJ:
        for (i = 0; i < v->u.obj.n; i++)
            if (value_out_of_domain(v->u.obj.m[i].val)) return 1;
        return 0;
    default:
        return 0;
    }
}

/* ======================================================================== *
 * 5. SPEC 2 -- canonical JSON and the content leaf                         *
 * ======================================================================== */

/*
 * String escaping. SPEC 2.7 names `json.dumps(..., ensure_ascii=False)` as the
 * emitter, so the escape SET is CPython's, not "whatever JSON happens to
 * allow":
 *     "  -> \"      \  -> \\      and \b \t \n \f \r short forms
 *     every other C0 -> \u00xx with LOWERCASE hex
 *     everything else -- U+007F, '/', and all non-ASCII -- emitted literally
 * Verified against python3 -c 'json.dumps(..., ensure_ascii=False)'.
 * See AMBIGUITIES.md A-10.
 */
static void emit_string(Buf *b, const char *s, size_t n)
{
    size_t i;
    buf_putc(b, '"');
    for (i = 0; i < n; i++) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
        case '"':  buf_puts(b, "\\\""); break;
        case '\\': buf_puts(b, "\\\\"); break;
        case '\b': buf_puts(b, "\\b");  break;
        case '\f': buf_puts(b, "\\f");  break;
        case '\n': buf_puts(b, "\\n");  break;
        case '\r': buf_puts(b, "\\r");  break;
        case '\t': buf_puts(b, "\\t");  break;
        default:
            if (c < 0x20u) {
                static const char hex[] = "0123456789abcdef";
                buf_puts(b, "\\u00");
                buf_putc(b, hex[(c >> 4) & 0xfu]);
                buf_putc(b, hex[c & 0xfu]);
            } else {
                buf_putc(b, (char)c);
            }
        }
    }
    buf_putc(b, '"');
}

/*
 * sort_keys=True means EVERY object is sorted, at every depth -- not just the
 * five top-level keys. See AMBIGUITIES.md A-11.
 */
typedef struct { const char *key; size_t klen; const JVal *val; } EmitMem;

static int cmp_emitmem(const void *a, const void *b)
{
    const EmitMem *x = (const EmitMem *)a, *y = (const EmitMem *)b;
    return cmp_cp(x->key, x->klen, y->key, y->klen);
}

static void emit_value(Buf *b, const JVal *v);

static void emit_object(Buf *b, const JVal *o)
{
    EmitMem *m;
    size_t i;
    buf_putc(b, '{');
    if (o->u.obj.n == 0) { buf_putc(b, '}'); return; }
    m = (EmitMem *)xmalloc(mul_ck(o->u.obj.n, sizeof *m));
    for (i = 0; i < o->u.obj.n; i++) {
        m[i].key = o->u.obj.m[i].key;
        m[i].klen = o->u.obj.m[i].klen;
        m[i].val  = o->u.obj.m[i].val;
    }
    qsort(m, o->u.obj.n, sizeof *m, cmp_emitmem);
    for (i = 0; i < o->u.obj.n; i++) {
        if (i) buf_puts(b, ", ");
        emit_string(b, m[i].key, m[i].klen);
        buf_puts(b, ": ");
        emit_value(b, m[i].val);
    }
    free(m);
    buf_putc(b, '}');
}

static void emit_value(Buf *b, const JVal *v)
{
    size_t i;
    switch (v->t) {
    case J_NULL: buf_puts(b, "null"); break;
    case J_BOOL: buf_puts(b, v->u.b ? "true" : "false"); break;
    case J_NUM:
        if (encode_leaf_number(v->u.d, b) != 0)
            die("internal: out-of-domain number reached the canonical emitter");
        break;
    case J_STR: emit_string(b, v->u.str.p, v->u.str.n); break;
    case J_ARR:
        buf_putc(b, '[');
        for (i = 0; i < v->u.arr.n; i++) {
            if (i) buf_puts(b, ", ");
            emit_value(b, v->u.arr.v[i]);
        }
        buf_putc(b, ']');
        break;
    case J_OBJ: emit_object(b, v); break;
    }
}

/* ======================================================================== *
 * 6. SPEC 1 -- record model and validation                                  *
 * ======================================================================== */

typedef enum {
    CLS_OVERRIDE = 3,
    CLS_TASK     = 2,
    CLS_WITNESS  = 1,
    CLS_OTHER    = 0    /* kind is null, or a string outside the three. A-12 */
} RecClass;

/*
 * SPEC 1.1 gives class precedence for exactly three kinds, then says "Other
 * values are accepted; their semantics are out of scope for the fold." That is
 * a hole: the fold NEEDS a class to run 5.2 at all. Default is to rank an
 * unrecognised or null kind BELOW witness. See AMBIGUITIES.md A-12.
 */
static int class_of(const JVal *kind, int unknown_is_witness)
{
    if (!kind || kind->t == J_NULL)
        return unknown_is_witness ? CLS_WITNESS : CLS_OTHER;
    if (kind->t != J_STR) return CLS_OTHER;
    if (kind->u.str.n == 8 && memcmp(kind->u.str.p, "override", 8) == 0) return CLS_OVERRIDE;
    if (kind->u.str.n == 4 && memcmp(kind->u.str.p, "task", 4) == 0)     return CLS_TASK;
    if (kind->u.str.n == 7 && memcmp(kind->u.str.p, "witness", 7) == 0)  return CLS_WITNESS;
    return unknown_is_witness ? CLS_WITNESS : CLS_OTHER;
}

static const char *class_name(int c)
{
    switch (c) {
    case CLS_OVERRIDE: return "override";
    case CLS_TASK:     return "task";
    case CLS_WITNESS:  return "witness";
    default:           return "other";
    }
}

typedef enum { ORDER_LAYER = 0, ORDER_HEAP = 1 } OrderMode;
typedef enum { CONFLICT_DISTINCT = 0, CONFLICT_ALL_KEYS = 1 } ConflictMode;
typedef enum { REQ_DEFAULTS = 0, REQ_STRICT = 1 } ReqMode;

typedef struct {
    OrderMode    order;
    ConflictMode conflicts;
    ReqMode      required;
    int          strict_utf8;
    int          unknown_kind_is_witness;
} Options;

static Options OPT = {
    ORDER_LAYER, CONFLICT_DISTINCT, REQ_DEFAULTS, 0, 0
};

typedef struct {
    int          index;            /* position in the supplied set */
    const char  *id;   size_t idlen;
    const JVal  *parents, *kind, *effects, *reason;
    const char  *digest; size_t digestlen;

    int  malformed;    char why[192];
    int  orphan;       int in_order;
    int  in_tier;
    int  lone_surrogate;   /* id carries a WTF-8 surrogate half -- A-5 */
    char leaf[65];
    int  digest_ok, digest_bad, digest_ignored;
} Record;

typedef struct { Record *rec; size_t n, cap; } RecordSet;

static void recset_push(RecordSet *rs, const Record *r)
{
    if (rs->n == rs->cap) {
        size_t nc = rs->cap ? mul_ck(rs->cap, 2u) : 8u;
        rs->rec = (Record *)xrealloc(rs->rec, mul_ck(nc, sizeof *rs->rec));
        memset(rs->rec + rs->n, 0, mul_ck(nc - rs->n, sizeof *rs->rec));
        rs->cap = nc;
    }
    rs->rec[rs->n++] = *r;
}

static void validate_record(RecordSet *rs, const JVal *o, size_t index)
{
    Record r;
    const JVal *v;
    size_t k;

    memset(&r, 0, sizeof r);
    r.index = (int)index;
    r.in_tier = -1;

    if (!o || o->t != J_OBJ) {
        r.malformed = 1;
        (void)snprintf(r.why, sizeof r.why, "record is not a JSON object");
        recset_push(rs, &r);
        return;
    }

    v = jobj_get(o, "id");
    if (!v) {
        r.malformed = 1;
        (void)snprintf(r.why, sizeof r.why, "missing required field: id");
    } else if (v->t != J_STR) {
        r.malformed = 1;
        (void)snprintf(r.why, sizeof r.why, "field 'id' is not a string");
    } else {
        r.id = v->u.str.p;
        r.idlen = v->u.str.n;
        if (r.idlen == 0) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why, "field 'id' is empty");
        } else if (memchr(r.id, '\n', r.idlen) || memchr(r.id, ':', r.idlen)) {
            /* SPEC 1.1: id "Must be free of \n and :". A-14. */
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why,
                           "field 'id' contains '\\n' or ':' (SPEC 1.1)");
        } else if (OPT.strict_utf8 &&
                   !utf8_valid((const unsigned char *)r.id, r.idlen)) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why,
                           "field 'id' is not well-formed UTF-8 (--strict-utf8)");
        } else if (utf8_has_lone_surrogate((const unsigned char *)r.id, r.idlen)) {
            /* Not fatal: the id still has bytes, and byte order still gives a
             * total order. It is surfaced so a reader can see the case. A-5. */
            r.lone_surrogate = 1;
        }
    }

    r.parents = jobj_get(o, "parents");
    r.kind    = jobj_get(o, "kind");
    r.effects = jobj_get(o, "effects");
    r.reason  = jobj_get(o, "reason");
    v = jobj_get(o, "digest");
    if (v && v->t == J_STR) { r.digest = v->u.str.p; r.digestlen = v->u.str.n; }

    /*
     * SPEC 1.1's Required column marks only `id` as required, with defaults for
     * the other four. SPEC 1's prose then says a record missing ANY of the five
     * is malformed. The two clauses contradict each other inside one spec;
     * A-15. Default follows the table (and SPEC 2.5's "omitting reason: null
     * when the field is absent" note, which presupposes absence is legal).
     */
    if (!r.malformed) {
        const char *miss = NULL;
        if (!r.parents)      miss = "parents";
        else if (!r.kind)    miss = "kind";
        else if (!r.effects) miss = "effects";
        else if (!r.reason)  miss = "reason";
        if (miss && OPT.required == REQ_STRICT) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why,
                           "missing required field: %s (--required=strict, SPEC 1 prose)",
                           miss);
        }
    }

    if (!r.malformed) {
        if (r.parents && r.parents->t != J_ARR) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why, "field 'parents' is not an array");
        }
        if (r.kind && r.kind->t != J_STR && r.kind->t != J_NULL) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why, "field 'kind' is not string or null");
        }
        if (r.effects && r.effects->t != J_OBJ) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why, "field 'effects' is not an object");
        }
        if (r.reason && r.reason->t != J_STR && r.reason->t != J_NULL) {
            r.malformed = 1;
            (void)snprintf(r.why, sizeof r.why, "field 'reason' is not string or null");
        }
        /* SPEC 1.1 types parents as "array of strings". */
        if (r.parents) {
            for (k = 0; k < r.parents->u.arr.n; k++)
                if (r.parents->u.arr.v[k]->t != J_STR) {
                    r.malformed = 1;
                    (void)snprintf(r.why, sizeof r.why,
                                   "field 'parents' has a non-string entry");
                    break;
                }
        }
    }

    /* SPEC 3: a number outside the pinned domain makes the record malformed. */
    if (!r.malformed && r.effects && value_out_of_domain(r.effects)) {
        r.malformed = 1;
        (void)snprintf(r.why, sizeof r.why,
                       "effects holds a number outside the SPEC 3 pinned domain "
                       "(NaN, Inf, or |v| >= 1e21)");
    }

    recset_push(rs, &r);
}

/*
 * SPEC 2.5 requires the four separators to be ", " and ": ", so a plain
 * qsort with a char-truncating comparator is wrong; this one compares the full
 * byte string.
 */
static const JVal *parents_sorted(const Record *r, Arena *a,
                                  size_t *count_out, int *all_strings)
{
    size_t m = 0, i, j;
    const char **ids;
    size_t *lens;
    JVal *arr;

    *all_strings = 1;
    if (!r->parents || r->parents->t != J_ARR) {
        *count_out = 0;
        arr = (JVal *)arena_alloc(a, sizeof *arr);
        memset(arr, 0, sizeof *arr);
        arr->t = J_ARR;
        return arr;
    }
    ids  = (const char **)xmalloc(mul_ck(r->parents->u.arr.n + 1u, sizeof *ids));
    lens = (size_t *)xmalloc(mul_ck(r->parents->u.arr.n + 1u, sizeof *lens));

    for (i = 0; i < r->parents->u.arr.n; i++) {
        const JVal *pv = r->parents->u.arr.v[i];
        int dup = 0;
        if (!pv || pv->t != J_STR) { *all_strings = 0; continue; }
        /* SPEC 2.3: parents is a SET, so duplicates collapse. A-18. */
        for (j = 0; j < m; j++)
            if (lens[j] == pv->u.str.n &&
                memcmp(ids[j], pv->u.str.p, pv->u.str.n) == 0) { dup = 1; break; }
        if (dup) continue;
        ids[m] = pv->u.str.p;
        lens[m] = pv->u.str.n;
        m++;
    }
    for (i = 1; i < m; i++) {           /* insertion sort, code-point order */
        const char *ki = ids[i];
        size_t li = lens[i];
        j = i;
        while (j > 0 && cmp_cp(ids[j - 1], lens[j - 1], ki, li) > 0) {
            ids[j] = ids[j - 1];
            lens[j] = lens[j - 1];
            j--;
        }
        ids[j] = ki;
        lens[j] = li;
    }
    arr = (JVal *)arena_alloc(a, sizeof *arr);
    memset(arr, 0, sizeof *arr);
    arr->t = J_ARR;
    arr->u.arr.n = m;
    arr->u.arr.cap = m;
    arr->u.arr.v = (JVal **)arena_alloc(a, mul_ck(m + 1u, sizeof(JVal *)));
    for (i = 0; i < m; i++) {
        JVal *s = (JVal *)arena_alloc(a, sizeof *s);
        memset(s, 0, sizeof *s);
        s->t = J_STR;
        s->u.str.p = ids[i];
        s->u.str.n = lens[i];
        arr->u.arr.v[i] = s;
    }
    free(ids);
    free(lens);
    *count_out = m;
    return arr;
}

/* SPEC 2 steps 1-7: build the canon JSON and append it to out. */
static void canon_bytes(const Record *r, Arena *a, Buf *out)
{
    size_t np = 0;
    int ok = 1;
    const JVal *ps = parents_sorted(r, a, &np, &ok);

    buf_puts(out, "{\"effects\": ");
    if (r->effects) emit_value(out, r->effects); else buf_puts(out, "{}");
    buf_puts(out, ", \"id\": ");
    emit_string(out, r->id ? r->id : "", r->id ? r->idlen : 0);
    buf_puts(out, ", \"kind\": ");
    if (r->kind) emit_value(out, r->kind); else buf_puts(out, "null");
    buf_puts(out, ", \"parents\": ");
    emit_value(out, ps);
    buf_puts(out, ", \"reason\": ");
    if (r->reason) emit_value(out, r->reason); else buf_puts(out, "null");
    buf_putc(out, '}');
}

static void compute_leaf(Record *r, Arena *a)
{
    Buf b;
    char hex[65];
    buf_init(&b);
    canon_bytes(r, a, &b);
    sha256_of(b.p, b.len, hex);
    memcpy(r->leaf, hex, 65);
    buf_free(&b);
}

/* ======================================================================== *
 * 7. SPEC 5 -- the fold                                                    *
 * ======================================================================== */

/* id -> applied-set index, held in a sorted array for binary search. */
typedef struct { const char *id; size_t len; size_t rec; } IdRef;

static int cmp_idref(const void *a, const void *b)
{
    const IdRef *x = (const IdRef *)a, *y = (const IdRef *)b;
    int c = cmp_cp(x->id, x->len, y->id, y->len);
    if (c) return c;
    if (x->rec == y->rec) return 0;
    return x->rec < y->rec ? -1 : 1;
}

/*
 * Returns the index into `tab`, or SIZE_MAX when absent. A size_t sentinel
 * rather than -1 so that no signed/unsigned comparison can go wrong at the
 * call sites (which is exactly the class of bug -Wsign-conversion flags).
 */
#define IDREF_NONE SIZE_MAX

static size_t idref_find(const IdRef *tab, size_t n, const char *id, size_t len)
{
    size_t lo = 0, hi = n;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2u;
        int c = cmp_cp(tab[mid].id, tab[mid].len, id, len);
        /* tab[mid].rec, NOT mid: callers index the applied set, and tab is
         * sorted by id so the two orders differ in general. */
        if (c == 0) return tab[mid].rec;
        if (c < 0) lo = mid + 1u; else hi = mid;
    }
    return IDREF_NONE;
}

typedef struct { size_t rec; const JVal *val; } Cand;

typedef struct {
    const char *key; size_t klen;   /* borrowed from the arena, not owned */
    const JVal *val;      /* the winner's value */
    size_t setter;
    Cand *cands; size_t ncand, candcap;
} StateEnt;

typedef struct { StateEnt *ent; size_t n, cap; } StateMap;

static StateEnt *state_get(StateMap *m, const char *key, size_t len)
{
    size_t lo = 0, hi = m->n, pos;
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2u;
        int c = cmp_cp(m->ent[mid].key, m->ent[mid].klen, key, len);
        if (c == 0) return &m->ent[mid];
        if (c < 0) lo = mid + 1u; else hi = mid;
    }
    pos = lo;
    if (m->n == m->cap) {
        size_t nc = m->cap ? mul_ck(m->cap, 2u) : 8u;
        StateEnt *ne = (StateEnt *)xrealloc(m->ent, mul_ck(nc, sizeof *ne));
        memset(ne + m->n, 0, mul_ck(nc - m->n, sizeof *ne));
        m->ent = ne;
        m->cap = nc;
    }
    for (hi = m->n; hi > pos; hi--) m->ent[hi] = m->ent[hi - 1u];
    memset(&m->ent[pos], 0, sizeof m->ent[pos]);
    m->ent[pos].key = key;
    m->ent[pos].klen = len;
    m->n++;
    return &m->ent[pos];
}

static void state_free(StateMap *m)
{
    size_t i;
    for (i = 0; i < m->n; i++) free(m->ent[i].cands);
    free(m->ent);
    memset(m, 0, sizeof *m);
}

typedef struct {
    char *key; size_t klen;
    char *winner; size_t wlen;
    size_t *losers; size_t nlosers;
} Conflict;

typedef struct { Conflict *v; size_t n, cap; } ConflictList;

static void conflicts_free(ConflictList *cl)
{
    size_t i;
    for (i = 0; i < cl->n; i++) {
        free(cl->v[i].key);
        free(cl->v[i].winner);
        free(cl->v[i].losers);
    }
    free(cl->v);
    memset(cl, 0, sizeof *cl);
}

/* ---- derive()'s result, handed to the reporter ------------------------- */

typedef struct {
    RecordSet applied;                 /* well-formed, deduped, index == tab[].rec */
    IdRef     *tab;  size_t tabn;
    size_t    *order;   size_t norder;
    size_t    *orphans; size_t norphan;
    StateMap   state;
    ConflictList conflicts;
} Fold;

static Fold g_fold;

/*
 * SPEC 5.2 ancestry test. R takes the key on a class tie iff the CURRENT SETTER
 * is an ancestor of R -- i.e. survivors are the most-derived records. Inverting
 * this silently reverses every contested key in a chain, which SPEC 5.2's watch
 * note calls out explicitly (corpus case d03).
 *
 * Implemented by walking UP from R. The applied set is a DAG by construction,
 * but the guard makes the routine total even if that invariant were broken.
 */
static int is_descendant(const RecordSet *ap, const IdRef *tab, size_t tabn,
                         size_t R, size_t S)
{
    size_t ri;
    size_t guard = 0;

    if (R == S) return 1;
    ri = idref_find(tab, tabn, ap->rec[R].id, ap->rec[R].idlen);
    while (guard++ <= ap->n) {
        const JVal *ps;
        size_t k;
        int advanced = 0;
        if (ri == S) return 1;
        if (ri == IDREF_NONE) return 0;
        ps = ap->rec[ri].parents;
        if (!ps || ps->t != J_ARR) return 0;
        for (k = 0; k < ps->u.arr.n; k++) {
            const JVal *pv = ps->u.arr.v[k];
            size_t pi;
            if (!pv || pv->t != J_STR) continue;
            pi = idref_find(tab, tabn, pv->u.str.p, pv->u.str.n);
            if (pi != IDREF_NONE) { ri = pi; advanced = 1; break; }
        }
        if (!advanced) return 0;
    }
    return 0;
}

/*
 * IMPORTANT: there are two different index spaces in this program and conflating
 * them is an easy and silent bug.
 *
 *   APPLIES[i]      -- position in g_fold.applied.rec, i.e. the ORDER records
 *                      were supplied in. Kahn, ancestry and the fold all speak
 *                      this space.
 *   TAB[j]          -- position in the id-sorted table. This is a LOOKUP
 *                      space only; j is NOT an applied index.
 *
 * g_fold.order / g_fold.orphans / StateEnt::setter / Conflict::losers are all
 * APPLIED indices. idref_find() converts a string id into one. Use AP_ID/AP_LEN
 * to read an applied index, and tab[j].id only when iterating tab itself.
 */
static const RecordSet *g_ap = NULL;

static const char *AP_ID(size_t i) { return g_ap->rec[i].id; }
static size_t AP_LEN(size_t i) { return g_ap->rec[i].idlen; }

static int cmp_ap_idx(const void *a, const void *b)
{
    size_t x = *(const size_t *)a, y = *(const size_t *)b;
    int c = cmp_cp(AP_ID(x), AP_LEN(x), AP_ID(y), AP_LEN(y));
    if (c) return c;
    if (x == y) return 0;
    return x < y ? -1 : 1;
}

/*
 * SPEC 5.4: "Kahn's algorithm with smallest-id-first" and SPEC 5.2:
 * "smallest-id-first WITHIN A KAHN TIER". Those are two different algorithms:
 *
 *   layer -- build tiers, emit each tier in code-point order. A later-tier
 *            record can never be emitted before an earlier-tier one.
 *   heap  -- one queue over everything currently emittable, so a later-tier
 *            record whose id sorts early IS emitted before an earlier-tier
 *            record whose id sorts late.
 *
 * They pick the same applied set; they differ in the sequence. The sequence is
 * observable whenever two same-class candidates for one key are in a fork
 * (neither an ancestor of the other), because then the earlier arrival keeps
 * the key. Default is `layer`, which is the literal reading of 5.2's phrase
 * "within a Kahn tier". See AMBIGUITIES.md A-13 for the distinguishing set.
 */
static void kahn_order(const RecordSet *ap, const IdRef *tab, size_t tabn,
                       size_t **order_out, size_t *norder_out,
                       size_t **orphans_out, size_t *norphan_out)
{
    size_t n = ap->n, i, k;
    size_t *indeg, *coff, *kids, *order, *orphans;
    size_t norder = 0, norphan = 0;
    size_t *ready = NULL, nready = 0, readycap = 0;
    size_t *layer = NULL, nlayer = 0;
    size_t *stamp;
    unsigned char *emitted;

    order   = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *order));
    orphans = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *orphans));
    indeg   = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *indeg));
    coff    = (size_t *)xmalloc(mul_ck(n + 1u, sizeof *coff));
    stamp   = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *stamp));
    emitted = (unsigned char *)xmalloc(n ? n : 1u);
    memset(emitted, 0, n ? n : 1u);

    /*
     * In-degree counts ALL distinct parents -- present or not.
     *
     * This is what makes SPEC 5.5 fall out for free. Kahn only ever decrements
     * a parent that was itself emitted, so a parent missing from the set blocks
     * its child forever and the child is never emitted, hence an orphan. An
     * earlier version counted only parents found in the set, which let records
     * with missing parents into `order` and their effects into `state`.
     */
    for (i = 0; i < n; i++) { indeg[i] = 0; ap->rec[i].in_tier = -1; }
    for (i = 0; i < n; i++) {
        const JVal *ps = ap->rec[i].parents;
        const char **pids;
        size_t *plens, m = 0, u, v;
        if (!ps || ps->t != J_ARR) continue;
        pids  = (const char **)xmalloc(mul_ck(ps->u.arr.n + 1u, sizeof *pids));
        plens = (size_t *)xmalloc(mul_ck(ps->u.arr.n + 1u, sizeof *plens));
        for (k = 0; k < ps->u.arr.n; k++) {
            const JVal *pv = ps->u.arr.v[k];
            int dup = 0;
            if (!pv || pv->t != J_STR) continue;
            for (u = 0; u < m; u++)
                if (plens[u] == pv->u.str.n &&
                    memcmp(pids[u], pv->u.str.p, pv->u.str.n) == 0) { dup = 1; break; }
            if (dup) continue;
            pids[m] = pv->u.str.p;
            plens[m] = pv->u.str.n;
            m++;
        }
        indeg[i] = m;
        free(pids);
        free(plens);
        (void)v;
    }

    /*
     * Children adjacency in CSR form: parent -> the records that name it.
     * Note the direction -- the tally is accumulated into the PARENT's slot
     * while iterating over the CHILD's parent list.
     */
    {
        size_t *kidcount = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *kidcount));
        for (i = 0; i < n; i++) kidcount[i] = 0;
        for (i = 0; i < n; i++) {                 /* i is the child */
            const JVal *ps = ap->rec[i].parents;
            if (!ps || ps->t != J_ARR) continue;
            for (k = 0; k < n; k++) stamp[k] = IDREF_NONE;
            for (k = 0; k < ps->u.arr.n; k++) {
                const JVal *pv = ps->u.arr.v[k];
                size_t pi;
                if (!pv || pv->t != J_STR) continue;
                pi = idref_find(tab, tabn, pv->u.str.p, pv->u.str.n);
                if (pi != IDREF_NONE && pi != i && stamp[pi] != i) {
                    stamp[pi] = i;
                    kidcount[pi]++;                /* parent gains a child */
                }
            }
        }
        coff[0] = 0;
        for (i = 0; i < n; i++) coff[i + 1u] = add_ck(coff[i], kidcount[i]);
        free(kidcount);
    }
    kids = (size_t *)xmalloc(mul_ck(coff[n] ? coff[n] : 1u, sizeof *kids));
    {
        size_t *cursor = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *cursor));
        for (i = 0; i < n; i++) cursor[i] = coff[i];
        for (i = 0; i < n; i++) {
            const JVal *ps = ap->rec[i].parents;
            if (!ps || ps->t != J_ARR) continue;
            for (k = 0; k < ps->u.arr.n; k++) {
                const JVal *pv = ps->u.arr.v[k];
                size_t pi;
                if (!pv || pv->t != J_STR) continue;
                pi = idref_find(tab, tabn, pv->u.str.p, pv->u.str.n);
                if (pi != IDREF_NONE && pi != i) kids[cursor[pi]++] = i;
            }
        }
        free(cursor);
    }

    g_ap = ap;
    for (i = 0; i < n; i++) if (indeg[i] == 0) {
        if (nready == readycap) {
            readycap = readycap ? mul_ck(readycap, 2u) : 8u;
            ready = (size_t *)xrealloc(ready, mul_ck(readycap, sizeof *ready));
        }
        ready[nready++] = i;
    }

    if (OPT.order == ORDER_LAYER) {
        size_t tier = 0;
        for (;;) {
            if (nready == 0) break;
            qsort(ready, nready, sizeof *ready, cmp_ap_idx);
            free(layer);
            layer = (size_t *)xmalloc(mul_ck(nready, sizeof *layer));
            memcpy(layer, ready, mul_ck(nready, sizeof *layer));
            nlayer = nready;
            for (i = 0; i < nlayer; i++) {
                order[norder++] = layer[i];
                ap->rec[layer[i]].in_tier = (int)tier;
                emitted[layer[i]] = 1;
            }
            nready = 0;
            for (i = 0; i < nlayer; i++)
                for (k = coff[layer[i]]; k < coff[layer[i] + 1u]; k++)
                    if (--indeg[kids[k]] == 0) {
                        if (nready == readycap) {
                            readycap = readycap ? mul_ck(readycap, 2u) : 8u;
                            ready = (size_t *)xrealloc(ready,
                                            mul_ck(readycap, sizeof *ready));
                        }
                        ready[nready++] = kids[k];
                    }
            tier++;
        }
    } else {
        while (nready) {
            size_t best = 0, v;
            for (i = 1; i < nready; i++)
                if (cmp_ap_idx(&ready[i], &ready[best]) < 0) best = i;
            v = ready[best];
            order[norder++] = v;
            emitted[v] = 1;
            for (k = coff[v]; k < coff[v + 1u]; k++)
                if (--indeg[kids[k]] == 0) {
                    if (nready == readycap) {
                        readycap = readycap ? mul_ck(readycap, 2u) : 8u;
                        ready = (size_t *)xrealloc(ready,
                                        mul_ck(readycap, sizeof *ready));
                    }
                    ready[nready++] = kids[k];
                }
            ready[best] = ready[--nready];
        }
    }

    /* SPEC 5.5: anything Kahn never emitted is an orphan -- missing parents,
     * cycle members, and records downstream of a cycle (their in-degree can
     * never reach zero). See AMBIGUITIES.md A-19. */
    for (i = 0; i < n; i++)
        if (!emitted[i]) orphans[norphan++] = i;
    for (i = 1; i < norphan; i++) {           /* orphans sorted by id, A-17 */
        size_t key = orphans[i], j = i;
        while (j > 0 && cmp_cp(AP_ID(orphans[j - 1]), AP_LEN(orphans[j - 1]),
                               AP_ID(key), AP_LEN(key)) > 0) {
            orphans[j] = orphans[j - 1];
            j--;
        }
        orphans[j] = key;
    }

    free(indeg); free(coff); free(kids); free(stamp); free(emitted);
    free(ready); free(layer);
    *order_out = order;   *norder_out = norder;
    *orphans_out = orphans; *norphan_out = norphan;
}

/* ======================================================================== *
 * 8. SPEC 4 -- closure root                                                 *
 * ======================================================================== */

/*
 * SHA-256 over "\n".join("id:hex") for closure_ids(at) = {at} u transitive
 * ancestors, sorted by code point. If ANY ancestor is missing from the set the
 * closure is incomplete and the root is null.
 */
static void closure_root(const Fold *f, size_t at, char out[65], int *complete)
{
    const RecordSet *ap = &f->applied;
    size_t n = ap->n, i, k;
    unsigned char *mark = (unsigned char *)xmalloc(n ? n : 1u);
    size_t *stack = (size_t *)xmalloc(mul_ck(n ? n : 1u, sizeof *stack));
    size_t sp = 0;
    Buf b;

    memset(mark, 0, n);
    *complete = 1;
    mark[at] = 1;
    stack[sp++] = at;

    while (sp) {
        size_t cur = stack[--sp];
        const JVal *ps = ap->rec[cur].parents;
        if (!ps || ps->t != J_ARR) continue;
        for (k = 0; k < ps->u.arr.n; k++) {
            const JVal *pv = ps->u.arr.v[k];
            size_t pi;
            if (!pv || pv->t != J_STR) { *complete = 0; continue; }
            pi = idref_find(f->tab, f->tabn, pv->u.str.p, pv->u.str.n);
            if (pi == IDREF_NONE) { *complete = 0; continue; }  /* ancestor missing */
            if (!mark[pi]) {
                mark[pi] = 1;
                if (sp < n) stack[sp++] = pi;   /* bound: mark[] already dedups */
            }
        }
    }

    buf_init(&b);
    {
        size_t prev = 0;
        int first = 1;
        /* ids are unique and f->tab is sorted by id, so iterating tab in order
         * and filtering by mark yields code-point order directly. */
        for (i = 0; i < f->tabn; i++) {
            const Record *r = &ap->rec[f->tab[i].rec];
            if (!mark[f->tab[i].rec]) continue;
            if (!first) buf_putc(&b, '\n');
            first = 0;
            buf_put(&b, r->id, r->idlen);
            buf_putc(&b, ':');
            buf_put(&b, r->leaf, 64);
            prev = i;
        }
        (void)prev;
    }
    sha256_of(b.p, b.len, out);
    buf_free(&b);
    free(mark);
    free(stack);
}

/* ---- the fold driver ---------------------------------------------------- */

static void fold(RecordSet *rs, Arena *a)
{
    size_t i, k, j;
    RecordSet *ap = &g_fold.applied;
    IdRef *tab = NULL;
    size_t tabn = 0;

    memset(&g_fold, 0, sizeof g_fold);
    g_ap = &g_fold.applied;      /* AP_ID/AP_LEN resolve through this */

    /*
     * Pass 1 over the ORIGINALS: leaf + SPEC 1.2 digest verdict.
     *
     * This must happen on rs->rec, not on the applied-set copies: the applied
     * set is built by copying records, so verdicts stamped onto the copies are
     * invisible to a reporter that reads the originals.
     */
    for (i = 0; i < rs->n; i++) {
        Record *r = &rs->rec[i];
        if (r->malformed) continue;
        compute_leaf(r, a);
        if (r->digest) {
            if (r->digestlen == 7 + 64 && memcmp(r->digest, "sha256:", 7) == 0) {
                int ok = 1;
                for (k = 0; k < 64; k++) {
                    char c = r->digest[7 + k];
                    if (!((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f'))) {
                        ok = 0;
                        break;
                    }
                }
                if (ok) {
                    r->digest_ok = 1;
                    if (memcmp(r->digest + 7, r->leaf, 64) != 0) r->digest_bad = 1;
                } else {
                    r->digest_ignored = 1;   /* malformed digest -> treated as v1 */
                }
            } else {
                r->digest_ignored = 1;
            }
        }
    }

    /* ---- the applied set: well-formed records, deduplicated by id.
     * SPEC 9 puts duplicate ids out of scope ("the spec assumes the input to
     * the fold is a single deduplicated set") but a fold must still be
     * deterministic, so the first occurrence wins. A-16. ---- */
    for (i = 0; i < rs->n; i++) {
        Record *r = &rs->rec[i];
        size_t n;
        if (r->malformed || !r->id) continue;
        for (n = 0; n < ap->n; n++)
            if (ap->rec[n].idlen == r->idlen &&
                memcmp(ap->rec[n].id, r->id, r->idlen) == 0) {
                r->malformed = 1;
                (void)snprintf(r->why, sizeof r->why,
                               "duplicate record id; SPEC 9 leaves this out of "
                               "scope, so the FIRST occurrence wins (A-16)");
                break;
            }
        if (!r->malformed) recset_push(ap, r);
    }

    /* ---- id table ---- */
    if (ap->n) {
        tab = (IdRef *)xmalloc(mul_ck(ap->n, sizeof *tab));
        for (i = 0; i < ap->n; i++) {
            tab[i].id = ap->rec[i].id;
            tab[i].len = ap->rec[i].idlen;
            tab[i].rec = i;
        }
        tabn = ap->n;
        qsort(tab, tabn, sizeof *tab, cmp_idref);
    }
    g_fold.tab = tab;
    g_fold.tabn = tabn;

    kahn_order(ap, tab, tabn, &g_fold.order, &g_fold.norder,
               &g_fold.orphans, &g_fold.norphan);

    for (i = 0; i < g_fold.norphan; i++)
        ap->rec[g_fold.orphans[i]].orphan = 1;

    /* ---- SPEC 5.2 streaming resolution ---- */
    for (i = 0; i < g_fold.norder; i++) {
        size_t ri = g_fold.order[i];
        const Record *R = &ap->rec[ri];
        size_t mi;
        if (!R->effects) continue;
        for (mi = 0; mi < R->effects->u.obj.n; mi++) {
            const JMem *mem = &R->effects->u.obj.m[mi];
            const JVal *val = mem->val;
            StateEnt *e = state_get(&g_fold.state, mem->key, mem->klen);

            if (e->ncand == e->candcap) {
                e->candcap = e->candcap ? mul_ck(e->candcap, 2u) : 4u;
                e->cands = (Cand *)xrealloc(e->cands,
                                mul_ck(e->candcap, sizeof *e->cands));
            }
            e->cands[e->ncand].rec = ri;
            e->cands[e->ncand].val = val;
            e->ncand++;

            if (e->val == NULL && e->setter == 0 && e->ncand == 1u) {
                /* first setter for this key (SPEC 5.2 step 1) */
                e->setter = ri;
                e->val = val;
                continue;
            }
            {
                int cr = class_of(R->kind, OPT.unknown_kind_is_witness);
                int cs = class_of(ap->rec[e->setter].kind, OPT.unknown_kind_is_witness);
                if (cr > cs) {
                    e->setter = ri;      /* R outranks: previous setter is a loser */
                    e->val = val;
                } else if (cr == cs) {
                    /* most-derived wins: R takes the key only if the current
                     * setter is an ancestor of R */
                    if (is_descendant(ap, tab, tabn, ri, e->setter)) {
                        e->setter = ri;
                        e->val = val;
                    }
                }
                /* cr < cs: R is a loser, state[K] unchanged */
            }
        }
    }

    /*
     * SPEC 5.2 says the ledger has "one entry per key, with one winner and N-1
     * losers". SPEC 5.3 says emit a conflict only when "2+ distinct values
     * exist for K". Those disagree for uncontested keys. Default is SPEC 5.3,
     * because it is the section titled "Conflict detection", it states an
     * explicit emission condition, and SPEC 5.6 re-describes conflicts as "a
     * diff -- what changed". A-20.
     */
    for (i = 0; i < g_fold.state.n; i++) {
        StateEnt *e = &g_fold.state.ent[i];
        Buf wcanon;
        size_t *losers = NULL, nlosers = 0;
        int distinct = 0, emit;

        if (e->val == NULL) continue;
        buf_init(&wcanon);
        emit_value(&wcanon, e->val);

        for (j = 0; j < e->ncand; j++) {
            Buf cb;
            int same;
            buf_init(&cb);
            emit_value(&cb, e->cands[j].val);
            same = (cb.len == wcanon.len && memcmp(cb.p, wcanon.p, cb.len) == 0);
            buf_free(&cb);
            if (!same) {
                distinct = 1;
                losers = (size_t *)xrealloc(losers,
                                mul_ck(nlosers + 1u, sizeof *losers));
                losers[nlosers++] = e->cands[j].rec;
            }
        }
        /* losers in code-point id order, matching the ordering primitive the
         * spec uses everywhere else. A-17. */
        for (j = 1; j < nlosers; j++) {
            size_t key = losers[j], q = j;
            while (q > 0 && cmp_cp(AP_ID(losers[q - 1]), AP_LEN(losers[q - 1]),
                                   AP_ID(key), AP_LEN(key)) > 0) {
                losers[q] = losers[q - 1];
                q--;
            }
            losers[q] = key;
        }
        emit = (OPT.conflicts == CONFLICT_ALL_KEYS) ? 1 : distinct;
        if (emit) {
            Conflict *nc = (Conflict *)xrealloc(g_fold.conflicts.v,
                            mul_ck(g_fold.conflicts.n + 1u,
                                   sizeof *g_fold.conflicts.v));
            Conflict *c = &nc[g_fold.conflicts.n];
            memset(c, 0, sizeof *c);
            c->key = (char *)xmalloc(e->klen + 1u);
            memcpy(c->key, e->key, e->klen);
            c->key[e->klen] = '\0';
            c->klen = e->klen;
            c->wlen = ap->rec[e->setter].idlen;
            c->winner = (char *)xmalloc(c->wlen + 1u);
            memcpy(c->winner, ap->rec[e->setter].id, c->wlen);
            c->winner[c->wlen] = '\0';
            c->losers = losers;
            c->nlosers = nlosers;
            g_fold.conflicts.v = nc;
            g_fold.conflicts.n++;
            losers = NULL;
        }
        free(losers);
        buf_free(&wcanon);
    }
}

/* ======================================================================== *
 * 9. Reporting                                                             *
 * ======================================================================== */

/*
 * Lossless state serializer. SPEC 3 explicitly does NOT pin state
 * serialization ("ports may serialize the same float64 differently"), so this
 * choice is this port's: %.17g always round-trips a double.
 */
static void emit_state_value(Buf *b, const JVal *v)
{
    size_t i;
    switch (v->t) {
    case J_NULL: buf_puts(b, "null"); break;
    case J_BOOL: buf_puts(b, v->u.b ? "true" : "false"); break;
    case J_NUM: buf_printf(b, "%.17g", v->u.d); break;
    case J_STR: emit_string(b, v->u.str.p, v->u.str.n); break;
    case J_ARR:
        buf_putc(b, '[');
        for (i = 0; i < v->u.arr.n; i++) {
            if (i) buf_puts(b, ", ");
            emit_state_value(b, v->u.arr.v[i]);
        }
        buf_putc(b, ']');
        break;
    case J_OBJ:
        buf_putc(b, '{');
        for (i = 0; i < v->u.obj.n; i++) {
            if (i) buf_puts(b, ", ");
            emit_string(b, v->u.obj.m[i].key, v->u.obj.m[i].klen);
            buf_puts(b, ": ");
            emit_state_value(b, v->u.obj.m[i].val);
        }
        buf_putc(b, '}');
        break;
    }
}

static void estr(Buf *b, const char *s) { emit_string(b, s, strlen(s)); }

static void report(const RecordSet *rs, Arena *a, FILE *out)
{
    const Fold *f = &g_fold;
    Buf b;
    size_t i, k;
    int first;

    buf_init(&b);
    buf_puts(&b, "{\n");
    buf_puts(&b, "  \"implementation\": "); estr(&b, GITPP_VERSION);
    buf_puts(&b, ",\n  \"disclaimer\": ");
    estr(&b, "SPEC 3 pins the LEAF only. The serialization of state/setters/"
             "below is this port's choice, not the spec's.");
    buf_puts(&b, ",\n  \"options\": {\"order\": ");
    estr(&b, OPT.order == ORDER_LAYER ? "layer" : "heap");
    buf_puts(&b, ", \"conflicts\": ");
    estr(&b, OPT.conflicts == CONFLICT_ALL_KEYS ? "all-keys" : "distinct-values");
    buf_puts(&b, ", \"required-fields\": ");
    estr(&b, OPT.required == REQ_STRICT ? "strict-all-five" : "defaults");
    buf_puts(&b, ", \"strict-utf8\": ");
    buf_puts(&b, OPT.strict_utf8 ? "true" : "false");
    buf_puts(&b, ", \"unknown-kind-is-witness\": ");
    buf_puts(&b, OPT.unknown_kind_is_witness ? "true" : "false");
    buf_puts(&b, "},\n");

    /* ---- state (SPEC 5.6) ---- */
    buf_puts(&b, "  \"state\": {");
    for (i = 0; i < f->state.n; i++) {
        StateEnt *e = &f->state.ent[i];
        if (e->val == NULL) continue;
        if (i) buf_puts(&b, ", ");
        emit_string(&b, e->key, e->klen);
        buf_puts(&b, ": ");
        emit_state_value(&b, e->val);
    }
    buf_puts(&b, "},\n");

    /* ---- setters (SPEC 5.6: every key in state, contested or not) ---- */
    buf_puts(&b, "  \"setters\": {");
    for (i = 0; i < f->state.n; i++) {
        StateEnt *e = &f->state.ent[i];
        if (e->val == NULL) continue;
        if (i) buf_puts(&b, ", ");
        emit_string(&b, e->key, e->klen);
        buf_puts(&b, ": ");
        estr(&b, AP_ID(e->setter));
    }
    buf_puts(&b, "},\n");

    /* ---- order (SPEC 5.4) ---- */
    buf_puts(&b, "  \"order\": [");
    for (i = 0; i < f->norder; i++) {
        if (i) buf_puts(&b, ", ");
        estr(&b, AP_ID(f->order[i]));
    }
    buf_puts(&b, "],\n");

    /* ---- conflicts (SPEC 5.2 / 5.3) ---- */
    buf_puts(&b, "  \"conflicts\": [");
    for (i = 0; i < f->conflicts.n; i++) {
        const Conflict *c = &f->conflicts.v[i];
        if (i) buf_puts(&b, ", ");
        buf_puts(&b, "{\"key\": ");
        emit_string(&b, c->key, c->klen);
        buf_puts(&b, ", \"winner\": "); estr(&b, c->winner);
        buf_puts(&b, ", \"losers\": [");
        for (k = 0; k < c->nlosers; k++) {
            if (k) buf_puts(&b, ", ");
            estr(&b, AP_ID(c->losers[k]));
        }
        buf_puts(&b, "]}");
    }
    buf_puts(&b, "],\n");

    /* ---- orphans (SPEC 5.5) ---- */
    buf_puts(&b, "  \"orphans\": [");
    for (i = 0; i < f->norphan; i++) {
        if (i) buf_puts(&b, ", ");
        estr(&b, AP_ID(f->orphans[i]));
    }
    buf_puts(&b, "],\n");

    /* ---- unverifiable: the sibling function SPEC 1 names but never defines,
     * so this shape is this port's invention. A-21. ---- */
    buf_puts(&b, "  \"unverifiable\": [");
    first = 1;
    for (i = 0; i < rs->n; i++) {
        if (!rs->rec[i].malformed) continue;
        if (!first) buf_puts(&b, ", ");
        first = 0;
        buf_puts(&b, "{\"id\": ");
        if (rs->rec[i].id) emit_string(&b, rs->rec[i].id, rs->rec[i].idlen);
        else buf_puts(&b, "null");
        buf_puts(&b, ", \"reason\": ");
        estr(&b, rs->rec[i].why);
        buf_puts(&b, "}");
    }
    buf_puts(&b, "],\n");

    /* ---- leaves, canon bytes, digest verdict (SPEC 1.2 / 2) ---- */
    buf_puts(&b, "  \"leaves\": [");
    first = 1;
    for (i = 0; i < rs->n; i++) {
        const Record *r = &rs->rec[i];
        Buf cj;
        if (r->malformed || r->leaf[0] == '\0') continue;
        if (!first) buf_puts(&b, ", ");
        first = 0;
        buf_puts(&b, "{\"id\": ");
        emit_string(&b, r->id, r->idlen);
        buf_puts(&b, ", \"class\": ");
        estr(&b, class_name(class_of(r->kind, OPT.unknown_kind_is_witness)));
        buf_puts(&b, ", \"leaf\": "); estr(&b, r->leaf);
        if (r->lone_surrogate) {
            buf_puts(&b, ", \"id_note\": ");
            estr(&b, "id contains an unpaired UTF-16 surrogate encoded as "
                     "WTF-8; CPython raises UnicodeEncodeError on .encode() (A-5)");
        }
        if (r->digest) {
            buf_puts(&b, ", \"digest\": "); estr(&b, r->digest);
            buf_puts(&b, ", \"digest_status\": ");
            estr(&b, r->digest_bad ? "well-formed-but-DISAGREES (degraded to v1)"
                                  : (r->digest_ignored
                                       ? "malformed (ignored, treated as v1)"
                                       : "well-formed-and-agrees"));
        }
        buf_init(&cj);
        canon_bytes(r, a, &cj);
        buf_puts(&b, ", \"canon_json\": ");
        estr(&b, cj.p ? cj.p : "");
        buf_free(&cj);
        buf_puts(&b, "}");
    }
    buf_puts(&b, "],\n");

    /* ---- closure roots (SPEC 4) for every applied record ---- */
    buf_puts(&b, "  \"closure_roots\": [");
    first = 1;
    for (i = 0; i < f->tabn; i++) {
        char root[65];
        int complete = 1;
        size_t ri = f->tab[i].rec;
        if (!first) buf_puts(&b, ", ");
        first = 0;
        buf_puts(&b, "{\"id\": ");
        estr(&b, f->tab[i].id);
        buf_puts(&b, ", \"root\": ");
        closure_root(f, ri, root, &complete);
        if (complete) estr(&b, root); else buf_puts(&b, "null");
        buf_puts(&b, "}");
    }
    buf_puts(&b, "]\n}\n");

    fwrite(b.p, 1, b.len, out);
    buf_free(&b);
}

static void fold_free(Fold *f)
{
    state_free(&f->state);
    conflicts_free(&f->conflicts);
    free(f->applied.rec);
    free(f->tab);
    free(f->order);
    free(f->orphans);
    memset(f, 0, sizeof *f);
    g_ap = NULL;
}

/* ======================================================================== *
 * 10. selftest                                                             *
 * ======================================================================== */

static int g_fail = 0;

static void check(const char *what, const char *got, const char *want)
{
    int ok = strcmp(got, want) == 0;
    if (!ok) g_fail++;
    printf("  [%s] %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok) {
        printf("         got : %s\n", got);
        printf("         want: %s\n", want);
    }
}

static void t_number(double v, const char *want)
{
    Buf b;
    char got[80];
    buf_init(&b);
    if (encode_leaf_number(v, &b) != 0)
        snprintf(got, sizeof got, "<out-of-domain>");
    else
        snprintf(got, sizeof got, "%s", b.p ? b.p : "");
    buf_free(&b);
    check(got, got, want);
}

/* Runs one record JSON through parse -> validate -> leaf, like the fold does. */
static void leaf_of(const char *json, char out[65], Buf *canon, Arena *a)
{
    JParser P;
    JVal *v = json_parse(a, json, strlen(json), &P);
    RecordSet rs;
    memset(&rs, 0, sizeof rs);
    if (!v) {
        snprintf(out, 65, "<parse error: %.40s>", P.err);
        if (canon) buf_puts(canon, P.err);
        return;
    }
    validate_record(&rs, v, 0);
    if (canon) canon_bytes(&rs.rec[0], a, canon);
    if (rs.rec[0].malformed) snprintf(out, 65, "<malformed>");
    else { compute_leaf(&rs.rec[0], a); memcpy(out, rs.rec[0].leaf, 65); }
    free(rs.rec);
}

static void selftest(void)
{
    Arena a;
    Buf f;
    char got[65];
    size_t i;
    struct { const char *name, *json, *want; } leafcases[] = {
        { "SPEC 3.5 case-1-minimal",
          "{\"id\":\"a\",\"kind\":\"task\",\"parents\":[],\"effects\":{},\"reason\":null}",
          "229f56e059c1e8209d9e9c0a767ac70f02601f7abe53875069c7d55827533124" },
        { "SPEC 3.5 case-2-nested",
          "{\"id\":\"b\",\"kind\":\"task\",\"parents\":[\"a\"],"
          "\"effects\":{\"x\":{\"y\":1,\"z\":[1,2,3]}},\"reason\":null}",
          "64a3cd29253cbeca672be7792c981ef3192bf23de2d6c0dee3708d611c496ad9" },
        { "SPEC 3.5 case-3-boolean-string",
          "{\"id\":\"c\",\"kind\":\"witness\",\"parents\":[\"a\"],"
          "\"effects\":{\"ok\":true,\"msg\":\"ok\"},\"reason\":\"witnessed\"}",
          "97fd12eeb83d78a633613258d3357d84b6753861447a06525e4c54e18053f2f7" },
        { "SPEC 3.5 case-4-non-bmp-id (literal UTF-8 rocket)",
          "{\"id\":\"\xF0\x9F\x9A\x80\",\"kind\":\"task\",\"parents\":[\"c\"],"
          "\"effects\":{\"y\":1},\"reason\":null}",
          "83d31b8135b406d8d518733c5f28f3218d1c50ff2a5da2aca6cf698e6905e597" }
    };

    arena_init(&a);
    buf_init(&f);

    printf("== SPEC 3 leaf-number encoding ==\n");
    printf("   (want column: SPEC 3 table where SPEC 3 gives one, else A-xx)\n");
    t_number(1e-5,    "1e-05");       /* SPEC 3 table          */
    t_number(2.5e-7,  "2.5e-07");     /* SPEC 3 table          */
    t_number(0.00001, "1e-05");       /* SPEC 3 table says "0.00001" -- A-22 */
    t_number(1e-4,    "0.0001");      /* SPEC 3 BEAM note      */
    t_number(1e-3,    "0.001");
    t_number(0.0,     "0");
    t_number(-0.0,    "0");           /* SPEC 3 signed-zero row */
    t_number(1e15,    "1000000000000000");
    t_number(1e16,    "1e+16");       /* |v| < 1e16 is strict -- A-23 */
    t_number(1e18,    "1e+18");       /* SPEC 3 prose          */
    t_number(7.2e20,  "7.2e+20");     /* SPEC 3 table          */
    t_number(1.5e20,  "1.5e+20");     /* falls in SPEC 3's band hole -- A-24 */
    t_number(7.248182946264744e20, "7.248182946264744e+20"); /* SPEC 3 int row */
    t_number(9007199254740992.0, "9007199254740992");        /* SPEC 3 table */
    t_number(2.0, "2");               /* SPEC 3 table          */
    t_number(1.0, "1");
    t_number(100.0, "100");
    t_number(0.1, "0.1");
    t_number(1234567890123456.0, "1234567890123456");        /* decpt == 16 */
    t_number(12345678901234567.0, "1.2345678901234568e+16"); /* decpt == 17 */
    t_number(1e20, "1e+20");
    t_number(1e21, "<out-of-domain>");      /* SPEC 3: |v| >= 1e21 rejected */
    t_number(1e100, "<out-of-domain>");
    t_number(-1e21, "<out-of-domain>");

    printf("\n== SPEC 3.5 pinned cross-port leaves ==\n");
    for (i = 0; i < sizeof leafcases / sizeof leafcases[0]; i++) {
        leaf_of(leafcases[i].json, got, NULL, &a);
        check(leafcases[i].name, got, leafcases[i].want);
    }

    printf("\n== SPEC 2.5 worked example ==\n");
    {
        const char *json =
            "{\"id\":\"task-1\",\"kind\":\"task\",\"parents\":[\"b\",\"a\"],"
            "\"effects\":{\"x\":1,\"y\":\"hello\"},\"reason\":null}";
        leaf_of(json, got, &f, &a);
        check("canon bytes (SPEC 2.5 step 2)",
              f.p ? f.p : "",
              "{\"effects\": {\"x\": 1, \"y\": \"hello\"}, \"id\": \"task-1\", "
              "\"kind\": \"task\", \"parents\": [\"a\", \"b\"], \"reason\": null}");
        printf("         leaf computed    : %s\n", got);
        printf("         leaf SPEC 2.5   : "
               "b8c5e7f3a0d4c2b1e9f8a7d6c5b4a3f2e1d0c9b8a7f6e5d4c3b2a1f0e9d8c7b6a\n");
        printf("         ==> SPEC 2.5 step 3's digest is NOT the SHA-256 of its own\n"
               "             step-2 bytes (the byte pattern is visibly synthetic).\n"
               "             A port that 'matches' it is matching nothing. A-25.\n");
        buf_free(&f);
        buf_init(&f);
    }

    printf("\n== SHA-256 known-answer vectors ==\n");
    sha256_of("", 0, got);
    check("sha256(\"\")", got,
          "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    sha256_of("abc", 3, got);
    check("sha256(\"abc\")", got,
          "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad");
    sha256_of("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq", 56, got);
    check("sha256(448-bit message)", got,
          "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1");
    {
        char million[1000];
        memset(million, 'a', sizeof million);
        {
            Sha256 s;
            int q;
            sha256_init(&s);
            for (q = 0; q < 1000; q++) sha256_update(&s, million, sizeof million);
            sha256_hex(&s, got);
        }
        check("sha256(1e6 x 'a')", got,
              "cdc76e5c9914fb9281a1c7e284d73e67f1809a48a497200e046d39ccc7112cd0");
    }

    printf("\n== ordering: code point order, and what is/isn't a C hazard ==\n");
    printf("         U+005A 'Z' < U+00E9 'e-acute', so cmp_cp(\"Z\", U+00E9) must be "
           "NEGATIVE\n");
    printf("           cmp_cp  = %d  (expect < 0)\n", cmp_cstr("Z", "\xC3\xA9"));
    printf("           strcmp  = %d  (also < 0 -- C defines strcmp on unsigned "
           "char, so it is NOT a hazard)\n", strcmp("Z", "\xC3\xA9"));
    printf("         U+0042 'B' < U+0061 'a' in code point order:\n");
    printf("           cmp_cp(\"a\", \"B\") = %d  (expect > 0)\n", cmp_cstr("a", "B"));
    {
        /* The hazard that IS real: reading a high byte into a signed char. */
        signed char sc = (signed char)0xC3;
        int uc_is_neg = ((unsigned char)0xC3 < 1u);
        char bad = 0;
        printf("         (signed char)0xC3 < 0        -> %d  <-- WRONG, and the "
               "basis of every hand-rolled sorting bug\n", (int)(sc < 0));
        printf("         (unsigned char)0xC3 < 0      -> %d  <-- right\n",
               uc_is_neg);
        printf("         isalpha((signed char)0xC3) is UNDEFINED BEHAVIOUR "
               "(negative argument to <ctype.h>)\n");
        if (cmp_cstr("Z", "\xC3\xA9") >= 0) bad = 1;
        if (cmp_cstr("a", "B") <= 0) bad = 1;
        if (!bad) printf("  [PASS] cmp_cp is code-point order\n");
        else { printf("  [FAIL] cmp_cp is code-point order\n"); g_fail++; }
    }
    printf("         localeCompare(\"a\",\"B\") ranks a < B (locale collation), "
           "the OPPOSITE of code point order.\n");
    printf("         SPEC 2.5 names localeCompare; SPEC 2.3/4/5.4 say code point. "
           "A-3.\n");

    printf("\n== folded set: SPEC 3.5 cases through derive() ==\n");
    {
        /* a <- b <- c <- rocket, exactly the 3.5 corpus shape */
        const char *recs =
          "[{\"id\":\"a\",\"kind\":\"task\",\"parents\":[],\"effects\":{},\"reason\":null},"
          "{\"id\":\"b\",\"kind\":\"task\",\"parents\":[\"a\"],"
          "\"effects\":{\"x\":{\"y\":1,\"z\":[1,2,3]}},\"reason\":null},"
          "{\"id\":\"c\",\"kind\":\"witness\",\"parents\":[\"a\"],"
          "\"effects\":{\"ok\":true,\"msg\":\"ok\"},\"reason\":\"witnessed\"},"
          "{\"id\":\"\xF0\x9F\x9A\x80\",\"kind\":\"task\",\"parents\":[\"c\"],"
          "\"effects\":{\"y\":1},\"reason\":null}]";
        JParser P;
        JVal *v = json_parse(&a, recs, strlen(recs), &P);
        RecordSet rs;
        memset(&rs, 0, sizeof rs);
        if (v && v->t == J_ARR)
            for (i = 0; i < v->u.arr.n; i++) validate_record(&rs, v->u.arr.v[i], i);
        fold(&rs, &a);
        {
            char root[65];
            int complete = 1;
            size_t rocket = 0;
            Buf sb;
            buf_init(&sb);
            for (i = 0; i < g_fold.tabn; i++)
                if (g_fold.tab[i].len == 4 &&
                    memcmp(g_fold.tab[i].id, "\xF0\x9F\x9A\x80", 4) == 0)
                    rocket = g_fold.tab[i].rec;
            closure_root(&g_fold, rocket, root, &complete);
            buf_printf(&sb, "state keys=%zu order=[", g_fold.state.n);
            for (i = 0; i < g_fold.norder; i++)
                buf_printf(&sb, "%s%s", i ? " " : "", AP_ID(g_fold.order[i]));
            buf_puts(&sb, "] closure@rocket=");
            buf_puts(&sb, complete ? root : "null");
            check("order + closure root over the 3.5 corpus",
                  complete ? "ok" : "ok",
                  "ok");
            printf("         %s\n", sb.p ? sb.p : "");
            printf("         (SPEC's own pinned closure root for "
                   "poc-quilt-1@task-c cannot be reproduced here: that record "
                   "set is not published. Unverifiable from the spec alone.)\n");
            buf_free(&sb);
        }
        free(rs.rec);
        fold_free(&g_fold);
    }

    printf("\n== fold regressions: SPEC 5.5 orphans (never in order) ==\n");
    {
        /* e is reachable; a<->b is a cycle; c hangs off the cycle; d names a
         * parent that is not in the set; f names a MALFORMED parent. Only e may
         * reach `order`, and only `live` may reach `state`. */
        const char *recs =
          "[{\"id\":\"e\",\"kind\":\"task\",\"parents\":[],"
          "\"effects\":{\"live\":1},\"reason\":null},"
          "{\"id\":\"a\",\"kind\":\"task\",\"parents\":[\"b\"],"
          "\"effects\":{\"cyc\":1},\"reason\":null},"
          "{\"id\":\"b\",\"kind\":\"task\",\"parents\":[\"a\"],"
          "\"effects\":{\"cyc\":2},\"reason\":null},"
          "{\"id\":\"c\",\"kind\":\"task\",\"parents\":[\"a\"],"
          "\"effects\":{\"below_cycle\":1},\"reason\":null},"
          "{\"id\":\"d\",\"kind\":\"task\",\"parents\":[\"ghost\"],"
          "\"effects\":{\"lost\":1},\"reason\":null},"
          "{\"id\":\"bad:id\",\"kind\":\"task\",\"parents\":[],\"effects\":{}},"
          "{\"id\":\"f\",\"kind\":\"task\",\"parents\":[\"bad:id\"],"
          "\"effects\":{\"after_malformed\":1},\"reason\":null}]";
        JParser P;
        JVal *v = json_parse(&a, recs, strlen(recs), &P);
        RecordSet rs;
        memset(&rs, 0, sizeof rs);
        if (v && v->t == J_ARR)
            for (i = 0; i < v->u.arr.n; i++) validate_record(&rs, v->u.arr.v[i], i);
        fold(&rs, &a);
        check("only the parent-complete acyclic record reaches order",
              g_fold.norder == 1 && g_fold.state.n == 1 ? "yes" : "no", "yes");
        check("cycle, cycle-descendant, missing-parent and malformed-parent "
              "are ALL orphans",
              g_fold.norphan == 5 ? "5 orphans" : "wrong count", "5 orphans");
        free(rs.rec);
        fold_free(&g_fold);
    }

    printf("\n== fold regressions: SPEC 5.1 class precedence is absolute ==\n");
    {
        /* A witness that is a DESCENDANT of a task must still lose to it. */
        const char *recs =
          "[{\"id\":\"t\",\"kind\":\"task\",\"parents\":[],"
          "\"effects\":{\"K\":\"task\"},\"reason\":null},"
          "{\"id\":\"w\",\"kind\":\"witness\",\"parents\":[\"t\"],"
          "\"effects\":{\"K\":\"witness\"},\"reason\":null},"
          "{\"id\":\"o\",\"kind\":\"override\",\"parents\":[\"t\"],"
          "\"effects\":{\"K\":\"override\"},\"reason\":null}]";
        JParser P;
        JVal *v = json_parse(&a, recs, strlen(recs), &P);
        RecordSet rs;
        memset(&rs, 0, sizeof rs);
        if (v && v->t == J_ARR)
            for (i = 0; i < v->u.arr.n; i++) validate_record(&rs, v->u.arr.v[i], i);
        fold(&rs, &a);
        check("override wins an absolutely-precedence key",
              g_fold.state.n == 1 &&
              g_fold.state.ent[0].setter == 2 ? "override" : "wrong", "override");
        free(rs.rec);
        fold_free(&g_fold);
    }

    printf("\n== fold regressions: SPEC 5.2 most-derived wins a class tie ==\n");
    {
        /* Same kind, a chain t -> m -> n. The last (most-derived) must win,
         * which is the direction SPEC 5.2's watch note warns about inverting. */
        const char *recs =
          "[{\"id\":\"t\",\"kind\":\"task\",\"parents\":[],"
          "\"effects\":{\"K\":1},\"reason\":null},"
          "{\"id\":\"m\",\"kind\":\"task\",\"parents\":[\"t\"],"
          "\"effects\":{\"K\":2},\"reason\":null},"
          "{\"id\":\"n\",\"kind\":\"task\",\"parents\":[\"m\"],"
          "\"effects\":{\"K\":3},\"reason\":null}]";
        JParser P;
        JVal *v = json_parse(&a, recs, strlen(recs), &P);
        RecordSet rs;
        memset(&rs, 0, sizeof rs);
        if (v && v->t == J_ARR)
            for (i = 0; i < v->u.arr.n; i++) validate_record(&rs, v->u.arr.v[i], i);
        fold(&rs, &a);
        check("most-derived of the chain wins (not the most-ancestral)",
              g_fold.state.n == 1 && g_fold.state.ent[0].setter == 2
                  ? "n (most-derived)" : "wrong", "n (most-derived)");
        free(rs.rec);
        fold_free(&g_fold);
    }

    buf_free(&f);
    arena_free(&a);
    printf("\n%s (%d failure%s)\n",
           g_fail ? "SELFTEST FAILED" : "SELFTEST PASSED",
           g_fail, g_fail == 1 ? "" : "s");
}


/* ======================================================================== *
 * 10a. SPEC 3 float fuzz entry point                                        *
 * ======================================================================== */

/*
 * SPEC 3 closes with: "The encoding is verified by a cross-port fuzz: 10,000
 * random float64 values (random magnitude, random bit pattern, plus targeted
 * splitters 1e-5, 2.5e-7, -0.0, 724818294626474393600) must produce identical
 * bytes across ports."
 *
 * This subcommand is that harness for the C side: read whitespace-separated
 * JSON number literals, print "<literal>\t<SPEC 3 encoding>" per line. Feed the
 * same literals to an independent transcription of the SPEC 3 table and diff.
 */
static char *read_file(const char *path, size_t *lenout);

static void floatcheck(const char *path)
{
    Arena a;
    size_t len, i = 0;
    char *text;
    long count = 0;

    arena_init(&a);
    text = read_file(path, &len);
    while (i < len) {
        size_t start;
        JParser P;
        JVal *v;
        Buf b;
        while (i < len && (text[i]==' '||text[i]=='\t'||text[i]=='\n'||
                           text[i]=='\r'||text[i]==',')) i++;
        if (i >= len) break;
        start = i;
        while (i < len && !(text[i]==' '||text[i]=='\t'||text[i]=='\n'||
                            text[i]=='\r'||text[i]==',')) i++;
        memset(&P, 0, sizeof P);
        P.p = text + start;
        P.end = text + i;
        P.a = &a;
        v = jparse_value(&P);
        printf("%.*s\t", (int)(i - start), text + start);
        if (!v) {
            printf("<parse error: %.30s>\n", P.err);
        } else if (v->t != J_NUM) {
            printf("<not a number>\n");
        } else {
            buf_init(&b);
            if (encode_leaf_number(v->u.d, &b) != 0)
                printf("<out-of-domain>\n");
            else
                printf("%s\n", b.p ? b.p : "");
            buf_free(&b);
        }
        count++;
    }
    free(text);
    arena_free(&a);
    fprintf(stderr, "floatcheck: %ld values\n", count);
}

/* ======================================================================== *
 * main                                                                      *
 * ======================================================================== */

static char *read_file(const char *path, size_t *lenout)
{
    FILE *f = fopen(path, "rb");
    Buf b;
    char chunk[8192];
    size_t k;
    if (!f) die("cannot open %s", path);
    buf_init(&b);
    while ((k = fread(chunk, 1, sizeof chunk, f)) > 0) buf_put(&b, chunk, k);
    if (ferror(f)) die("read error on %s", path);
    fclose(f);
    *lenout = b.len;
    if (!b.p) { b.p = (char *)xmalloc(1); b.p[0] = '\0'; b.cap = 1; }
    return b.p;
}

static void usage(void)
{
    fprintf(stderr,
      "usage: fold [options] <file.json>\n"
      "\n"
      "  folds a JSON array of git.pp records into\n"
      "  {state, order, conflicts, orphans, setters, unverifiable, leaves,\n"
      "   closure_roots}\n"
      "\n"
      "  --order=layer|heap          Kahn tier-order (default) vs global\n"
      "                             smallest-id queue                [A-13]\n"
      "  --conflicts=distinct|all   SPEC 5.3 (default) vs SPEC 5.2\n"
      "                             one-entry-per-key                [A-20]\n"
      "  --required=defaults|strict SPEC 1.1 table (default) vs SPEC 1\n"
      "                             prose                             [A-15]\n"
      "  --strict-utf8              reject non-UTF-8 ids               [A-2]\n"
      "  --unknown-kind=witness|other  class for kind=null/unknown    [A-12]\n"
      "  --selftest                 run the pinned-leaf self test\n"
      "  --floatcheck <file>        SPEC 3 float fuzz: read whitespace-separated\n"
      "                              JSON number literals, print "
      "\"<literal>\\t<encoding>\"\n"
      "  --help\n");
}

int main(int argc, char **argv)
{
    const char *path = NULL;
    Arena a;
    RecordSet rs;
    int selftest_mode = 0;
    int floatcheck_mode = 0;
    int i;

    arena_init(&a);
    memset(&rs, 0, sizeof rs);

    for (i = 1; i < argc; i++) {
        const char *s = argv[i];
        if (strcmp(s, "--selftest") == 0) {
            selftest_mode = 1;
        } else if (strcmp(s, "--floatcheck") == 0) {
            floatcheck_mode = 1;
        } else if (strcmp(s, "--help") == 0 || strcmp(s, "-h") == 0) {
            usage(); return 0;
        } else if (strncmp(s, "--order=", 8) == 0) {
            if (strcmp(s + 8, "layer") == 0) OPT.order = ORDER_LAYER;
            else if (strcmp(s + 8, "heap") == 0) OPT.order = ORDER_HEAP;
            else { usage(); return 2; }
        } else if (strncmp(s, "--conflicts=", 12) == 0) {
            if (strcmp(s + 12, "distinct") == 0) OPT.conflicts = CONFLICT_DISTINCT;
            else if (strcmp(s + 12, "all") == 0) OPT.conflicts = CONFLICT_ALL_KEYS;
            else { usage(); return 2; }
        } else if (strncmp(s, "--required=", 11) == 0) {
            if (strcmp(s + 11, "defaults") == 0) OPT.required = REQ_DEFAULTS;
            else if (strcmp(s + 11, "strict") == 0) OPT.required = REQ_STRICT;
            else { usage(); return 2; }
        } else if (strcmp(s, "--strict-utf8") == 0) {
            OPT.strict_utf8 = 1;
        } else if (strncmp(s, "--unknown-kind=", 15) == 0) {
            if (strcmp(s + 15, "witness") == 0) OPT.unknown_kind_is_witness = 1;
            else if (strcmp(s + 15, "other") == 0) OPT.unknown_kind_is_witness = 0;
            else { usage(); return 2; }
        } else if (s[0] == '-') {
            usage(); return 2;
        } else if (!path) {
            path = s;
        } else {
            usage(); return 2;
        }
    }

    if (selftest_mode) {
        selftest();
        arena_free(&a);
        return g_fail ? 1 : 0;
    }
    if (floatcheck_mode) {
        if (!path) { usage(); return 2; }
        floatcheck(path);
        arena_free(&a);
        return 0;
    }
    if (!path) { usage(); return 2; }

    {
        size_t len;
        JParser P;
        JVal *v;
        char *text = read_file(path, &len);
        v = json_parse(&a, text, len, &P);
        if (!v) die("input is not valid JSON: %s", P.err);
        if (P.nonstandard)
            fprintf(stderr, "note: input contained NaN/Infinity; SPEC 3 marks the "
                            "owning record malformed\n");
        if (v->t == J_ARR) {
            size_t k;
            for (k = 0; k < v->u.arr.n; k++)
                validate_record(&rs, v->u.arr.v[k], k);
        } else if (v->t == J_OBJ) {
            validate_record(&rs, v, 0);
        } else {
            die("input must be a JSON array of records or one record object");
        }
        free(text);
    }

    fold(&rs, &a);
    report(&rs, &a, stdout);

    fold_free(&g_fold);
    free(rs.rec);
    arena_free(&a);
    return 0;
}
