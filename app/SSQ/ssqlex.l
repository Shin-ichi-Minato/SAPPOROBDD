%x src

%{
// SSQ Parser(Lex) (v0.1)
// Shin-ichi MINATO (May 17, 2015)

#include <cstring>
#include "SeqBDD.h"
#include "y.tab.h"
using namespace std;

static const int	textMax = 256;

static void StrSave();
void StrSave()
{
	yylval.name.str = new char[yyleng+1];
	yylval.name.len = yyleng;
	strcpy(yylval.name.str, yytext);
}

YY_BUFFER_STATE lastbuf;

%}

%%

"#"[^\n]*"\n" { return RETURN; }
"\\\n" { ; }
[ \t] { ; }
";" { return SEMICOLON; }
"\n"   { return RETURN; }
"\r\n"   { return RETURN; }

symbol { return SYMBOL; }

print { return PRINT; }

source { BEGIN(src); }

exit { return EXIT; }
quit { return EXIT; }
help { return HELP; }

"," { return COMMA; }
"=" { return ASSIGN; }

"&" { return AND; }
"+" { return PLUS; }
"-" { return MINUS; }

"/" { return QUOTIENT; }
"%" { return REMAINDER; }

"?" { return QUESTION; }
":" { return COLON; }

"(" { return LPAREN; }
")" { return RPAREN; }

[a-z][a-zA-Z0-9_]* { StrSave(); return IDVAR; }
[A-Z][a-zA-Z0-9_]* { StrSave(); return IDFUNC; }
[0-9]+             { StrSave(); return NUMBER; }
\"[^\"^\n]+\" { StrSave(); return FNAME; }
.   { return UNKNOWN; }

<src>[ \t] { ; }
<src>"\\\n" { ; }
<src>"\n" {
  fprintf(stderr, "syntax error.\n");
  BEGIN(INITIAL);
}
<src>[^ \t\n]+ {
  if(lastbuf) fprintf(stderr, "nested source command.\n");
  else
  {
    FILE* fp = fopen(yytext, "r");
    if(!fp) fprintf(stderr, "can't open the file: %s\n", yytext);
    else
    {
      lastbuf = YY_CURRENT_BUFFER;
      yy_switch_to_buffer(yy_create_buffer(fp, YY_BUF_SIZE));
    }
  }
  BEGIN(INITIAL);
}

<<EOF>> {
  if(lastbuf == 0) yyterminate();
  else
  {
    yy_delete_buffer(YY_CURRENT_BUFFER);
    yy_switch_to_buffer(lastbuf);
    lastbuf = 0;
  }
}

%%
