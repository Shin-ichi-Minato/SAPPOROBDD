%x src

%{
// VSOP Parser(Lex) (v1.36)
// Shin-ichi MINATO (Dec. 18, 2010)

#include <string.h>
#include "CtoI.h"
#include "y.tab.h"

static const int	textMax = 256;

static void StrSave();
void StrSave()
{
	yylval.name.str = new char[yyleng+1];
	yylval.name.len = yyleng;
	strcpy(yylval.name.str, yytext);
}

YY_BUFFER_STATE lastbuf = 0;

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
exit { return EXIT; }
quit { return EXIT; }
help { return HELP; }
source { BEGIN(src); }

"."MaxVal { return MAXVAL; }
"."MinVal { return MINVAL; }
"."TotalVal { return TOTALVAL; }
"."Items { return ITEMS; }
"."Always { return ALWAYS; }
"."Restrict { return RESTRICT; }
"."Permit { return PERMIT; }
"."PermitSym { return PERMITSYM; }
"."TermsEQ { return TEQ; }
"."TermsNE { return TNE; }
"."TermsGT { return TGT; }
"."TermsGE { return TGE; }
"."TermsLT { return TLT; }
"."TermsLE { return TLE; }
"."FreqPatA { return FPA; }
"."FreqPatAV { return FPAV; }
"."FreqPatM { return FPM; }
"."FreqPatC { return FPC; }
"."SymGrp { return SYMGRP; }
Import { return IMPORT; }
Lcm { return LCM; }
autoexport { return AUTOEXPORT; }

"?" { return QUESTION; }
"," { return COMMA; }
"." { return PERIOD; }

"=" { return ASSIGN; }

"+" { return PLUS; }
"-" { return MINUS; }
"*" { return MULTIPLY; }
"/" { return QUOTIENT; }
"%" { return REMAINDER; }
"@" { return MEET; }

"==" { return EQ; }
"!=" { return NE; }
">"  { return GT; }
">=" { return GE; }
"<"  { return LT; }
"<=" { return LE; }

":" { return COLON; }

"(" { return LPAREN; }
")" { return RPAREN; }
"[" { return LBRACKET; }
"]" { return RBRACKET; }

[a-z][a-zA-Z0-9_]* { StrSave(); return IDVAR; }
[A-Z][a-zA-Z0-9_]* { StrSave(); return IDFUNC; }
[0-9]+             { StrSave(); return NUMBER; }
0[xX][0-9a-fA-F]+  { StrSave(); return NUMBER; }
0[bB][01]+         { StrSave(); return NUMBER; }
[0-9]*"."[0-9]+    { StrSave(); return RNUM; }
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
