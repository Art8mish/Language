#ifndef RUS_TRANS_H_INCLUDED
#define RUS_TRANS_H_INCLUDED

const int   RUS_ALPH_LEN = 33;
const int MAX_LETTER_LEN = 4;

static const char *const RUS_LOW_TRANSLIT[RUS_ALPH_LEN] = 
{"a",  "b",  "v",  "g",  "d",   "ye", "zh", "z", "i",  "yj",
 "k",  "l",  "m",  "n",  "o",    "p",  "r",  "s",  "t", "u",  "f",
 "kh", "tc", "ch", "sh", "shch", "", "y",  "'",  "e", "yu", "ya"};

static const char *const RUS_HIGH_TRANSLIT[RUS_ALPH_LEN] = 
{"A",  "B",  "V",  "G",  "D",  "YE", "ZH", "Z", "I",  "YJ",
 "K",  "L",  "M",  "N",  "O",    "P",  "R",  "S",  "T", "U",  "F",
 "KH", "TC", "CH", "SH", "SHCH", "", "Y",  "'",  "E", "YU", "YA"};

static const char *const HIGH_YO = "YO";
static const char *const  LOW_YO = "yo";

const char *RusTranslate(const char *str);

#endif //RUS_TRANS_H_INCLUDED