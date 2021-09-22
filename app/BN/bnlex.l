%{
// BN Parser(Lex) (ver0.1)
// Shin-ichi MINATO (Nov. 4, 2007)

#include <string.h>
#include "bn.h"
#include "y.tab.h"

static const int	textMax = 256;

static void StrSave();
void StrSave()
{
	bnlval.name.str = new char[yyleng+1];
	bnlval.name.len = yyleng;
	strcpy(bnlval.name.str, yytext);
}

%}

%%


";" { return SEMICOLON; }
"{" { return LBRACE; }
"}" { return RBRACE; }
"[" { return LBRACKET; }
"]" { return RBRACKET; }
"(" { return LPAREN; }
")" { return RPAREN; }

network { return NETWORK; }
variable { return VARIABLE; }
property { while(yyinput() != ';'); }
type { return TYPE; }
discrete { return DISCRETE; }
probability { return PROBABILITY; }
order { return ORDER; }
table { return TABLE; }

[a-zA-Z0-9_\-\%\.]+ { StrSave(); return ID; }
[01]"."[0-9]+    { StrSave(); return ID; }
[0-9]"."[0-9]+E"-"[0-9]+  { StrSave(); return ID; }
[0-9]E"-"[0-9]+  { StrSave(); return ID; }
\"[^\"\n]+\" { StrSave(); return ID; }
[\|,= \t\n] { }
"//" { while(yyinput() != '\n'); }
. { return UNKNOWN; }
%%
